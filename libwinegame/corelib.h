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


#ifndef CORELIB_H
#define CORELIB_H

#include <QtCore>
#include <QInputDialog>
#include <QMessageBox>
#include "linux.h"
#include "libwinegame_global.h"
class  WINESTUFFSHARED_EXPORT corelib : public QObject
{
public:
    corelib();
     static QString whichBin (QString bin);
     static void init (); /// этот метод на данный момент только прописывает видеопамять. В конфиг.
     static void unpackWine(QString distr, QString destination);
};

#endif // CORELIB_H
