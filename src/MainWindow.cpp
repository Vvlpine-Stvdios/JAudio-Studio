/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : MainWindow.cpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#include <MainWindow>
#include <TrackInfoDisplay>
#include <BulkConverterDialog>

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QScrollBar>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <QtConcurrent>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_parser(nullptr), m_exporter(nullptr) {
	setWindowTitle ("JAudio Studio");
	resize         (1000, 700);
	setupUI        ();
}

MainWindow::~MainWindow() {
	delete m_parser;
	delete m_exporter;
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
	QLabel         *dropLabel       = new QLabel         ("Drag and drop JAudio files", this);
	QPushButton    *browseButton    = new QPushButton    ("or click here to browse",    this);

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

	QSplitter *mainSplitter      = new QSplitter(Qt::Vertical,   this);
	QSplitter *secondarySplitter = new QSplitter(Qt::Horizontal, this);

	// Top half (piano roll)
	QWidget     *topHalf   = new QWidget     (this);
	QVBoxLayout *topLayout = new QVBoxLayout (topHalf);

	topLayout->setContentsMargins(0, 0, 0, 0);

	QWidget *controlPanel = new QWidget     (this);
	m_controlLayout       = new QHBoxLayout (controlPanel);
	
	m_controlLayout->setContentsMargins(5, 5, 5, 5);

	m_keySignatureBox = new QComboBox(this);

	m_keySignatureBox->addItems({
		"C Major / a minor",  "G Major / e minor",  "D Major / b minor",   "A Major / f♯ minor",
		"E Major / c♯ minor", "B Major / g♯ minor", "G♭ Major / e♭ minor", "D♭ Major / b♭ minor",
		"A♭ Major / f minor", "E♭ Major / c minor", "B♭ Major / g minor",  "F Major / d minor",
	});

	QHBoxLayout *timeSignature = new QHBoxLayout();

	timeSignature->setContentsMargins (0, 0, 0, 0);
	timeSignature->setSpacing         (0);

	m_timeSignatureNumerator   = new QSpinBox  (this);
	m_timeSignatureDenominator = new QComboBox (this);
	
	m_timeSignatureNumerator->setRange(1, 64);
	m_timeSignatureNumerator->setValue(4);

	m_timeSignatureDenominator->addItems({ "1", "2", "4", "8", "16" });
	m_timeSignatureDenominator->setCurrentText("4");

	timeSignature->addWidget  (m_timeSignatureNumerator  );
	timeSignature->addWidget  (new QLabel("/")           );
	timeSignature->addWidget  (m_timeSignatureDenominator);

	m_tempo = new QLabel("Tempo: -- BPM");

	m_controlLayout->addWidget  (new QLabel("Key: "  ));
	m_controlLayout->addWidget  (m_keySignatureBox    );
	m_controlLayout->addWidget  (new QLabel("Time: " ));
	m_controlLayout->addLayout  (timeSignature        );
	m_controlLayout->addWidget  (m_tempo              );
	m_controlLayout->addWidget  (new QLabel("Track: "));
	m_controlLayout->addStretch ();

	QWidget     *editorArea   = new QWidget     (this);
	QHBoxLayout *editorLayout = new QHBoxLayout (editorArea);

	editorLayout->setContentsMargins (0, 0, 0, 0);
	editorLayout->setSpacing         (0);

	m_pianoRoll = new PianoRoll(this);

	editorLayout->addWidget(m_pianoRoll);

	topLayout->addWidget(controlPanel);
	topLayout->addWidget( editorArea );

	secondarySplitter->addWidget(topHalf);

	// Left panel
	QWidget     *rightPanel  = new QWidget     (this);
	QVBoxLayout *rightLayout = new QVBoxLayout (rightPanel);

	QLabel *trackInspectorLabel = new QLabel("<b>Track Inspector</b>", this);
	rightLayout->addWidget(trackInspectorLabel);

	QHBoxLayout *navLayout = new QHBoxLayout();
	QPushButton *left      = new QPushButton("<", this);
	QPushButton *right     = new QPushButton(">", this);

	m_trackList        = new QComboBox      (this);
	m_trackDataDisplay = new QStackedWidget (this);
	m_trackList        -> addItems({ "All", "Global" });
	m_trackDataDisplay -> addWidget(new TrackInfoDisplay(this,  -1, "All"));
	m_trackDataDisplay -> addWidget(new TrackInfoDisplay(this, 255, "All"));

	
	navLayout  -> addWidget(left              );
	navLayout  -> addWidget(m_trackList       );
	navLayout  -> addWidget(right             );
	rightLayout -> addLayout(navLayout         );
	rightLayout -> addWidget(m_trackDataDisplay);

	secondarySplitter->addWidget (rightPanel);
	secondarySplitter->setSizes  ({ 800, 200 });

	// Bottom half (automation)
	QWidget       *bottomHalf    = new QWidget       (this);
	QVBoxLayout   *bottomLayout  = new QVBoxLayout   (bottomHalf);
	m_automationTab              = new AutomationTab (this);

	bottomLayout->addWidget(m_automationTab);

	mainSplitter->addWidget(secondarySplitter);
	mainSplitter->addWidget(bottomHalf);

	m_stackedWidget->addWidget(mainSplitter);

	// 
	// MENUS
	// 

	QMenu   *fileMenu       = menuBar()->addMenu ("&File");
	QAction *open           = new QAction        ("&Open BMS",       this);
	QAction *exportToMIDI   = new QAction        ("&Export to MIDI", this);
	QAction *quit           = new QAction        ("&Quit",           this);

	QMenu   *convertMenu    = menuBar()->addMenu ("&Convert");
	QAction *bulkConvertBMS = new QAction        ("&Bulk Convert Files", this);
	
	QMenu   *helpMenu       = menuBar()->addMenu ("&Help");
	QAction *about          = new QAction        ("&About",    this);
	QAction *aboutQt        = new QAction        ("&About Qt", this);


	open         -> setShortcut (QKeySequence::Open    );
	exportToMIDI -> setShortcut (QKeySequence("Ctrl+E"));
	quit         -> setShortcut (QKeySequence::Quit    );
	quit         -> setMenuRole (QAction     ::QuitRole);
	fileMenu     -> addAction   (    open    );
	fileMenu     -> addAction   (exportToMIDI);
	fileMenu     -> addAction   (    quit    );

	bulkConvertBMS -> setShortcut (QKeySequence("Ctrl+B"));
	convertMenu    -> addAction   (bulkConvertBMS);

	about    -> setMenuRole (QAction::AboutRole  );
	aboutQt  -> setMenuRole (QAction::AboutQtRole);
	helpMenu -> addAction   ( about );
	helpMenu -> addAction   (aboutQt);

	// 
	// EVENTS
	// 

	// File stuffs
	connect(open,             &QAction             ::triggered,   this, &MainWindow::openFileBrowser  );
	connect(dropZone,         &DropZoneWidget      ::fileDropped, this, &MainWindow::onFileSelected   );
	connect(browseButton,     &QPushButton         ::clicked,     this, &MainWindow::openFileBrowser  );
	connect(&m_openWatcher,   &QFutureWatcher<bool>::finished,    this, &MainWindow::onFileParsed     );
	connect(exportToMIDI,     &QAction             ::triggered,   this, &MainWindow::openExportBrowser);
	connect(&m_exportWatcher, &QFutureWatcher<bool>::finished,    this, &MainWindow::onFileExported   );
	connect(bulkConvertBMS,   &QAction             ::triggered,   this, &MainWindow::openBulkConverter);

	connect(quit, &QAction::triggered, qApp, &QCoreApplication::quit);

	connect(about,   &QAction::triggered, this, [this]() { QMessageBox::about   (this, "About JAudio Studio & libJAudio",
		QString("by Vvlpine Stvdios\nVersion %1 - %2\nJAudio %3 - %4\nQt %5 - %6")
			.arg(JAUDIO_STUDIO_VERSION_STR)
			.arg(JAUDIO_STUDIO_COPYRIGHT)
			.arg(LIBJAUDIO_VERSION_STR)
			.arg(LIBJAUDIO_COPYRIGHT)
			.arg(QT_VERSION_STR)
			.arg(QT_COPYRIGHT)
		);
	});
	connect(aboutQt, &QAction::triggered, this, [this]() { QMessageBox::aboutQt(this, "About Qt"); });

	// UI updates
	connect(m_keySignatureBox,          &QComboBox::currentIndexChanged, this, &MainWindow::onUIUpdated);
	connect(m_timeSignatureNumerator,   &QSpinBox ::valueChanged,        this, &MainWindow::onUIUpdated);
	connect(m_timeSignatureDenominator, &QComboBox::currentIndexChanged, this, &MainWindow::onUIUpdated);
	connect(m_trackList,                &QComboBox::currentIndexChanged, this, &MainWindow::onUIUpdated);

	connect(left,  &QPushButton::clicked, [this]() { m_trackList->setCurrentIndex((m_trackList->currentIndex() - 1) % m_trackList->count()); m_trackDataDisplay->setCurrentIndex(m_trackList->currentIndex()); });
	connect(right, &QPushButton::clicked, [this]() { m_trackList->setCurrentIndex((m_trackList->currentIndex() + 1) % m_trackList->count()); m_trackDataDisplay->setCurrentIndex(m_trackList->currentIndex()); });

	connect(m_pianoRoll     -> getHorizontalScrollBar(), &QScrollBar::valueChanged, m_automationTab -> getHorizontalScrollBar(), &QScrollBar::setValue);
	connect(m_automationTab -> getHorizontalScrollBar(), &QScrollBar::valueChanged, m_pianoRoll     -> getHorizontalScrollBar(), &QScrollBar::setValue);

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

void MainWindow::open          (const QString &filePath) { onFileSelected(filePath); }
void MainWindow::onFileSelected(const QString &filePath) {
	m_stackedWidget->setCurrentIndex(1);

	// Push the parsing onto a background thread
	QFuture<bool> future = QtConcurrent::run([this, filePath] () {
		m_parser = new JAudio::BMS::Parser();
		return m_parser->loadFromFile(filePath.toStdString(), m_data);
	});

	m_openWatcher.setFuture(future);
}

void MainWindow::onFileParsed() {
	bool success = m_openWatcher.result();

	if (success) {
		if (std::holds_alternative<JAudio::BMS::Sequence>(m_data)) {
			const JAudio::BMS::Sequence &sequenceData = std::get<JAudio::BMS::Sequence>(m_data);

			setWindowTitle    ("JAudio Studio — " + m_fileInfo.fileName());
			setWindowFilePath (m_fileInfo.filePath());

			m_stackedWidget -> setCurrentIndex (2);
			m_pianoRoll     -> setPPQN         (sequenceData.PPQN);
			m_automationTab -> setPPQN         (sequenceData.PPQN);

			for (const int &track : sequenceData.tracks) {
				QString name = QString("%1").arg(track);
				m_trackList        -> addItem   (                                  name               );
				m_trackDataDisplay -> addWidget (new TrackInfoDisplay(this, track, name.toStdString()));
			}

			updateUI();
		}// else if (std::holds_alternative<JAudio::AFC::StreamData>(data)) {
		// 	const JAudio::AFC::StreamData &streamData = std::get<JAudio::AFC::StreamData>(data);
		// }
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
	if (std::holds_alternative<JAudio::BMS::Sequence>(m_data)) {
		JAudio::BMS::Sequence sequence = std::get<JAudio::BMS::Sequence>(m_data);

		bool OK;
		int  track = m_trackList->currentText().toInt(&OK);

		if (!OK) {
			if (m_trackList->currentText() == QString("Global")) { track =  255; }
			else                                                 { track = -1;   }
		}

		m_trackDataDisplay->setCurrentIndex(m_trackList->currentIndex());

		// 3 should be the track list
		int numerator   = 4;
		int denominator = 4;
		
		numerator   = m_timeSignatureNumerator   -> value       ();
		denominator = m_timeSignatureDenominator -> currentText ().toInt(&OK);

		if (!OK) { denominator = 4; }

		m_pianoRoll     -> setTimeSignature (numerator, denominator);
		m_automationTab -> setTimeSignature (numerator, denominator);
		m_pianoRoll     -> populate         (sequence.notes,      track);
		m_automationTab -> populate         (sequence.automation, track, m_pianoRoll->getSceneWidth());

		// 0xFF is the global track
		m_tempo->setText(QString("Tempo: %1 BPM").arg(sequence.automation.at(0xFF).at(JAudio::BMS::CMD_TEMPO_SET).begin()->args[1]));
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
	std::vector<MIDI::TrackInfo> trackInfos = {};

	for (int i = 0; i < m_trackDataDisplay->count(); i++) {
		MIDI::TrackInfo info = ((TrackInfoDisplay *)m_trackDataDisplay->widget(i))->getInfo();

		if (info.name != "All" && info.name != "Global") { trackInfos.push_back(info); }
	}

	QFuture<bool> future = QtConcurrent::run([this, filePath, trackInfos] () {
		m_exporter = new MIDI::Exporter();
		return m_exporter->exportToFile(filePath.toStdString(), m_data, trackInfos);
	});

	m_exportWatcher.setFuture(future);

	return;
}

void MainWindow::onFileExported() {
	bool success = m_exportWatcher.result();

	if (success) { QMessageBox::information (this, "Success!", "Successfully exported MIDI file."); }
	else         { QMessageBox::critical    (this,  "Error!" ,   "Failed to export MIDI file."   ); }
}