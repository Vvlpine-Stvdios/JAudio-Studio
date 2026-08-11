/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : MainWindow.cpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#include <MainWindow>
#include <BulkConverterDialog>

#include <QAction>
#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include <QtConcurrent>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle ("JAudio Studio");
	resize         (1000, 700);
	setupUI        ();
}

void MainWindow::setupUI() {
	m_stackedWidget = new QStackedWidget(this);
	setCentralWidget(m_stackedWidget);

	// 
	// PAGE 0: FILE IMPORT
	// 

	QWidget        *dropPage        = new QWidget        (this);
	QVBoxLayout    *dropLayout      = new QVBoxLayout    (dropPage);
	DropZoneWidget *dropZone        = new DropZoneWidget (this);
	QVBoxLayout    *innerDropLayout = new QVBoxLayout    (dropZone);
	QLabel         *dropLabel       = new QLabel         ("Drag and Drop JAudio Files!", this);
	QPushButton    *browseButton    = new QPushButton    ("or click here to browse", this);

	browseButton->setCursor(Qt::PointingHandCursor);

	innerDropLayout->addStretch ();
	innerDropLayout->addWidget  (dropLabel);
	innerDropLayout->addWidget  (browseButton, 0, Qt::AlignHCenter);
	innerDropLayout->addStretch ();

	dropLayout->setAlignment (Qt::AlignCenter);
	dropLayout->addWidget    (dropZone);

	m_stackedWidget->addWidget(dropPage);

	// 
	// PAGE 1: LOADING SCREEN
	// 

	QWidget     *loadingPage   = new QWidget     (this);
	QVBoxLayout *loadingLayout = new QVBoxLayout (loadingPage);
	QLabel      *loadingLabel  = new QLabel      ("Loading...", this);

	loadingLabel->setAlignment(Qt::AlignCenter);

	m_progressBar = new QProgressBar(this);
	m_progressBar->setRange(0, 0); // Makes it bounce indefinitely (for now I don't want to have to calculate the actual percentage because I'm lazy)

	loadingLayout->addStretch ();
	loadingLayout->addWidget  (loadingLabel);
	loadingLayout->addWidget  (m_progressBar);
	loadingLayout->addStretch ();

	m_stackedWidget->addWidget(loadingPage);

	// 
	// PAGE 2: Editor
	// 

	QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);

	// Top half (piano roll)
	QWidget     *topHalf   = new QWidget     (this);
	QVBoxLayout *topLayout = new QVBoxLayout (topHalf);

	topLayout->setContentsMargins(0, 0, 0, 0);

	QWidget *controlPanel = new QWidget     (this);
	m_controlLayout       = new QHBoxLayout (controlPanel);
	
	m_controlLayout->setContentsMargins(5, 5, 5, 5);

	QComboBox *keySignatureBox = new QComboBox(this);

	keySignatureBox->addItems({
		"C Major / a minor",  "G Major / e minor",  "D Major / b minor",   "A Major / f♯ minor",
		"E Major / c♯ minor", "B Major / g♯ minor", "G♭ Major / e♭ minor", "D♭ Major / b♭ minor",
		"A♭ Major / f minor", "E♭ Major / c minor", "B♭ Major / g minor",  "F Major / d minor",
	});

	QHBoxLayout *timeSignature = new QHBoxLayout();

	timeSignature->setContentsMargins (0, 0, 0, 0);
	timeSignature->setSpacing         (0);

	QSpinBox  *timeSignatureNumerator   = new QSpinBox  (this);
	QComboBox *timeSignatureDenominator = new QComboBox (this);
	
	timeSignatureNumerator->setRange(1, 64);
	timeSignatureNumerator->setValue(4);

	timeSignatureDenominator->addItems({ "1", "2", "4", "8", "16" });
	timeSignatureDenominator->setCurrentText("4");

	timeSignature->addWidget  (timeSignatureNumerator  );
	timeSignature->addWidget  (new QLabel("/")         );
	timeSignature->addWidget  (timeSignatureDenominator);

	QComboBox *trackList = new QComboBox(this);
	trackList->addItem("All");

	for (const int &track : m_parser.getTracks()) {
		trackList->addItem(QString("%1").arg(track));
	}

	m_controlLayout->addWidget  (new QLabel("Key: "        ));
	m_controlLayout->addWidget  (keySignatureBox            );
	m_controlLayout->addWidget  (new QLabel("Time: "       ));
	m_controlLayout->addLayout  (timeSignature              );
	m_controlLayout->addWidget  (new QLabel("Tempo: -- BPM"));
	m_controlLayout->addWidget  (new QLabel("Track: "      ));
	m_controlLayout->addWidget  (trackList                  );
	m_controlLayout->addStretch ();

	QWidget     *editorArea   = new QWidget     (this);
	QHBoxLayout *editorLayout = new QHBoxLayout (editorArea);

	editorLayout->setContentsMargins (0, 0, 0, 0);
	editorLayout->setSpacing         (0);

	m_pianoRoll = new PianoRoll(this);

	editorLayout->addWidget(m_pianoRoll);

	topLayout->addWidget(controlPanel);
	topLayout->addWidget( editorArea );

	// Bottom half (automation)
	QWidget     *bottomHalf            = new QWidget     (this);
	QVBoxLayout *bottomLayout          = new QVBoxLayout (bottomHalf);
	QLabel      *automationPlaceholder = new QLabel      ("Automation", this);

	automationPlaceholder->setAlignment(Qt::AlignCenter);

	bottomLayout->addWidget(automationPlaceholder);

	mainSplitter->addWidget(topHalf);
	mainSplitter->addWidget(bottomHalf);

	m_stackedWidget->addWidget(mainSplitter);

	// 
	// MENUS
	// 

	QMenu   *fileMenu       = menuBar()->addMenu ("&File");
	QAction *open           = new QAction        ("&Open BMS",       this);
	QAction *exportToMIDI   = new QAction        ("&Export to MIDI", this);
	QMenu   *convertMenu    = menuBar()->addMenu ("&Convert");
	QAction *bulkConvertBMS = new QAction        ("&Bulk Convert BMS to MIDI", this);

	open         -> setShortcut (QKeySequence::Open    );
	exportToMIDI -> setShortcut (QKeySequence("Ctrl+E"));
	fileMenu     -> addAction   (    open    );
	fileMenu     -> addAction   (exportToMIDI);

	bulkConvertBMS -> setShortcut (QKeySequence("Ctrl+B"));
	convertMenu    -> addAction   (bulkConvertBMS);

	// 
	// EVENTS
	// 

	// File stuffs
	connect(open,             &QAction             ::triggered,   this, &MainWindow::openFileBrowser  );
	connect(dropZone,         &DropZoneWidget      ::fileDropped, this, &MainWindow::onFileSelected   );
	connect(browseButton,     &QPushButton         ::clicked,     this, &MainWindow::openFileBrowser  );
	connect(&m_openWatcher,   &QFutureWatcher<bool>::finished,    this, &MainWindow::onFileParsed     );
	connect(exportToMIDI,     &QAction             ::triggered,   this, &MainWindow::openExportBrowser);
	connect(&m_exportWatcher, &QFutureWatcher<bool>::finished,    this, &MainWindow::onFileExported    );
	connect(bulkConvertBMS,   &QAction             ::triggered,   this, &MainWindow::openBulkConverter);

	// UI updates
	connect(keySignatureBox,          &QComboBox::currentIndexChanged, this, &MainWindow::onUIUpdated);
	connect(timeSignatureNumerator,   &QSpinBox ::valueChanged,        this, &MainWindow::onUIUpdated);
	connect(timeSignatureDenominator, &QComboBox::currentIndexChanged, this, &MainWindow::onUIUpdated);
	connect(trackList,                &QComboBox::currentIndexChanged, this, &MainWindow::onUIUpdated);

	// 
	// SET CURRENT WIDGET
	// 

	m_stackedWidget->setCurrentIndex(0);
}

