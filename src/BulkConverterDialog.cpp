/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/10
 * 	File    : BulkConverterDialog.cpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vvlpine Stvdios
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
#include <JAudio/AFC/Decoder>
#include <JAudio/AFC/WAVEExporter>

#include <string>
#include <iostream>
#include <variant>

BulkConverterDialog::BulkConverterDialog(QWidget *parent) : QDialog(parent) {
	setWindowTitle ("Bulk Convert Files");
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

	QHBoxLayout *outputLayout = new QHBoxLayout();
	QPushButton *browseButton = new QPushButton("Browse...", this);

	m_outputDirectoryEdit = new QLineEdit(this);
	m_outputDirectoryEdit->setText("./bin/exports/");
	
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

	QHBoxLayout *bottomLayout = new QHBoxLayout();
	QPushButton *closeButton  = new QPushButton("Close", this);

	m_convertButton = new QPushButton("Convert", this);
	m_convertButton->setDefault(true);

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
		"Select JAudio Files",
		"",
		"BMS Files (*.bms);AFC Files (*.afc)"
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

	JAudio::BMS::Parser   BMSParser;
	MIDI       ::Exporter MIDIExporter;

	JAudio::AFC::Decoder      AFCDecoder;
	PCM        ::WAVEExporter PCMWAVEExporter;

	std::cout << (int)m_fileList->count() << std::endl;

	for (int i = 0; i < m_fileList->count(); i++) {
		const QString &filePath = m_fileList->item(i)->text();
		QFileInfo      info     = QFileInfo(filePath);

		JAudio::Core::ParsedData parsedData;

		if (info.suffix() == "bms") {
			QString outPath = QString("%1/mid/%2.mid")
				.arg(m_outputDirectoryEdit->text())
				.arg(info.baseName());
			
			if (!QDir().mkpath(QString("%1/%2").arg(m_outputDirectoryEdit->text()).arg("mid"))) {
				QMessageBox::warning(this, "Failed to create `mid` folder!", "Please make sure this app can modify the export directory.");
				return;
			}
			
			BMSParser.loadFromFile(filePath.toStdString(), parsedData);
			std::vector<MIDI::TrackInfo> infos = { };

			if (!std::holds_alternative<JAudio::BMS::Sequence>(parsedData)) {
				std::cout << "???" << std::endl;
				return;
			}

			JAudio::BMS::Sequence sequence = std::get<JAudio::BMS::Sequence>(parsedData);

			for (const int &track : sequence.tracks){
				if (track != 0xFF && track != -1) {
					infos.push_back((MIDI::TrackInfo) { "Track " + std::to_string(track), false });
				}
			}

			MIDIExporter.exportToFile(outPath.toStdString(), sequence, infos);

		} else if (info.suffix() == "afc") {
			QString outPath = QString("%1/wav/%2.wav")
				.arg(m_outputDirectoryEdit->text())
				.arg(info.baseName());
			
			if (!QDir().mkpath(QString("%1/%2").arg(m_outputDirectoryEdit->text()).arg("wav"))) {
				QMessageBox::warning(this, "Failed to create `wav` folder!", "Please make sure this app can modify the export directory.");
				return;
			}

			AFCDecoder.loadFromFile(filePath.toStdString(), parsedData);
			
			if (!std::holds_alternative<JAudio::AFC::Stream>(parsedData)) {
				std::cout << "???" << std::endl;
				return;
			}

			JAudio::AFC::Stream      stream = std::get<JAudio::AFC::Stream>(parsedData);
			JAudio::Core::ExportData nothing;

			PCMWAVEExporter.exportToFile(outPath.toStdString(), stream, nothing);
		}

		m_progressBar->setValue(m_progressBar->value() + 1);

		std::cout << "Successfully converted " << info.baseName().toStdString() << std::endl;
	}

	QMessageBox::information(this, "Success!", "Successfully converted all files!");
	close();
}