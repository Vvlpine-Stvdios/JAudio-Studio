/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/07
 * 	File    : DropZone.cpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/

#include <DropZone>

DropZoneWidget::DropZoneWidget(QWidget *parent) : QFrame(parent) {
	setAcceptDrops(true);

	setStyleSheet("DropZoneWidget { border: 2px dashed #AAA; border-radius: 10px }");
}

void DropZoneWidget::dragEnterEvent(QDragEnterEvent *event) {
	// We want the file path; reject if it contains something else
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void DropZoneWidget::dropEvent(QDropEvent *event) {
	const QList<QUrl> urls = event->mimeData()->urls();

	if (!urls.isEmpty()) {
		emit fileDropped(urls.first().toLocalFile());
	}
}