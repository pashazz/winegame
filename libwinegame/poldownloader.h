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
/*!
  PolDownloader is a http://wine.playonlinux.com/linux-i386/ download & integration client
  */

#ifndef POLDOWNLOADER_H
#define POLDOWNLOADER_H

#include <QtCore>
#include <QtNetwork>
#include "prefix.h"

class PolDownloader : public QObject
{
	Q_OBJECT
public:
	PolDownloader(Prefix *prefixObj);
	QStringList versionList() {return versions;}
	bool setWineVersion (QString version); //return false if SQL error or no such version
	void fallback();


private:
	Prefix *prefix;
	corelib *core;
	bool goodGet;
	bool downloadWine(QString version);
	bool checkSHA1(QString file);

	QStringList sha1sums;
	QStringList versions;
	QStringList files;

protected:
	 QString URL;

private slots:
	 void	setProgressRange (qint64, qint64);
	 void	error (QNetworkReply::NetworkError);
};

#endif // POLDOWNLOADER_H