void MainWindow::openFileBrowser() {
	QString filePath = QFileDialog::getOpenFileName(
		this,
		"Open BMS",
		"",
		"BMS File (*.bms);;All Files (*.*)"
	);

	if (!filePath.isEmpty()) {
		onFileSelected(filePath);
		m_fileInfo = QFileInfo(filePath);
	}
}

void MainWindow::onFileSelected(const QString &filePath) {
	m_stackedWidget->setCurrentIndex(1);

	// Push the parsing onto a background thread
	QFuture<bool> future = QtConcurrent::run([this, filePath] () {
		return m_parser.loadFromFile(filePath.toStdString());
	});

	m_openWatcher.setFuture(future);
}

void MainWindow::onFileParsed() {
	bool success = m_openWatcher.result();

	if (success) {
		setWindowTitle    ("JAudio Studio — " + m_fileInfo.fileName());
		setWindowFilePath (m_fileInfo.filePath());

		m_stackedWidget -> setCurrentIndex (2);
		m_pianoRoll     -> setPPQN         (m_parser.getPPQN());

		// 5 should be the track list
		QLayoutItem *trackItem = m_controlLayout->itemAt(6);
		int          track     = -1;

		if (trackItem) {
			QComboBox *trackList = (QComboBox *)trackItem->widget();
			
			for (const int &track : m_parser.getTracks()) {
				trackList->addItem(QString("%1").arg(track));
			}
		}

		updateUI();
	} else {
		m_stackedWidget->setCurrentIndex(1);
		QMessageBox::critical(this, "Error", "Failed to parse file!");
	}
}

