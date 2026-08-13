/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/11
 * 	File    : automationtab.hpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/

#pragma once

#include <PianoRoll>

#include <QWidget>
#include <QComboBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QScrollBar>

#include <JAudio/BMS/Parser>
#include <JAudio/Core/Types>

#include <map>
#include <vector>

class AutomationTab : public QWidget {

	Q_OBJECT

	public:
		explicit AutomationTab(QWidget *parent = nullptr);

		void populate(const std::map<u8, std::map<u8, std::vector<JAudio::BMS::AutomationEvent>>> &automationData, int trackSolo, int width);

		void setPPQN          (int ppqn);
		void setTimeSignature (int numerator, int denominator);

		constexpr QScrollBar *getHorizontalScrollBar () const { return m_view->horizontalScrollBar (); }
		constexpr QScrollBar *getVerticalScrollBar   () const { return m_view->verticalScrollBar   (); }

	private:
		void setupUI();

		QComboBox      *m_automationSelector;

		// Just steal this 'cause I need the background
		PianoRollScene *m_scene;
		QGraphicsView  *m_view;

		std::map<u8, std::map<u8, std::vector<JAudio::BMS::AutomationEvent>>> m_automationData;
		int m_trackSolo = -1;

		const int TICK_WIDTH_MULTIPLIER = 1.0f;

};