/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : main_window.hpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#pragma once

#include <DropZone>
#include <PianoRoll>

#include <QFileInfo>
#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QProgressBar>
#include <QSplitter>
#include <QStackedWidget>

#include <JAudio/BMS/Parser>

class MainWindow : public QMainWindow {

	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = nullptr);
	   ~MainWindow() override = default;
	
	private slots:
		void onFileSelected  (const QString &filePath);
		void openFileBrowser ();
		void onFileParsed    ();

	private:
		void setupUI();

		JAudio::BMS::Parser m_parser;

		QFutureWatcher<bool> m_watcher;
		QFileInfo            m_fileInfo;

		QStackedWidget *m_stackedWidget;
		QProgressBar   *m_progressBar;
		PianoRoll      *m_pianoRoll;

		QHBoxLayout *m_controlLayout;

};