/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/11
 * 	File    : AutomationTab.cpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/

#include <AutomationTab>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPainterPath>
#include <QGraphicsEllipseItem>
#include <QRandomGenerator>

#include <iostream>

AutomationTab::AutomationTab(QWidget *parent) : QWidget(parent) {
	setupUI();
}

void AutomationTab::setupUI() {
	QVBoxLayout *mainLayout   = new QVBoxLayout(this);
	QHBoxLayout *topBarLayout = new QHBoxLayout();

	m_automationSelector = new QComboBox(this);
	m_automationSelector->addItem("All");
	
	QPushButton *left  = new QPushButton("<", this);
	QPushButton *right = new QPushButton(">", this);

	topBarLayout->addWidget  (left);
	topBarLayout->addWidget  (m_automationSelector);
	topBarLayout->addWidget  (right);
	topBarLayout->addStretch ();

	m_scene = new PianoRollScene (this);
	m_view  = new QGraphicsView  (m_scene, this);

	m_view->setFrameShape                (QFrame::NoFrame);
	m_view->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOn);
	m_view->setVerticalScrollBarPolicy   (Qt::ScrollBarAlwaysOn);

	mainLayout->addLayout(topBarLayout);
	mainLayout->addWidget(   m_view   );

	connect(left, &QPushButton::clicked, [this]() {
		m_automationSelector->setCurrentIndex((m_automationSelector->currentIndex() - 1) % m_automationSelector->count());
	});

	connect(right, &QPushButton::clicked, [this]() {
		m_automationSelector->setCurrentIndex((m_automationSelector->currentIndex() + 1) % m_automationSelector->count());
	});
	
	connect(m_automationSelector, &QComboBox::currentIndexChanged, [this]() { populate(m_automationData, m_trackSolo, m_view->sceneRect().width()); });
}

void AutomationTab::populate(const std::map<u8, std::map<u8, std::vector<JAudio::BMS::AutomationEvent>>> &automationData, int trackSolo, int width) {
	m_automationData = automationData;
	m_trackSolo      = trackSolo;

	m_scene->clear();

	bool OK;
	int  automationSolo = m_automationSelector->currentText().right(3).left(2).toUInt(&OK, 16);

	if (!OK) { automationSolo = -1; }

	QBrush backgroundBrush("#282828");

	// Parse event stack container per track
	for (const auto &[trackNumber, trackAutomationData] : m_automationData) {
		if (trackNumber != m_trackSolo && m_trackSolo != -1) { continue; }
	
		// Parse event stack per event stack container
		for (const auto &[opcode, events] : trackAutomationData) {
			if (opcode != automationSolo && automationSolo != -1) { continue; }

			QString automationName = QString("%1").arg(opcode, 2, 16, '0').toUpper();

			if (m_automationSelector->findText (automationName) == -1) {
				m_automationSelector->addItem  (automationName);
			}

			std::array<QPoint, 4> previousPoints = { };
			previousPoints.fill(QPoint(-1, -1));

			// Parse event per event stack
			for (const JAudio::BMS::AutomationEvent &event : events) {
				// std::cout << (int)event.start << ": " << (int)opcode << std::endl;

				std::vector<u32> args = { };

				switch (opcode) {
					case JAudio::BMS::CMD_PARAM_SET:
						args.push_back(event.args[0]);
						args.push_back((u32)(static_cast<s16>(event.args[1]) << 8) | static_cast<u16>(event.args[2]));
						break;

					case JAudio::BMS::CMD_PARAM_SET_OVER_TIME:
						args.push_back(event.args[0]);
						args.push_back((u32)(static_cast<s16>(event.args[1]) << 8) | static_cast<u16>(event.args[2]));
						args.push_back(event.args[3]);
						break;

					default:
						for (const int &arg : event.args) { args.push_back(static_cast<u8>(arg)); }
						break;
				}

				// Parse argument per event
				for (size_t i = 4; const u32 &arg : args) {
					QPen pen = QPen(QColor::fromHsv((opcode * 37) % 360, 200, 64 * i - 1));

					int x = event.start * TICK_WIDTH_MULTIPLIER;
					int y = m_scene->height() - (arg / (float)UINT8_MAX * m_scene->height());

					QString tooltip = QString("Track %1\n%2 ").arg(trackNumber).arg(opcode, 2, 16, '0');

					for (const u32 &a : args) { tooltip += (a == arg) ? QString("<b>%1</b> ").arg(a, 2, 16, '0') : QString("%1 ").arg(a, 2, 16, '0'); }

					if (previousPoints[i].x() > -1 && previousPoints[i].y() > -1 && previousPoints[i].x() <= width) {
						QGraphicsLineItem *hLine = m_scene->addLine(previousPoints[i].x(), previousPoints[i].y(), x, previousPoints[i].y(), pen);
						QGraphicsLineItem *vLine = m_scene->addLine(x,                     previousPoints[i].y(), x, y,                     pen);

						hLine->setToolTip(tooltip.toUpper());
						vLine->setToolTip(tooltip.toUpper());
					}

					// If it's the last element
					if (&event == &events.back()) {
						QGraphicsLineItem *hLine = m_scene->addLine(x, y, width, y, pen);

						hLine->setToolTip(tooltip.toUpper());
					}
					
					previousPoints[i] = QPoint(event.start * TICK_WIDTH_MULTIPLIER, y);

					QGraphicsEllipseItem *dot = m_scene->addEllipse(x - 3, y - 3, 6, 6, pen, backgroundBrush);
					dot->setToolTip(tooltip.toUpper());

					i--;
				}
			}
		}
	}

	update();
}

void AutomationTab::setPPQN(int ppqn) {
	m_scene->setPPQN(ppqn);
}

void AutomationTab::setTimeSignature(int numerator, int denominator) {
	m_scene->setTimeSignature(numerator, denominator);
}