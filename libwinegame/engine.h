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

#ifndef ENGINE_H
#define ENGINE_H

#include <QMessageBox> //для показа сообщений об ошибках
#include <QString>
#include <QLabel>
#include <QTextStream>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QIcon>
#include <QSettings>
#include <QDesktopServices>

#include "corelib.h"
#include "libwinegame_global.h"
class WINESTUFFSHARED_EXPORT  engine : public QObject
{
Q_OBJECT
public:
    explicit engine(QObject *parent = 0);
 void lauch (QString workdir, bool msg = true);
 void setCdMode (bool mode){cdMode=mode;}
 void setDiskpath (QString path){diskpath = path;}
 static void makefix (QString prefix); //исправляет запуск WineBrowser
 static QString getExeWorkingDirectory(QString exe);
private:
QString initMyWindows(); //возвращаем имя префикса, с которым создается объект Prefix
bool cdMode;
corelib *core;
QString diskpath;
void doPkgs (QString pkgs, const QProcessEnvironment &env);
//нужные переменные
QString prefix;
QString wineBinary;
QString name;
QString note;
QString iconPath;
QString program;
 void setMemory (QString);
 void makecdlink();
 QString controlFile;
 //И еще функции
 QString getRunnableExe();
QString myPrefixName(); /// имя префикса, используется в lauch
};

#endif // ENGINE_H
