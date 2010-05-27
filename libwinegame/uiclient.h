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


#ifndef UICLIENT_H
#define UICLIENT_H

#include <QtCore>

/*
  Этот класс необходим, чтобы предоставить пользователю разные варианты базового UI (напр. альтернативного UI на QtDeclarative)
  */

class UiClient : public QObject
{
Q_OBJECT
public:
   virtual void showNotify (QString title, QString body) = 0; //показывает нотификацию
   /*! ProgressBar func. */
   virtual void showProgressBar (QString title) = 0; //показываем прогрессбар с заголовком title
   virtual void progressText (QString text) = 0 ; //показать текст text на прогрессбаре
   virtual void progressRange (int, int) = 0; //прогресс операции
   virtual void endProgress () = 0; //закрытие бара.
   virtual void error (QString title, QString text) =0; //показ ошибки.
   virtual int getVideoMemory() = 0; //запрос видеопамяти при первом запуске.
   virtual QString desktopLocation () = 0; //Папка рабочего стола (может определяться по-разному в зависимости от типа UI). Используется для создания ярлыка Freedesktop
   virtual bool questionDialog (const QString &title, const QString &text) = 0; //Запрос ДА/НЕТ (true/false)
   virtual void infoDialog (const QString &title, const QString &text) = 0; //Показ информации _И ОЖИДАНИЕ РЕАКЦИИ ПОЛЬЗОВАТЕЛЯ_. Проще говоря, мессейджбокс OK
};

#endif // UICLIENT_H
