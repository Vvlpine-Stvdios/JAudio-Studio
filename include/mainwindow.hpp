/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : mainwindow.hpp
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
#include <JAudio/BMS/MIDIExporter>

class MainWindow : public QMainWindow {

	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = nullptr);
	   ~MainWindow() override = default;
	
	private slots:
		void onFileSelected    (const QString &filePath);
		void openFileBrowser   ();
		void openExportBrowser ();
		void onFileParsed      ();
		void onFileExported    ();
		void onUIUpdated       ();

		void exportCurrentToMIDI (const QString &filePath);
		void openBulkConverter   ();

	private:
		void setupUI  ();
		void updateUI ();

		JAudio::BMS::Parser   m_parser;
		MIDI       ::Exporter m_exporter;

		QFutureWatcher<bool> m_openWatcher;
		QFutureWatcher<bool> m_exportWatcher;
		QFileInfo            m_fileInfo;

		QStackedWidget *m_stackedWidget;
		QProgressBar   *m_progressBar;
		PianoRoll      *m_pianoRoll;

		QHBoxLayout *m_controlLayout;

};