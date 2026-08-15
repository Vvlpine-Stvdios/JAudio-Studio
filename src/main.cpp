/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : main.cpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/

#include <MainWindow>

#include <QApplication>

#include <iostream>

int main(int argc, char **argv) {
	QApplication app(argc, argv);

	app.setApplicationDisplayName ("JAudio Studio");
	app.setApplicationName        ("JAudio Studio");

	MainWindow window;
	window.show();

	if (argc >= 2) {
		std::cout << "Opening \"" << argv[1] << "\"" << std::endl;
		window.open(QString(argv[1]));
	}

	return app.exec();
}