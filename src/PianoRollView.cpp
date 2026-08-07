/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : piano_roll_view.cpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#include <PianoRollView>

#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>

#include <cmath>

PianoRollView::PianoRollView(QWidget *parent) : QGraphicsView(parent) {
	m_scene = new QGraphicsScene(this);

	setScene      (m_scene);
	setRenderHint (QPainter::Antialiasing);

	m_scene->setSceneRect(0, 0, 5000, 128 * NOTE_HEIGHT);
}

void PianoRollView::populate(const std::vector<JAudio::BMS::BMSNoteEvent> &notes) {
	m_scene->clear();

	for (const JAudio::BMS::BMSNoteEvent &note : notes) {
		float x = note.start        * TICK_WIDTH_MULTIPLIER;
		float y = (127 - note.note) * NOTE_HEIGHT;
		float w = note.duration     * TICK_WIDTH_MULTIPLIER;
		float h =                     NOTE_HEIGHT;

		QGraphicsRectItem *rect = m_scene->addRect(x, y, w, h);

		QColor trackColors[8] = { Qt::red, Qt::blue, Qt::green, Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkRed, Qt::darkBlue };
		
		rect->setBrush (QBrush (trackColors[note.track % 8]));
		rect->setPen   (QPen   (Qt::black));
		
		std::string noteNames[12] = { "C♮", "C♯/D♭", "D♮", "D♯/E♭", "E♮", "F♮", "F♯/G♭", "G♮", "G♯/A♭", "A♮", "A♯/B♭", "B♮" };

		rect->setToolTip(
			QString("%1%2\nVoice: %3\nVelocity: %4")
				.arg(noteNames[note.note % 12])
				.arg(std::floor(note.note / 12) - 1)
				.arg(note.voice)
				.arg(note.velocity)
		);
	}
}