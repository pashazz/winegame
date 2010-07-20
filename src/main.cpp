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
#include "ejectdialog.h"

void runDVD (QString path, corelib *lib) //запуск с DVD
{
	PluginWorker *worker = new PluginWorker(qApp, lib);
	DVDRunner *runner = new DVDRunner (lib, path, worker);
	if (lib->autorun(runner->diskDirectory()).isEmpty() && runner->isMounted())
	{
		if (QMessageBox::warning(0, QObject::tr("Incorrect path"), QObject::tr("This does not like a disc with software: no autorun.inf found."), QMessageBox::Cancel, QMessageBox::Ignore) == QMessageBox::Cancel)
			return;
	}
	if (runner->success())
	{
		SourceReader *reader = runner->sourceReader();
		GameDialog *dlg = new GameDialog (0, reader, lib);
		if (dlg->exec() == QDialog::Rejected)
		{
			runner->cancel();
			runner->cleanup();
			return;
		}

		/* о возможности сменить диск*/
		EjectDialog *edlg = new EjectDialog ();
		edlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::Tool);

		QObject::connect(edlg, SIGNAL(ejectRequested(bool&)), runner, SLOT(eject(bool&)));
		edlg->show();

		PrefixCollection collection (lib->database(), lib, 0);
		if (collection.havePrefix(reader->ID()))
		{
		 Prefix *prefix =	collection.getPrefix(reader->ID());
		 prefix->setDiscAttributes(runner->diskDirectory(), runner->device());
		 prefix->runApplication(runner->exe());
		}
		else
		{
			QStringList obj = QStringList () << runner->diskDirectory() << runner->device();
			collection.install(reader, runner->exe(), obj, false);
		}
		edlg->close();
		edlg->deleteLater();
	}
	else
	{
		if (runner->isMounted())
		{
		DiskDialog *dlg = new DiskDialog (0, runner, lib, worker);
		dlg->exec();
	}
	}
if (runner->isMounted())
	runner->cleanup();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	//загружаем локализацию
#ifdef QT_NO_CONCURRENT
	qDebug() << "No QtConcurrent support";
	return 0;
#endif
    QTranslator qt;
    qt.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qt);
    QTranslator app;
	if (!app.load("wg_" + QLocale::system().name(), QDir(a.applicationDirPath() + "/../share/winegame/translations").absolutePath()))
		qDebug() << "Failed to load translation" << "wg_" + QLocale::system().name() << "in dir" << QDir(a.applicationDirPath() + "/../share/winegame/translations").absolutePath();
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
	a.setApplicationVersion("0.1.91");
    a.setOrganizationName("Pashazz");
	a.setOrganizationDomain("org");
#ifdef Q_WS_WIN
	QMessageBox::critical(0, tr("Proprietary OS detected"), tr("Winegame will not work  when Ballmer sees."));
	return 0;
#endif
#ifdef Q_WS_MAC
	QMessageBox::critical(tr("Proprietary OS detected"), tr("Winegame will not work  when Jobs sees."));
	return 0;
#endif
	if (!QSqlDatabase::drivers().contains("QSQLITE"))
	{
		QMessageBox::critical(0, QObject::tr("Initialization error"), QObject::tr("Qt`s SQLite module not found"));
		return -5;
	}
	  //Our winegame GUI client
	WinegameUi *client = new WinegameUi(); //опасные утечки памяти
	corelib *core = new corelib (0, client);
	core->init(QDir::homePath() + "/." + a.applicationName ().toLower());
	//Перехватываем параметр -r для запуска EXE-приложения из префикса с нужными настройками.
	if (a.arguments().length() > 2)
	{
		  if (a.arguments().at(1) == "-r")
		{
			  QStringList exe = a.arguments();
			  exe.removeFirst();
			  exe.removeOne("-r");
			  core->runSingleExe(exe);
			  return 0;
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
	
	MainWindow w(core);
	w.show();
	return a.exec();
}
