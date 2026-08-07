/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : main_window.hpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#pragma once

#include <QMainWindow>
#include <JAudio/BMS/Parser>

class MainWindow : public QMainWindow {

	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = nullptr);
	   ~MainWindow() override = default;
	
	private slots:
		void openBMSFile();

	private:
		JAudio::BMS::BMSParser m_parser;

};