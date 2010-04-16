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


#ifndef AUTOPACKAGE_H
#define AUTOPACKAGE_H

#include <QtCore>
#include <QSettings>
#include <QMessageBox>
#include <QInputDialog>
#include "engine.h" //for Engine::showNotify
/*!
 WISOtool class.
 BETA version now
  */

class AutoPackage : public QObject
{
Q_OBJECT
    public:

    AutoPackage();
    void load();
    void setWorkingDirectory (QString work) {_dir=work;}
    static bool isAutoPackage(QString dir);
private:
    void startInstall(QProcess *p);
    QString _prefix;
    QString _dir;
    QString _game;
  bool dvd;
};

#endif // AUTOPACKAGE_H
