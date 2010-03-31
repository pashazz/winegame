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


#include "mainwindow.h"
#include "linux.h"
#include "discdetector.h"
#include "isomaster.h"
#include "QDir"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//загружаем локализацию
    QTranslator qt;
    qt.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qt);
    QTranslator app;
    app.load(":/ts/l10n/wg_" + QLocale::system().name());
    a.installTranslator(&app);
#ifndef Q_WS_X11
    QMessageBox::critical(0, QObject::tr("This is not for Windows or Mac OSX"), QObject::tr("This program is not suitable to run on Windows or Mac OSX. <br> If you want to run WineGame on Win or Mac. pls remove this warning in main.cpp");
    return -256;
#endif
    //set Linux console encoding to UTF-8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
     QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
      //проверяем Winetricks
     if (!QFile::exists("/usr/bin/winetricks"))
         QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Winetricks not found. Put it in /usr/bin/winetricks"));
      QDir dir (QDir::homePath() + QDir::separator() + winepath);
if (!dir.exists())
    dir.mkdir(dir.path()); //проверяем главную папочку  WineGame
// детектинг диска

if (a.arguments().length() > 1) {
    QFileInfo info (a.arguments().at(1));
    if (!info.exists())
    {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Incorrect commandline arguments"));
        return -3;
}
    if (info.isDir()) //запускаем детектор диска
    {
        DiscDetector det;
        if (det.tryDetect(info.absoluteFilePath()))
        {
            det.lauchApp();
            return 0;
        }
        else
        {
            QMessageBox::warning(0, QObject::tr("Error"), QObject::tr("Failed to detect game disc, exiting"));
        return -2;
    }
    }
    else if (info.isFile())
    {
        //запуск IsoMaster
        IsoMaster m (0, info.absoluteFilePath());
        qDebug() << "iso: {master] - sending disk image file" << info.absoluteFilePath();
        m.lauchApp();
        return 0;
    }

}
    MainWindow w;
    w.show();
    return a.exec();

}
