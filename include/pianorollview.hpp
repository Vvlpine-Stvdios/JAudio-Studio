/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : piano_roll_view.hpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>

#include <JAudio/BMS/Parser>

class PianoRollView : public QGraphicsView {

	Q_OBJECT

	public:
		PianoRollView(QWidget *parent = nullptr);
		void populate(const std::vector<JAudio::BMS::BMSNoteEvent> &notes);

	private:
		QGraphicsScene *m_scene;

		// UI Constants
		const int   NOTE_HEIGHT           = 10;
		const float TICK_WIDTH_MULTIPLIER = 2.0f;

};