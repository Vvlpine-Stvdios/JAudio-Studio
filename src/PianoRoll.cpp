/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : PianoRoll.cpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#include <PianoRoll>

#include <QGridLayout>
#include <QScrollBar>
#include <QPainter>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>

#include <cmath>
#include <iostream>

//
// PIANO ROLL SCENE CLASS
//

PianoRollScene::PianoRollScene(QWidget *parent) : QGraphicsScene(parent) { }

void PianoRollScene::setPPQN(int ppqn) {
	m_ppqn = ppqn;
	update();
}

void PianoRollScene::setTimeSignature(int numerator, int denominator) {
	m_timeSignature[0] = numerator;
	m_timeSignature[1] = denominator;
	update();
}

void PianoRollScene::drawBackground(QPainter *painter, const QRectF &rect) {
	painter->fillRect(rect, QColor(40, 40, 40));

	QPen horizontalPen (QColor(60, 60, 60));
	QPen verticalPen   (QColor(80, 80, 80));
	QPen barlinePen    (QColor(90, 90, 90));
	verticalPen.setStyle(Qt::DashLine);
	barlinePen .setWidth(2);

	painter->setPen(horizontalPen);
	int topY = int(rect.top()) - (int(rect.top()) % NOTE_HEIGHT);

	for (int y = topY; y < int(rect.bottom()); y += NOTE_HEIGHT) {
		painter->drawLine(rect.left(), y, rect.right(), y);
	}

	// 2. Draw Vertical Lines (Scaled to match time signature)
	painter->setPen(verticalPen);
	int beatWidth = m_ppqn * (m_timeSignature[1] / 4.0f) * TICK_WIDTH_MULTIPLIER;
	int leftX     = (int)rect.left() - ((int)rect.left() % beatWidth);

	for (int x = leftX; x < int(rect.right()); x += beatWidth) {
		if ((x / 120) % m_timeSignature[0] == 0) { painter->setPen(barlinePen ); }
		else                                { painter->setPen(verticalPen); }
		
		painter->drawLine(x, rect.top(), x, rect.bottom());
	}
}

//
// PIANO ROLL CLASS
//

PianoRoll::PianoRoll(QWidget *parent) : QWidget(parent) {
	setupUI        ();
	syncScrollBars ();
}


void PianoRoll::setupUI() {
	QGridLayout *grid = new QGridLayout(this);

	grid->setSpacing         (0);
	grid->setContentsMargins (0, 0, 0, 0);

	m_keyboardScene   = new QGraphicsScene(this);
	m_pianoRollScene  = new PianoRollScene(this);
	m_tempoTrackScene = new QGraphicsScene(this);

	m_keyboardView   = new QGraphicsView(m_keyboardScene,   this);
	m_pianoRollView  = new QGraphicsView(m_pianoRollScene,  this);
	m_tempoTrackView = new QGraphicsView(m_tempoTrackScene, this);

	m_keyboardView->setFrameShape                (QFrame::NoFrame);
	m_keyboardView->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
	m_keyboardView->setVerticalScrollBarPolicy   (Qt::ScrollBarAlwaysOff);
	m_keyboardView->setFixedWidth                (KEYBOARD_WIDTH);

	m_tempoTrackView->setFrameShape                (QFrame::NoFrame);
	m_tempoTrackView->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
	m_tempoTrackView->setVerticalScrollBarPolicy   (Qt::ScrollBarAlwaysOff);
	m_tempoTrackView->setFixedHeight               (TEMPO_TRACK_HEIGHT);

	m_pianoRollView->setFrameShape(QFrame::NoFrame);

	grid->addWidget(m_tempoTrackView, 0, 1);
	grid->addWidget(m_keyboardView,   1, 0);
	grid->addWidget(m_pianoRollView,  1, 1);
}

void PianoRoll::syncScrollBars() {
	connect(m_pianoRollView->verticalScrollBar   (), &QScrollBar::valueChanged, m_keyboardView   -> verticalScrollBar   (), &QScrollBar::setValue);
	connect(m_pianoRollView->horizontalScrollBar (), &QScrollBar::valueChanged, m_tempoTrackView -> horizontalScrollBar (), &QScrollBar::setValue);

	connect(m_keyboardView   -> verticalScrollBar   (), &QScrollBar::valueChanged, m_pianoRollView->verticalScrollBar   (), &QScrollBar::setValue);
	connect(m_tempoTrackView -> horizontalScrollBar (), &QScrollBar::valueChanged, m_pianoRollView->horizontalScrollBar (), &QScrollBar::setValue);
}

