/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/10
 * 	File    : bulkconverterdialog.hpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#pragma once

#include <QDialog>
#include <QListWidget>
#include <QProgressBar>
#include <QLineEdit>
#include <QPushButton>

class BulkConverterDialog : public QDialog {

	Q_OBJECT

	public:
		explicit BulkConverterDialog(QWidget *parent = nullptr);

	private slots:
		void addFiles        ();
		void removeSelected  ();
		void browseOutput    ();
		void startConversion ();
	
	private:
		QListWidget  *m_fileList;
		QLineEdit    *m_outputDirectoryEdit;
		QProgressBar *m_progressBar;
		QPushButton  *m_convertButton;

};