void MainWindow::onUIUpdated() {
	updateUI();
}

void MainWindow::openBulkConverter() {
	BulkConverterDialog dialog(this);
	dialog.exec();
}

void MainWindow::updateUI() {
	// 5 should be the track list
	QLayoutItem *trackItem = m_controlLayout->itemAt(6);
	int          track     = -1;

	if (trackItem) {
		bool       OK;
		QComboBox *trackList = (QComboBox *)trackItem->widget();
		track                = trackList->currentText().toInt(&OK);

		if (!OK) { track = -1; }
	}

	// 3 should be the track list
	QLayoutItem *timeSignatureItem = m_controlLayout->itemAt(3);
	int          numerator         = 4;
	int          denominator       = 4;

	if (timeSignatureItem) {
		QLayoutItem *numeratorItem   = timeSignatureItem->layout()->itemAt(0);
		QLayoutItem *denominatorItem = timeSignatureItem->layout()->itemAt(2);
		bool         OK;
		
		if ( numeratorItem ) { numerator   = ((QSpinBox  *)numeratorItem   -> widget())->value       (); }
		if (denominatorItem) { denominator = ((QComboBox *)denominatorItem -> widget())->currentText ().toInt(&OK); }

		if (!OK) { denominator = 4; }
	}

	m_pianoRoll->setTimeSignature (numerator, denominator);
	m_pianoRoll->populate         (m_parser.getNotes (), m_parser.getTempoMap(), track);

	// 5 should be the tempo
	QLayoutItem *tempoItem = m_controlLayout->itemAt(4);
	
	if (tempoItem) {
		QLabel *tempo = (QLabel *)tempoItem->widget();	
		tempo->setText(QString("Tempo: %1 BPM").arg(m_parser.getTempoMap().begin()->second));
	}
}

void MainWindow::openExportBrowser() {
	QString filePath = QFileDialog::getSaveFileName(
		this,
		"Export MIDI",
		"",
		"MIDI File (*.mid);;All Files (*.*)"
	);

	if (!filePath.isEmpty()) {
		exportCurrentToMIDI(filePath);
		m_fileInfo = QFileInfo(filePath);
	}
}

void MainWindow::exportCurrentToMIDI(const QString &filePath) {
	QFuture<bool> future = QtConcurrent::run([this, filePath] () {
		return m_exporter.exportToFile(filePath.toStdString(), m_parser);
	});

	m_exportWatcher.setFuture(future);

	return;
}

void MainWindow::onFileExported() {
	bool success = m_exportWatcher.result();

	if (success) { QMessageBox::information (this, "Success!", "Successfully exported MIDI file."); }
	else         { QMessageBox::critical    (this, "Error!",   "Failed to export MIDI file.");      }
}