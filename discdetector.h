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

#ifndef DISCDETECTOR_H
#define DISCDETECTOR_H

#include <QObject>
#include "engine.h"
#include "gamedialog.h"
class DiscDetector : public QObject
{
Q_OBJECT
public:
	explicit DiscDetector(corelib *lib);
 bool  tryDetect (QString path);
 QString getGame () {return gamefolder;}
 void lauchApp (); //call only even tryDetect is TRUE!!!!!!!!
signals:

private slots:
void selectFile (QString &fileName);
private:
 QString gamefolder;
 QString cdroot;
 corelib *core;
};

#endif // DISCDETECTOR_H
