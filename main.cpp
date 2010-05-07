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
    app.load(":/l10n/wg_" + QLocale::system().name());
    a.installTranslator(&app);
    //set Linux console encoding to UTF-8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
     QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

      //Set some refspecs
      a.setApplicationName("WineGame");
      a.setApplicationVersion("0.0.1");
	  a.setOrganizationName("Pashazz");
	  a.setOrganizationDomain("org");
	  if (!QSqlDatabase::drivers().contains("QSQLITE"))
	  {
		  QMessageBox::critical(0, tr("Initialization error"), tr("Qt`s SQLite module not found"));
		  return -5;
	  }
		  corelib *core =  new corelib (0);
	  core->init();
	  //Перехватываем параметр -r для запуска EXE-приложения из префикса с нужными настройками.
	  if (a.arguments().length() > 2)
	  {
		  if (a.arguments().at(1) == "-r")
		  {
			  QStringList exe = a.arguments();
			  exe.removeFirst();
			  exe.removeOne("-r");
			  core->runSingleExe(exe);
			  qApp->quit();
		  }
	  }

 //проверяем главную папочку  WineGame

if (a.arguments().length() > 1) {
    QFileInfo info (a.arguments().at(1));
    if (!info.exists())
    {
        QMessageBox::critical(0,QObject::tr("Error"), QObject::tr("Incorrect commandline arguments"));
        return -3;
}

    if (info.isDir()) //запускаем детектор диска
    {
        if (!QFile::exists(info.absoluteFilePath() + "/autorun.inf") && (!QFile::exists(info.absoluteFilePath() + "/Setup.exe"/*блядь, это все EA Games кривые*/)))
        {
            QMessageBox::critical(0, QObject::tr("I am confused"), QObject::tr ("This disc is not Windows Software disc, exiting"));
            return -2;
        }
		DiscDetector det (core);
        if (det.tryDetect(info.absoluteFilePath()))
        {
            det.lauchApp();
            return 0;
        }
        else
        {
            qDebug() << "engine: failed detect.";
            QMessageBox::warning(0, QObject::tr("Error"), QObject::tr("Failed to detect game disc, exiting"));
        return -2;
    }
    }
    else if (info.isFile())
    {
        //запуск IsoMaster

        IsoMaster m (0, info.absoluteFilePath());
        qDebug() << "iso: [master] - sending disk image file" << info.absoluteFilePath();

        bool res = m.lauchApp();
        if (!res)
        {
            QMessageBox::warning(0, QObject::tr("Error"), QObject::tr ("Error mount/unmount image"));
            return -5;
        }
        //Чистим за собой
        return 0;
    }

}
	MainWindow w(core);
    w.show();
    return a.exec();

}
