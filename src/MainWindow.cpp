/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : main_window.hpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#include <MainWindow>

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

	QWidget     *timeSignature       = new QWidget     (this);
	QHBoxLayout *timeSignatureLayout = new QHBoxLayout (timeSignature);

	timeSignatureLayout->setContentsMargins (0, 0, 0, 0);
	timeSignatureLayout->setSpacing         (0);

	QSpinBox  *timeSignatureNumerator   = new QSpinBox  (this);
	QComboBox *timeSignatureDenominator = new QComboBox (this);
	
	timeSignatureNumerator->setRange(1, 64);
	timeSignatureNumerator->setValue(4);

	timeSignatureDenominator->addItems({ "1", "2", "4", "8", "16" });
	timeSignatureDenominator->setCurrentText("4");

	timeSignatureLayout->addWidget(timeSignatureNumerator  );
	timeSignatureLayout->addWidget(new QLabel("/")         );
	timeSignatureLayout->addWidget(timeSignatureDenominator);

	QSpinBox *tempoBox = new QSpinBox(this);

	tempoBox->setRange  (20, 300);
	tempoBox->setValue  (120);
	tempoBox->setSuffix (" BPM");

	m_controlLayout->addWidget  (new QLabel("Key: "  ));
	m_controlLayout->addWidget  (keySignatureBox      );
	m_controlLayout->addWidget  (new QLabel("Time: " ));
	m_controlLayout->addWidget  (timeSignature        );
	m_controlLayout->addWidget  (new QLabel("Tempo: "));
	m_controlLayout->addWidget  (tempoBox             );
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

	QMenu   *fileMenu   = menuBar()->addMenu ("&File");
	QAction *openAction = new QAction        ("&Open BMS...", this);

	openAction->setShortcut (QKeySequence::Open);
	fileMenu  ->addAction   (openAction);

	// 
	// EVENTS
	// 

	connect(openAction,   &QAction             ::triggered,   this, &MainWindow::openFileBrowser);
	connect(dropZone,     &DropZoneWidget      ::fileDropped, this, &MainWindow::onFileSelected );
	connect(browseButton, &QPushButton         ::clicked,     this, &MainWindow::openFileBrowser);
	connect(&m_watcher,   &QFutureWatcher<bool>::finished,    this, &MainWindow::onFileParsed   );

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

	m_watcher.setFuture(future);
}

void MainWindow::onFileParsed() {
	bool success = m_watcher.result();

	if (success) {
		setWindowTitle    ("JAudio Studio — " + m_fileInfo.fileName());
		setWindowFilePath (m_fileInfo.filePath());
		
		m_stackedWidget -> setCurrentIndex (2);
		m_pianoRoll     -> setPPQN         (m_parser.getPPQN  ());
		m_pianoRoll     -> populate        (m_parser.getNotes (), m_parser.getTempoMap());

		// 5 should be the time signature
		QLayoutItem *item = m_controlLayout->itemAt(5);
		
		if (item) {
			QSpinBox *tempo = (QSpinBox *)item->widget();
			
			tempo->setValue(m_parser.getTempoMap().begin()->second);
		}

	} else {
		m_stackedWidget->setCurrentIndex(1);
		QMessageBox::critical(this, "Error", "Failed to parse file!");
	}
}