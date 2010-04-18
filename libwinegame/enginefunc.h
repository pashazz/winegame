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

#ifndef ENGINEFUNC_H
#define ENGINEFUNC_H
#include <QtCore>
#include <QDebug>
#include <QMessageBox>
#include "libwinegame_global.h"

void unpackWine (QString distr, QString destination)
{
     QDir dir (destination);
     if (!dir.exists())
         dir.mkdir(dir.path());
     else
     {
         int result = QMessageBox::question(0, QObject::tr("Question"), QObject::tr("I see, that Wine did installed previously for this application (Directory %1 exists).<br>Would you like to install wine now?").arg(destination), QMessageBox::Yes, QMessageBox::No);
         if (result == QMessageBox::No)
             return;

     }
 QProcess *proc = new QProcess (0); //не забываем удалять
 proc->setWorkingDirectory(destination);
 QString unpackLine =  QObject::tr ("tar xvpf %1 -C %2").arg(distr).arg(destination);
 proc->start(unpackLine);
  proc->waitForFinished(-1);
  qDebug() << "DEBUG: Unpack commandline is " << unpackLine;
 qDebug() << QObject::tr("engine: Wine distribution %1 unpacked to %2").arg(distr).arg(destination);
}


#endif // ENGINEFUNC_H
