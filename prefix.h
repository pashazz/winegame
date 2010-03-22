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

#ifndef PREFIX_H
#define PREFIX_H

#include <QObject>
#include "linux.h"
#include "engine.h"
class Prefix : public QObject
{
Q_OBJECT
public:
    explicit Prefix(QObject *parent, QString path); //path-путь к директории с описанием
    QString getPrefix ()  {return wineprefix;}
    QProcessEnvironment getEnv () {return env;}
    QString getWine () {return wine;}
    QString getName () {return  _name;}
    QString getNote () {return _note;}
    void runProgram  (QString path);
    void lauchWinetricks (QStringList args);
    void removePrefix();
signals:

public slots:
    void lauch_c();


private:
       QString _name;
    QString _note;
    QString wine; //wine binary
    QString wineprefix; //wineprefix
    QProcessEnvironment env; //process environment with $WINEPREFIX and $WINE
};

#endif // PREFIX_H
