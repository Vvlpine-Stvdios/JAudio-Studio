/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : main_window.hpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#include <MainWindow>

#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle ("JAudio Studio");
	resize         (800, 600);

	QMenu   *fileMenu   = menuBar()->addMenu("&File");
	QAction *openAction = new QAction("&Open BMS...", this);

	openAction->setShortcut (QKeySequence::Open);
	fileMenu  ->addAction   (openAction);

	connect(openAction, &QAction::triggered, this, &MainWindow::openBMSFile);
}

void MainWindow::openBMSFile() {
	QString filePath = QFileDialog::getOpenFileName(
		this,
		"Open BMS File",
		"",
		"BMS Files (*.bms);;All Files (*.*)"
	);

	// Occurs on cancel
	if (filePath.isEmpty()) { return; }

	std::string stdFilePath = filePath.toStdString();

	std::cout << "Loading file '" << stdFilePath << "'..." << std::endl;

	bool result = m_parser.loadFromFile(stdFilePath);

	std::cout << ((result) ? "Success!" : ":(") << std::endl;

	if (result) {
		QMessageBox::information(this, "Success", "BMS File loaded successfully.");
	} else {
		QMessageBox::critical(this, "Error", "Failed to parse the BMS file.");
	}
}