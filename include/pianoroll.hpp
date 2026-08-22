/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : pianoroll.hpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#pragma once

#include <QWidget>
#include <QScrollBar>
#include <QGraphicsView>
#include <QGraphicsScene>

#include <map>

#include <JAudio/BMS/Parser>

class PianoRollScene : public QGraphicsScene {

	Q_OBJECT

	public:
		explicit PianoRollScene(QWidget *parent = nullptr);
	
		void setPPQN          (int ppqn);
		void setTimeSignature (int numerator, int denominator);

	protected:
		void drawBackground(QPainter *painter, const QRectF &rect) override;

	private:
		// 120 seems to be basically what every file has
		int m_ppqn             = 120;
		int m_timeSignature[2] = { 4, 4 };

		const int NOTE_HEIGHT           = 10;
		const int TICK_WIDTH_MULTIPLIER = 1.0f;

};

class PianoRoll : public QWidget {

	Q_OBJECT

	public:
		explicit PianoRoll(QWidget *parent = nullptr);

		void populate         (const std::vector<JAudio::BMS::NoteEvent> &notes, int trackSolo = -1);
		void setPPQN          (int ppqn);
		void setTimeSignature (int numerator, int denominator);

		constexpr QScrollBar *getHorizontalScrollBar () const { return m_pianoRollView->horizontalScrollBar (); }
		constexpr QScrollBar *getVerticalScrollBar   () const { return m_pianoRollView->verticalScrollBar   (); }

		int                   getSceneWidth          () const { return m_pianoRollView->sceneRect().width(); }
	
	private:
		void setupUI        ();
		void drawKeyboard   ();
		void syncScrollBars ();

		QGraphicsView *m_keyboardView;
		QGraphicsView *m_pianoRollView;

		QGraphicsScene *m_keyboardScene;
		PianoRollScene *m_pianoRollScene;

		const int NOTE_HEIGHT           = 10;
		const int KEYBOARD_WIDTH        = 60;
		const int TICK_WIDTH_MULTIPLIER = 1.0f;

};