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
/// класс для работы с префиксами (не preset)
#include <QtCore>
#include <QDesktopServices>
#include <QIcon>
#include <QFileDialog>
#include "libwinegame_global.h"
#include "corelib.h"

 class WINESTUFFSHARED_EXPORT  Prefix : public QObject
{
Q_OBJECT
public:
    explicit Prefix(QObject *parent, QString workdir);
	QString prefixPath () {return _path;}
    QString prefixName () {return _prefix;}
    QString wine();
    QString standardExe();
   QString name ();
   QString note ();
   QIcon icon ();
   QString getRunnableExe();
   bool runApplication (QString exe); //well, it`s helper.
  void makeDesktopIcon (const QString &path, const  QString &name);
  bool isPreset();
  QString projectWorkingDir () {return _workdir;}
  QProcessEnvironment envs () {return env;}
  //Запуск программы в данном Prefix
  void runProgram (QString exe);
  void lauchWinetricks(QStringList args);
  void setMemory();
  void removePrefix ();
  bool installFirstApplication ();
  bool checkWineDistr();
  bool hasDBEntry ();
public slots:
  void lauch_c();


signals:

private:
QString _prefix;
QString _path;
QString _workdir;
  QSettings *s;
QProcessEnvironment env;
QString downloadWine ();
corelib *core;
void getPrefixPath();
QSqlDatabase db;

protected:
QString selectExe ();
};
#endif // PREFIX_H
