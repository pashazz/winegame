/*
    Winegame - small utility to prepare Wine and install win32 apps in it.
    Copyright (C) 2010 Pavel Zinin <pzinin@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/


#include "winegameui.h"

WinegameUi::WinegameUi()
{

}

WinegameUi::~WinegameUi()
{
	if (progress != 0)
	delete progress;
}

void WinegameUi::error(QString title, QString text)
{
	QMessageBox::critical(0, title, text);
}

void WinegameUi::showNotify(QString title, QString body)
{
	/// знаю что тупизм,но никто не хочет помогать
		if (QProcessEnvironment::systemEnvironment().contains("KDE_FULL_SESSION")) //пока кеды юзают KDialog
			//вся земля юзает notify-send
			//чезез kdialog:
		{
								 QStringList arguments;
								arguments << "--passivepopup" <<body;
								arguments << "--title"<<title;
								QProcess::startDetached("/usr/bin/kdialog",arguments);
										  }


			//Через notify-send:
		else
		{
								 QStringList arguments;
								arguments << title << body;
								QProcess::startDetached("/usr/bin/notify-send",arguments);
							}

}

void WinegameUi::showProgressBar(QString title)
{
 progress = new QProgressDialog(0);
 connect (progress, SIGNAL(canceled()), this, SIGNAL(cancelOperation()));
 progress->setWindowTitle(title);
 progress->setModal (true);

 progress->show();
}

void WinegameUi::progressRange(int aval, int total)
{
	progress->setMaximum(total);
	progress->setValue(aval);
}

int WinegameUi::getVideoMemory()
{
	int mem = 0;
	 mem= QInputDialog::getInt(0, QObject::tr("WineGame"), QObject::tr("Enter memory size of your video card (in megabytes). If you click Cancel, then default will be used"), 128, 1, 4096);
	if (mem == 0)
			mem = 128;
	return mem;
	}

void WinegameUi::endProgress()
{
	progress->close();
}

void WinegameUi::progressText(QString text)
{
	progress->setLabelText(text);
}

bool WinegameUi::questionDialog(const QString &title, const QString &text)
{
	return QMessageBox::question(0, title, text) == QMessageBox::Yes;
}

void WinegameUi::infoDialog(const QString &title, const QString &text)
{
	QMessageBox::information(0, title, text);
	return;
}

void WinegameUi::selectExe(const QString &title, QString &file, QString home)
{
	if (home.isEmpty())
		home = QDir::homePath();
	QString myFile = QFileDialog::getOpenFileName(0, title, home);
	file = myFile;
}

void WinegameUi::insertNextCd(bool &result, QString count)
{
	int res = QMessageBox::question(0, tr("Insert next CD"), tr("Insert program CD %1").arg(QString(count)), QMessageBox::Ok, QMessageBox::Ignore);
	if (res == QMessageBox::Ok)
		result = true;
	else
		result = false;
}

void WinegameUi::showUserWaitMessage(const QString &message)
{
	waitDialog = new QDialog (0);
	waitDialog->setModal(true);
	QHBoxLayout *layout = new QHBoxLayout(waitDialog);
	QLabel *label = new QLabel (waitDialog);
	label->setText(message);
	layout->addWidget(label);
	waitDialog->setLayout(layout);
	waitDialog->show();

}
void WinegameUi::closeWaitMessage()
{
	waitDialog->close();
	delete waitDialog;
}
