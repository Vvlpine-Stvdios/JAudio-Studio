/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/15
 * 	File    : TrackInfoDisplay.cpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#include <TrackInfoDisplay>

#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>

#include <iostream>

TrackInfoDisplay::TrackInfoDisplay(QWidget *parent, int index, const std::string &name)
	: QWidget (parent),
	  m_info  ((MIDI::TrackInfo) {
		name,
		false
	})
{
	QVBoxLayout *rowLayout = new QVBoxLayout (this);
	this->setContentsMargins(5, 2, 5, 2);

	if (m_info.name == "") {
		m_info.name = QString("Track %1").arg(index, 2, '0').toStdString();
	}

	QLineEdit *nameEdit = new QLineEdit(QString(m_info.name.c_str()), this);

	QCheckBox *percCheck = new QCheckBox("Percussion", this);
	percCheck->setToolTip("Export on MIDI Channel 10");

	rowLayout->addWidget  (nameEdit );
	rowLayout->addWidget  (percCheck);
	rowLayout->addStretch ();

	if (m_info.name == "All" || m_info.name == "Global") {
		nameEdit  -> setDisabled (true );
		percCheck -> setChecked  (false);
		percCheck -> setDisabled (true );

		nameEdit  -> setToolTip(QString("Cannot edit name of track \"%1\""       ).arg(m_info.name));
		percCheck -> setToolTip(QString("Cannot mark track \"%1\" as percussion.").arg(m_info.name));
	} else {
		connect(nameEdit,  &QLineEdit::textChanged,       this, [this, nameEdit ]() { m_info.name   = nameEdit  -> text      ().toStdString(); });
		connect(percCheck, &QCheckBox::checkStateChanged, this, [this, percCheck]() { m_info.isPerc = percCheck -> isChecked ();               });
	}
}