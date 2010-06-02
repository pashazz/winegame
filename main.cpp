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
#include "diskdialog.h"
#include <QDir>
#include "winegameui.h"
#include "dvdrunner.h"
#include "gamedialog.h"

void runDVD (QString path, corelib *lib) //запуск с DVD
{
	DVDRunner *runner = new DVDRunner (lib, path);
	if (runner->success())
	{
		Prefix *prefix =  runner->prefix();
		GameDialog *dlg = new GameDialog (0, prefix->projectWorkingDir(), lib);
		if (dlg->exec() == QDialog::Rejected)
			return;
		qDebug() << "Installing conf " << prefix->name();
		prefix->runApplication(runner->exe(), runner->diskDirectory(), runner->imageFile());
	}
	else
	{
		qDebug() << "Not success";
		DiskDialog *dlg = new DiskDialog (0, runner, lib);
		dlg->exec();
	}
	runner->cleanup();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	//загружаем локализацию
    QTranslator qt;
    qt.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qt);
    QTranslator app;
	app.load(":/l10n/wg_" +QLocale::system().name());
	a.installTranslator(&app);
    //загружаем локализацию для WineStuff
    QTranslator winestuff;
	if(!winestuff.load(QLibraryInfo::location(QLibraryInfo::PrefixPath)+ "/share/winestuff/l10n/lwg_" + QLocale::system().name()))
		qDebug() << "No Loaded Localization for winestuff" << "PREFIX/share/winestuff/l10n/lwg_" + QLocale::system().name();
    a.installTranslator(&winestuff);
    //set Linux console encoding to UTF-8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    //Set some refspecs
    a.setApplicationName("WineGame");
    a.setApplicationVersion("0.1.0");
    a.setOrganizationName("Pashazz");
	a.setOrganizationDomain("org");
	if (!QSqlDatabase::drivers().contains("QSQLITE"))
	  {
		QMessageBox::critical(0, QObject::tr("Initialization error"), QObject::tr("Qt`s SQLite module not found"));
		  return -5;
	  }
	  //Our winegame GUI client
	WinegameUi *client = new WinegameUi(); //опасные утечки памяти

	corelib *core =  new corelib (0, client);
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

		  if (a.arguments().length() > 1) {
			  QFileInfo info (a.arguments().at(1));
			  if (!info.exists())
			  {
				  QMessageBox::critical(0,QObject::tr("Error"), QObject::tr("Incorrect commandline arguments"));
				  return -3;
			  }
			  runDVD(a.arguments().at(1), core);
			  return 0;
		  }

		  client->showNotify(QObject::tr("Hello!"),QObject::tr("Please connect to internet :)"));
		  MainWindow w(core);
		  w.show();
		  return a.exec();

	  }
