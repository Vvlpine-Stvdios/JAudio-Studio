/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/10
 * 	File    : BulkConverterDialog.cpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/

#include <BulkConverterDialog>

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

#include <JAudio/BMS/Parser>
#include <JAudio/BMS/MIDIExporter>

BulkConverterDialog::BulkConverterDialog(QWidget *parent) : QDialog(parent) {
	setWindowTitle ("Bulk Convert BMS to MIDI");
	resize         (500, 400);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	// 
	// FILE LIST
	// 
	mainLayout->addWidget(new QLabel("Source BMS Files", this));

	m_fileList = new QListWidget();
	m_fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	mainLayout->addWidget(m_fileList);

	QHBoxLayout *listButtonsLayout = new QHBoxLayout();
	QPushButton *addButton         = new QPushButton("Add Files");
	QPushButton *removeButton      = new QPushButton("Remove Selected");

	listButtonsLayout->addWidget  ( addButton  );
	listButtonsLayout->addWidget  (removeButton);
	listButtonsLayout->addStretch ();

	mainLayout->addLayout(listButtonsLayout);

	// 
	// OUTPUT DIRECTORY
	// 
	mainLayout->addWidget(new QLabel("Destination Folder", this));

	QVBoxLayout *outputLayout = new QVBoxLayout();
	QPushButton *browseButton = new QPushButton("Browse...", this);

	m_outputDirectoryEdit = new QLineEdit(this);
	
	outputLayout->addWidget(m_outputDirectoryEdit);
	outputLayout->addWidget(    browseButton     );

	mainLayout->addLayout(outputLayout);

	// 
	// PROGRESS
	// 
	m_progressBar = new QProgressBar(this);
	m_progressBar->setValue(0);

	mainLayout->addWidget(m_progressBar);

	// 
	// CONVERSION
	// 

	QVBoxLayout *bottomLayout = new QVBoxLayout();
	QPushButton *closeButton  = new QPushButton("Close", this);

	m_convertButton = new QPushButton("Convert", this);

	bottomLayout->addStretch ();
	bottomLayout->addWidget  (  closeButton  );
	bottomLayout->addWidget  (m_convertButton);
	
	mainLayout->addLayout(bottomLayout);

	// 
	// CONNECTIONS
	// 
	connect(addButton,       &QPushButton::clicked, this, &BulkConverterDialog::addFiles       );
	connect(removeButton,    &QPushButton::clicked, this, &BulkConverterDialog::removeSelected );
	connect(browseButton,    &QPushButton::clicked, this, &BulkConverterDialog::browseOutput   );
	connect(m_convertButton, &QPushButton::clicked, this, &BulkConverterDialog::startConversion);
	connect(closeButton,     &QPushButton::clicked, this, &BulkConverterDialog::close          );
}

void BulkConverterDialog::addFiles() {
	QStringList files = QFileDialog::getOpenFileNames(
		this,
		"Select BMS Files",
		"",
		"BMS Files (*.bms)"
	);

	m_fileList->addItems(files);
}

void BulkConverterDialog::removeSelected() {
	qDeleteAll(m_fileList->selectedItems());
}

void BulkConverterDialog::browseOutput() {
	QString directory = QFileDialog::getExistingDirectory(this, "Select Output Folder");

	if (!directory.isEmpty()) {
		m_outputDirectoryEdit->setText(directory);
	}
}

void BulkConverterDialog::startConversion() {
	if (m_fileList->count() == 0) {
		QMessageBox::warning(this, "No Files to Convert!", "Please add files to convert.");
		return;
	}

	if (m_outputDirectoryEdit->text().isEmpty()) {
		QMessageBox::warning(this, "No Output Folder!", "Please specify a folder to put the converted files into.");
		return;
	}

	m_progressBar->setMaximum (m_fileList->count());
	m_progressBar->setValue   (0);

	JAudio::BMS::Parser   parser;
	MIDI       ::Exporter exporter;

	for (int i = 0; i < m_fileList->count(); i++) {
		const QString &filePath = m_fileList->item(i)->text();
		QFileInfo      info     = QFileInfo(filePath);
		QString        outPath  = QString("%1/%2.mid")
			.arg(m_outputDirectoryEdit->text())
			.arg(info.fileName().replace(".bms", ""));

		parser  .loadFromFile(filePath.toStdString());
		exporter.exportToFile(outPath .toStdString(), parser);

		m_progressBar->setValue(m_progressBar->value() + 1);
	}

	close();
}