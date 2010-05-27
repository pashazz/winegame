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


#ifndef DVDRUNNER_H
#define DVDRUNNER_H

#include <QtCore>
#include "corelib.h"
#include "prefix.h"
namespace Pashazz
{
enum DriveType
{
	Real = 1, Image = 2, Unknown = 0
};
struct DiscInfo
{
	QString name;
	QString desc;
	QString icon;
};
};
class DVDRunner : public QObject
{
	Q_OBJECT
public:
	DVDRunner(corelib *lib, QString path);
	~DVDRunner();
	QString diskDirectory() {return diskPath;} //возвращает пустую строку, если монтирование завершилось неудачно
	QString exe();
	QString imageFile() {return realDrive;}
	Pashazz::DriveType objectType () {return type;}
	static Pashazz::DiscInfo * info (QString diskPath, corelib *lib);
	Prefix *prefix() {return this->Wprefix;} // Возвращает объект Prefix для работы
	bool success() {return result;} //Закончилось ли распознавание успешно
	void setPrefix (Prefix * prefix);
signals:
	void insertNextCd (bool &result, int cd); //Пользователь должен вставить CD.
	void fileNeed (QString &exe, QString disc);
private:
	corelib *core;
	bool result;
	bool multidisc;
	bool detect();
	static QString wrkdir (QString diskPath, QDir packageDir); //главная функция
	bool prepare (bool nodetect = false); //метод для выполнения различных подготовок (монтирования и т.д.). Если WineGame распознал диск сам, то этот метод вызывается из конструктора.
	Prefix *Wprefix;
	QString diskPath, realDrive;
	QString mount, umount;
	Pashazz::DriveType type;
	int _max; //copyfile size
private slots:
	void setProgress (qint64 bytes);
	void closeBar ();
};

#endif // DVDRUNNER_H
