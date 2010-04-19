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

#ifndef LINUX_H
#define LINUX_H
#include <QString>

//важные константы
const QString gamepath = "/usr/share/winegame"; // Директория, где искать пакеты со скриптами
const QString winepath = "/Windows"; // тут располагаются префиксы ($HOME/%winepath%)
const QString CTRL="/control"; //control file, DO NOT EDIT
const QString TMP = "/tmp"; //временная директория
const QString config = "/.config/winegame.conf"; //Конфиг программы  ($HOME/%config%)
const QString MOUNT_DIR = "/game"; // Куда монтировать ISO ($HOME/%MOUNT_DIR%)
const QString reg = "/tmp/win.reg"; //имя временного файла реестра, куда мы запишиваем все нужное
#endif
