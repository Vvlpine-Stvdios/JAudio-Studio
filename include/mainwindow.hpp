/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : mainwindow.hpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/

#pragma once

#include <DropZone>
#include <PianoRoll>
#include <AutomationTab>

#include <QFileInfo>
#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QProgressBar>
#include <QSplitter>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>

#include <JAudio/BMS/Parser>
#include <JAudio/BMS/MIDIExporter>

class MainWindow : public QMainWindow {

	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = nullptr);
	   ~MainWindow() override = default;

	    void open(const QString &filePath);
	
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
		AutomationTab  *m_automationTab;

		QHBoxLayout *m_controlLayout;

		QComboBox      *m_keySignatureBox;
		QSpinBox       *m_timeSignatureNumerator;
		QComboBox      *m_timeSignatureDenominator;
		QComboBox      *m_trackList;
		QStackedWidget *m_trackDataDisplay;
		QLabel         *m_tempo;

};