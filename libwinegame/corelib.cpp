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


#include "corelib.h"
#include "prefix.h"
//The core of WineGame. Commonly used func.
corelib::corelib(QObject *parent)
    :QObject(parent)
{
	//Init Settings object
	settings = new QSettings (config(), QSettings::IniFormat, this);
}

QString corelib::whichBin(QString bin) {
    QProcess p (0);
    p.start("which", QStringList (bin));
    p.waitForFinished(-1);
    QString path = QString(p.readAll()).trimmed();
    path.remove('\n');
    return path;
}
void corelib::init()
{
	if (!QFile::exists(whichBin("wine")))
	{
		qDebug() << "FATAL!!!! Wine not found, I WILL QUIT!";
		qApp->exit(-4);
	}
initconf();
//Init our DB.
QDir dir (wineDir());
if (!dir.exists())
  dir.mkdir(dir.path());
if (!QFile::exists(wineDir() + "/installed.db"))
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(wineDir() + "/installed.db");
	if (!db.open()){
		QMessageBox::critical(0, tr("Database error"), tr("Failed to create database for storing installed applications. See errors on console"));
		qDebug() << "DB: error: " << db.lastError().text();
		qApp->exit(-24);
}
	QSqlQuery q (db);
	q.prepare("CREATE TABLE Apps (id INTEGER PRIMARY KEY, prefix TEXT, wineprefix TEXT, wine TEXT)");
  if (!q.exec())
	{
	  QMessageBox::critical(0, tr("Database error"), tr("Failed to create table for storing installed applications. See errors on console"));
	  qDebug() << "DB: Query error " << q.lastError().text();
	  qApp->exit (-24);
  }

}
else
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(wineDir() + "/installed.db");
	if (!db.open()){
		QMessageBox::critical(0, tr("Database error"), tr("Failed to open database for storing installed applications. See errors on console"));
		qDebug() << "DB: error: " << db.lastError().text();
		qApp->exit(-24);
}
}
}

bool corelib::unpackWine (QString distr, QString destination)
{
     QDir dir (destination);
     if (!dir.exists())
         dir.mkdir(dir.path());
 QProcess *proc = new QProcess (0); //не забываем удалять
 proc->setWorkingDirectory(destination);
 QString unpackLine =  QObject::tr ("tar xvpf %1 -C %2").arg(distr).arg(destination);
 proc->start(unpackLine);
  proc->waitForFinished(-1);
 qDebug() << QObject::tr("engine: Wine distribution %1 unpacked to %2").arg(distr).arg(destination);
 return proc->exitCode() == 0 ? true : false;
	 }

QString corelib::downloadWine(QString url) //TODO: проверка на ошибки.
{
	downloadExitCode = true;
    QUrl myurl = QUrl(url);
    QFileInfo inf (myurl.path());
	QString wineFileName =QDir::tempPath() + QDir::separator() +  inf.fileName();
    //проверяем, есть ли у нас данный файл
    if (QFile::exists(wineFileName))
        return wineFileName;
     showNotify(tr("Don`t worry!"), tr("Now WineGame will download some files, that will need for get your applicaton running"));
  progress = new QProgressDialog(0);
     QEventLoop loop;
QNetworkAccessManager *manager = new QNetworkAccessManager (this);
QNetworkRequest req; //request для Url
this->progress = progress;
req.setUrl(QUrl(url));
req.setRawHeader("User-Agent", "Winegame-Browser 0.1");
QNetworkReply *reply = manager->get(req);
connect (reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(setRange(qint64,qint64)));
connect (reply, SIGNAL(finished()), &loop, SLOT(quit()));
connect (reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT (error(QNetworkReply::NetworkError)));
progress->setModal(true);
progress->setWindowTitle(tr("Downloading Wine..."));
progress->setLabelText(tr("Downloading %1").arg(url));
QPushButton *but = new QPushButton  (progress);
but->setFlat(true);
but->setDisabled(false);
but->setText(tr("Cancel"));
connect (but, SIGNAL(clicked()), this, SLOT (exitApp()));
progress->setCancelButton(but);
progress->show();
loop.exec();
progress->close();
QByteArray buffer = reply->readAll();
QFile file (wineFileName);
if (file.open(QIODevice::WriteOnly))
{
        file.write(buffer);
        file.close();
                    }
else
    qDebug() << "engine: error open file (WINEDISTR):" << file.errorString();
progress->deleteLater();
return downloadExitCode ? wineFileName : "";
}


void corelib::error(QNetworkReply::NetworkError error)
{
    if  (error != QNetworkReply::NoError)
    {
       return;
   }
    else
    {
		QMessageBox::warning(0, tr("Network error"), tr("Network error: %1. Exiting installation"));
	  downloadExitCode = false;
    }

}
void corelib::showNotify (QString header, QString body) //функция НУ СОВСЕМ не доделана.
{
/// знаю что тупизм,но никто не хочет помогать
    if (QProcessEnvironment::systemEnvironment().contains("KDE_FULL_SESSION")) //пока кеды юзают KDialog
        //вся земля юзает notify-send
        //чезез kdialog:
    {
                             QStringList arguments;
                            arguments << "--passivepopup" <<body;
                            arguments << "--title"<<header;
                            QProcess::startDetached("/usr/bin/kdialog",arguments);
                                      }


        //Через notify-send:
    else
    {
                             QStringList arguments;
                            arguments << header << body;
                            QProcess::startDetached("/usr/bin/notify-send",arguments);
                        }

   }