void PianoRoll::populate(const std::vector<JAudio::BMS::NoteEvent> &notes, const std::map<int, int> &tempoMap, int trackSolo) {
	m_pianoRollScene  -> clear();
	m_keyboardScene   -> clear();
	m_tempoTrackScene -> clear();

	int totalHeight = 128 * NOTE_HEIGHT;
	int totalWidth  = 5000;

	// 
	// PIANO ROLL
	// 

	for (const JAudio::BMS::NoteEvent &note : notes) {
		if (note.track != trackSolo && trackSolo != -1) { continue; }

		float x = note.start        * TICK_WIDTH_MULTIPLIER;
		float y = (127 - note.note) * NOTE_HEIGHT;
		float w = note.duration     * TICK_WIDTH_MULTIPLIER;
		float h =                     NOTE_HEIGHT;

		// Might change so it doesn't start updating every frame...
		if (x + w > totalWidth) {
			totalWidth = x + w;
		}

		QColor trackColors[8] = { Qt::green, Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkRed, Qt::darkBlue, Qt::red, Qt::blue };

		QGraphicsRectItem *rect = m_pianoRollScene->addRect(
			x, y, w, h,
			QPen   (Qt::black),
			QBrush (trackColors[note.track % 8])
		);
		
		std::string noteNames[12] = { "C♮", "C♯/D♭", "D♮", "D♯/E♭", "E♮", "F♮", "F♯/G♭", "G♮", "G♯/A♭", "A♮", "A♯/B♭", "B♮" };

		rect->setToolTip(
			QString("%1%2\nVoice: %3\nVelocity: %4")
				.arg(noteNames[note.note % 12])
				.arg(note.note / 12 - 1)
				.arg(note.voice)
				.arg(note.velocity)
		);

		QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(QString("%1%2").arg(noteNames[note.note % 12]).arg(note.note / 12 - 1), rect);

		QFont font = text->font();
		font.setPixelSize(NOTE_HEIGHT - 2);
		text->setFont(font);

		text->setBrush ((note.track % 8 < 4) ? Qt::black : Qt::white);
		text->setPos   (x + 2, y + 1);

		rect->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
	}

	m_pianoRollScene  -> setSceneRect(0, 0, totalWidth,     totalHeight);
	m_keyboardScene   -> setSceneRect(0, 0, KEYBOARD_WIDTH, totalHeight);
	m_tempoTrackScene -> setSceneRect(0, 0, totalWidth,     TEMPO_TRACK_HEIGHT);

	// 
	// KEYBOARD
	// 

	auto isBlack = [](int note) {
		int n = note % 12;
		return (n == 1 || n == 3 || n == 6 || n == 8 || n == 10);
	};

	auto getY = [](int note, float height) {
		//  0 -> 0
		//  2 -> 1
		//  4 -> 2
		//  5 -> 3
		//  7 -> 4
		//  9 -> 5
		// 11 -> 6
		// => ⎡n / 2⎤
		int n = std::ceil((note % 12) * 0.5f);
		int o = note / 12; // basically the same as flooring the result of float division

		float y = height * 12 * o;

		for (int i = 0; i < n; i++) {
			y += height * ((i < 3) ? 5 / 3.0f : 7 / 4.0f);
		}

		return y;
	};

	auto getH = [](int note, float height) {
		int n = std::ceil((note % 12) * 0.5f);

		return height * (
			(n < 3) ? 5 / 3.0f : 7 / 4.0f
		);
	};

	int backW  = KEYBOARD_WIDTH * 2 / 3.0f;
	int frontW = KEYBOARD_WIDTH     / 3.0f;

	for (int note = 0; note < 128; note++) {
		// Add the 0.5 for the offset to make a space between adjacent keys
		float y = (127 - note) * NOTE_HEIGHT + 1;

		if (isBlack(note)) {
			m_keyboardScene->addRect(
				0, y, backW, NOTE_HEIGHT - 1,
				QPen   (Qt::NoPen),
				QBrush (QColor("#222"))
			);
		} else {
			m_keyboardScene->addRect(
				0, y, backW + 1, NOTE_HEIGHT - 1,
				QPen   (Qt::NoPen),
				QBrush (QColor("#EEE"))
			);

			// if (note != 0) { continue; }

			int h      =                       getH(note, NOTE_HEIGHT);
			int frontY = (128 * NOTE_HEIGHT) - getY(note, NOTE_HEIGHT) - h + 1;

			// std::cout
			// 	<< note << ": "
			// 	<< std::endl;

			m_keyboardScene->addRect(
				backW + 1, frontY, frontW, h - 1,
				QPen   (Qt::NoPen),
				QBrush (QColor("#EEE"))
			);

			if (note % 12 == 0) {
				// Roland scale (MIDI 60 = C4)
				int octave = (note / 12) - 1;
				QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(QString("C%1").arg(octave));
				
				QFont font = text->font();
				font.setPixelSize(NOTE_HEIGHT - 2);
				text->setFont(font);
				text->setBrush(Qt::black);
				
				// Center the text vertically on the front part of the key
				// float textY = y_front + (bottom - y_front) / 2.0f - (NOTE_HEIGHT - 2) / 2.0f;
				// text->setPos(backW + 2, textY);
				
				m_keyboardScene->addItem(text);
			}
		}
	}

	// 
	// TEMPO TRACK
	// 

	for (const auto &[time, tempo] : tempoMap) {
		float x = time * TICK_WIDTH_MULTIPLIER;
		// y = 0

		QGraphicsLineItem       *line = m_tempoTrackScene->addLine  (x, 0, x, TEMPO_TRACK_HEIGHT, QPen(Qt::lightGray));
		QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem (QString("%1").arg(tempo), line);

		QFont font = text->font();
		font.setPixelSize (NOTE_HEIGHT - 2);
		text->setFont     (font);

		text->setBrush (Qt::lightGray);
		text->setPos   (x + 2, 1);
	}
}

void PianoRoll::setPPQN(int ppqn) {
	m_pianoRollScene->setPPQN(ppqn);
}

void PianoRoll::setTimeSignature(int numerator, int denominator) {
	m_pianoRollScene->setTimeSignature(numerator, denominator);
}