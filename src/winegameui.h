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


#ifndef WINEGAMEUI_H
#define WINEGAMEUI_H
#include "uiclient.h"
#include <QtGui>
class WinegameUi : public UiClient
{
	Q_OBJECT
public:
	WinegameUi();
	~WinegameUi();
	void showNotify (QString title, QString body);
	void error (QString title, QString text);
	void showProgressBar (QString title); //показываем прогрессбар с заголовком title
	void showProgressBar (QString title,  const char * cancelSlot, QObject *pointer);
	void progressText (QString text) ; //показать текст text на прогрессбаре
	void progressRange (int, int); //прогресс операции
	void endProgress (); //закрытие бара.
	int getVideoMemory();
	QString desktopLocation() { return QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);} //GUI интерфейс предоставляет десктоп по-умолчанию.
	bool questionDialog(const QString &title, const QString &text);
	void infoDialog (const QString &title, const QString &text);
	void selectExe(const QString &title, QString &file, QString home= "");
	void insertNextCd(bool &result, QString count);
	virtual void showUserWaitMessage (const QString &message); //показать модальный диалог (или что-то типа того, чтобы пользователь понял, что идет операция).
	virtual void closeWaitMessage(); //закрытие диалога
private:
	   QProgressDialog *progress;
	   QDialog *waitDialog;
};

#endif // WINEGAMEUI_H
