/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/07
 * 	File    : dropzone.hpp
 * 	Project : JAudio Studio
 * 
 ********************************************************************************************************************/

#pragma once

#include <QFrame>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

class DropZoneWidget : public QFrame {

	Q_OBJECT

	public:
		explicit DropZoneWidget(QWidget *parent = nullptr);
	
	signals:
		void fileDropped(const QString &filePath);
	
	protected:
		void dragEnterEvent (QDragEnterEvent *event) override;
		void dropEvent      (QDropEvent      *event) override;

};