/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : main.cpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#include <QApplication>
#include <MainWindow>

int main(int argc, char **argv) {
	QApplication app(argc, argv);

	MainWindow window;
	window.show();

	return app.exec();
}