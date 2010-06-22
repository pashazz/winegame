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


#include "messagehandler.h"

void MessageHandler::prefixID(QString &id)
{
	dialog:
	QString myPrefix =QInputDialog::getText(0, tr("Give a name for your application"), tr ("Give a short latin name for your application. <br> It will be installed in %1/Windows/APPLICATION NAME/drive_c").arg(QDir::homePath()));
	if (myPrefix.isEmpty())
	{
		QMessageBox::critical(0, tr("No name specified"), tr ("Abort. Reason: No application name specified"));
		id = "";
		return;
	}
	//проверим, есть ли такой префикс уже.
	QDir dir;
	dir.setPath(core->wineDir() + QDir::separator() + myPrefix);
	if (dir.exists())
	{
		QMessageBox::warning(0, tr("Application with this name is already installed."), tr("To force installation process, remove directory %1.").arg(core->wineDir() +QDir::separator() + myPrefix));
		//я знаю, это плохо
		goto dialog;
	}
	if (!corelib::checkPrefixName(myPrefix))
		goto dialog;
	id = myPrefix;
	return;
}

void MessageHandler::prefixName(QString &name)
{
	QString myPrefix =QInputDialog::getText(0, tr("Give a readable for your application"), tr ("Give a short readable name for your application, for example 'CoolGame v3'"));
	if (myPrefix.isEmpty())
	{
		QMessageBox::critical(0, tr("No name specified"), tr ("Abort. Reason: No application name specified"));
		name = "";
		return;
	}
	else
		name = myPrefix;}

void MessageHandler::prefixNote(QString &note)
{
	note =QInputDialog::getText(0, tr("Give a readable note about your application"), tr ("Give a short note about your application, for example 'CoolGame v3'"));
}