void corelib::setRange(qint64 aval, qint64 total)
{
    int kbAval = aval;
    int kbTotal = total;
    progress->setMaximum(kbTotal);
    progress->setValue(kbAval);
}

void corelib::exitApp()
{
	QMessageBox::critical(0, tr("Critical error"), tr("Wine distribution not downloaded, so exit installation."));
	downloadExitCode = false;
}

bool corelib::checkPrefixName(QString prefix)
{
    //пока что тут у нас проверяется на пробелы.
    if (prefix.contains(' '))
        return false;
 if (prefix == "wines") //зарезервировано для вайнов
     return false;
 return true;
}

void corelib::runSingleExe(QStringList exe)
{
	QString wineprefix = QProcessEnvironment::systemEnvironment().value("WINEPREFIX");
	if (wineprefix.isEmpty())
	{
		qDebug() << "winegame: Wineprefix not set, exiting.";
		return;
	}
	// Выбираем бинарник Wine по данному WINEPREFIX
	QSqlQuery q(db);
	q.prepare("SELECT wine FROM Apps WHERE wineprefix=:pr");
	q.bindValue(":pr", wineprefix);
	q.exec();
	q.first();
	QString wine =q.value(0).toString();
	if (wine.isEmpty())
	{
	  qDebug() << "Wine from WineGame not found, use default";
		wine = whichBin("wine");
	}
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	env.insert("WINEDEBUG", "-all");
 QProcess proc (this);
 proc.start(wine, exe);
 proc.waitForFinished(-1);
}

void corelib::initconf()
{
	//Init our configuration.
	if (QFile::exists(config()))
		return;
	qDebug() << "winegame: Init configuration";

	int mem = 0;
	 mem= QInputDialog::getInt(0, QObject::tr("WineGame"), QObject::tr("Enter memory size of your video card (in megabytes). If you click Cancel, then default will be used"), 128, 1, 4096);
		if (mem == 0)
			mem = 128;
		setVideoMemory(mem);
		setWineDir(QDir::homePath() + "/Windows");
		setMountDir(QDir::homePath() + "/game");
		setPackageDir("/usr/share/winegame");
}

QString corelib::wineDir() {
	return settings->value("WineDir").toString();
}
QString corelib::packageDir() {
	return settings->value("PackageDir").toString();
}
QString corelib::mountDir() {
	return settings->value("MountDir").toString();
}
void corelib::setWineDir(QString dir)
{
	settings->setValue("WineDir", dir);
}
void corelib::setPackageDir(QString dir)
{
	settings->setValue("PackageDir", dir);
}
void corelib::setMountDir(QString dir)
{
	settings->setValue("MountDir", dir);
}
void corelib::setVideoMemory(int memory)
{
	int oldMemory = settings->value("VideoMemory").toInt();
	if (oldMemory == memory)
		return;
	settings->setValue("VideoMemory", memory);
	settings->sync(); //we need to force sync
	//Sync all videomemory entries
	QDir dir (packageDir());
	foreach (QFileInfo info, dir.entryInfoList(QDir::Dirs | QDir::Readable))
	{
		//construct prefix obj
		Prefix *prefix = new Prefix (this, info.absoluteFilePath());
		if (prefix->hasDBEntry())
		{
			prefix->checkWineDistr();
			prefix->setMemory();
		}
	}

}
QString corelib::videoMemory()
{
	return settings->value("VideoMemory").toString();
}

QString corelib::autorun(QString diskRoot)
{
	QStringList autorunNames;
	autorunNames.append("autorun.inf");
	autorunNames.append("Autorun.inf");
	autorunNames.append("AUTORUN.INF");
	autorunNames.append("AutoRun.inf");
 QDir dir (diskRoot);
 qDebug() << "autorun: diskroot" << diskRoot;
 foreach (QString fileName,  dir.entryList(QDir::Files | QDir::Readable))
 {
	 if (autorunNames.contains(fileName, Qt::CaseSensitive))
	 {
		 qDebug()  << diskRoot + QDir::separator() + fileName;
		 return diskRoot + QDir::separator() + fileName;
	 }
 }
 return "";
}


corelib::~corelib()
{
	QSqlDatabase db = QSqlDatabase::database();
	db.close();

}

QString corelib::getSudoProg()
{
	QFile file;
	QStringList programs = QStringList () << "kdesu" << "gksu" << "xdg-su";
	foreach (QString str, programs)
	{
		file.setFileName(whichBin(str));
		if (file.exists())
			return str;
	}
	return "";
}

bool corelib::forceFuseiso()
{
	return settings->value("ForceFuseiso", false).toBool();
}

void corelib::setForceFuseiso(bool value)
{
	QFile file (corelib::whichBin("fuseiso"));
	if (file.exists())
		settings->setValue("ForceFuseiso", value);
}

QString corelib::config()
{
	if (QProcessEnvironment::systemEnvironment().contains("XDG_CONFIG_HOME"))
		return QProcessEnvironment::systemEnvironment().value("XDG_CONFIG_HOME") + QDir::separator() + "winegame.conf";
	else
		return QDir::homePath() + "/.cofig/winegame.conf";

}
