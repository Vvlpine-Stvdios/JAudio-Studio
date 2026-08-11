/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : piano_roll_view.hpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

#include <map>

#include <JAudio/BMS/Parser>

class PianoRollScene : public QGraphicsScene {

	Q_OBJECT

	public:
		explicit PianoRollScene(QWidget *paren = nullptr);
	
		void setPPQN(int ppqn);

	protected:
		void drawBackground(QPainter *painter, const QRectF &rect) override;

	private:
		// 120 seems to be basically what every file has
		int m_ppqn = 120;

		const int NOTE_HEIGHT           = 10;
		const int TICK_WIDTH_MULTIPLIER = 1.0f;

};

class PianoRoll : public QWidget {

	Q_OBJECT

	public:
		explicit PianoRoll(QWidget *parent = nullptr);

		void populate (const std::vector<JAudio::BMS::NoteEvent> &notes, const std::map<int, int> &tempoMap);
		void setPPQN  (int ppqn);
	
	private:
		void setupUI        ();
		void syncScrollBars ();

		QGraphicsView *m_keyboardView;
		QGraphicsView *m_tempoTrackView;
		QGraphicsView *m_pianoRollView;

		QGraphicsScene *m_keyboardScene;
		QGraphicsScene *m_tempoTrackScene;
		PianoRollScene *m_pianoRollScene;

		const int NOTE_HEIGHT           = 10;
		const int KEYBOARD_WIDTH        = 60;
		const int TEMPO_TRACK_HEIGHT    = 40;
		const int TICK_WIDTH_MULTIPLIER = 1.0f;

};