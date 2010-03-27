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
#include <QtCore>
#include <QUrl>
#include <QProgressDialog>
#include <QIcon>
#include <QtNetwork>
#include "linux.h"
class engine : public QObject
{
Q_OBJECT
public:
    explicit engine(QObject *parent = 0);
 void lauch (QString workdir, bool msg = true);
 void setCdMode (bool mode){cdMode=mode;}
 void setDiskpath (QString path){diskpath = path;}
 static QString getName (QString path);
 static QString getNote(QString path);
 static QIcon getIcon (QString path);
 static QString getPrefixName (QString path);
 static QString getWine (QString path);
 static void showNotify (QString, QString);
 signals:

private slots:
 void error (QNetworkReply::NetworkError);
 void setRange (qint64, qint64); //заглушка для QProgressDialog
 void exitApp ();
private:
static  QString getVariableValue (QString value, const QStringList &vars);
bool cdMode;
QString diskpath;
QProgressDialog *progress;
void doPkgs (QString pkgs, const QProcessEnvironment &env);
//нужные переменные
QString prefix;
QString wineBinary;
QString name;
QString note;
QString iconPath;
QString program;
//Функции собственно движка (исп. с QtConcurrent) объявлены в enginefunc.h
 QString downloadWine(QString url);
 void doDesktop(QString);
};

#endif // ENGINE_H
