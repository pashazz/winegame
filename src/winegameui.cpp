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

WinegameUi::~WinegameUi()
{
	if (progress != 0)
	delete progress;
}

void WinegameUi::error(QString title, QString text)
{
	QMessageBox::critical(qApp->desktop(), title, text);
}

void WinegameUi::showNotify(QString title, QString body)
{
	/// знаю что тупизм,но никто не хочет помогать
	if (QProcessEnvironment::systemEnvironment().contains("KDE_FULL_SESSION")) //пока кеды юзают KDialog
	{
		QStringList arguments;
		arguments << "--passivepopup" <<body;
		arguments << "--title"<<title;
		QProcess::startDetached("/usr/bin/kdialog",arguments);
	}
	else
	{
		QStringList arguments;
		arguments << title << body;
		QProcess::startDetached("/usr/bin/notify-send",arguments);
	}
}

void WinegameUi::showProgressBar(QString title)
{
 progress = new QProgressDialog(qApp->desktop());
 progress->setWindowTitle(title);
 progress->setModal (true);
 progress->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
 progress->show();
}

void WinegameUi::showProgressBar(QString title, const char *cancelSlot, QObject *pointer)
{
	progress = new QProgressDialog(qApp->desktop());
	connect (progress, SIGNAL(canceled()),  pointer, cancelSlot);
	progress->setWindowTitle(title);
	progress->setModal (true);
	progress->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
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
	 mem= QInputDialog::getInt(qApp->desktop(),tr("WineGame"), tr("Enter memory size of your video card (in megabytes). If you click Cancel, then default will be used"), 128, 1, 4096);
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
	return QMessageBox::question(qApp->desktop(), title, text) == QMessageBox::Yes;
}

void WinegameUi::infoDialog(const QString &title, const QString &text)
{
	QMessageBox::information(qApp->desktop(), title, text);
	return;
}

void WinegameUi::selectExe(const QString &title, QString &file, QString home)
{
	if (home.isEmpty())
		home = QDir::currentPath();
	QString myFile = QFileDialog::getOpenFileName(qApp->desktop(), title, home, tr("Windows executables (*.exe)"));
	file = myFile;
}

void WinegameUi::insertNextCd(bool &result, QString count)
{
	int res = QMessageBox::question(qApp->desktop(), tr("Insert next CD"), tr("Insert program CD %1").arg(QString(count)), QMessageBox::Ok, QMessageBox::Ignore);
	if (res == QMessageBox::Ok)
		result = true;
	else
		result = false;
}

void WinegameUi::showUserWaitMessage(const QString &message)
{
	waitDialog = new QDialog (0);
	waitDialog->setModal(true);
	waitDialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
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
QString WinegameUi::directoryDialog(const QString &description, const QString &startDirectory)
{
	return QFileDialog::getExistingDirectory(0, description, startDirectory);
}

void WinegameUi::getText(const QString &title, const QString &message, QString &result)
{
	result = QInputDialog::getText(qApp->desktop(), title, message);
}
