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


#include "prefix.h"

using namespace QtConcurrent;
Prefix::Prefix(QObject *parent, QString workdir) :
		QObject(parent), _workdir (workdir),  s (new QSettings (_workdir + "/control", QSettings::IniFormat, this)), core (new corelib(parent))
{
	db = QSqlDatabase::database();
_prefix = s->value("application/prefix").toString();
getPrefixPath();
//Настраиваем QProcessEnvironment
env = QProcessEnvironment::systemEnvironment();
env.insert("WINEDEBUG", "-all");
env.insert("WINEPREFIX", _path);
//env.insert("WINE", wine());
}

void rp(QString path, QProcessEnvironment env)
{
	QProcess *p = new QProcess (0);
	QFileInfo file (path);
	if (file.exists())
		p->setWorkingDirectory(file.absolutePath());
	else
		p->setWorkingDirectory(env.value("WINEPREFIX"));
    p->setProcessEnvironment(env);
	p->start(path);
    p->waitForFinished(-1);
    delete p;
}

void Prefix::runProgram(QString exe)
{
QFuture <void> fProc = run(rp, tr("%1  \"%2\"").arg(wine()).arg(exe),  this->env);
//fProc.waitForFinished();
}

void Prefix::removePrefix()
{
    rp("rm -rf " + this->_path, QProcessEnvironment::systemEnvironment());
	QSqlQuery q(db);
	q.prepare("DELETE FROM Apps WHERE prefix=:pr");
	q.bindValue(":pr", _prefix);
	if (!q.exec())
		qDebug() << "WARNING: Unable to execute query to delete Prefix";
}
QString Prefix::wine()
{
/// новый метод для получения wine из БД.
/// DO NOT USE IT BEFORE CALLING installFirstApplication (in first)
	QSqlQuery q (db);
	q.prepare("SELECT wine FROM Apps WHERE prefix=:prefix");
	q.bindValue(":prefix", _prefix);
	if (!q.exec())
	  {
		QMessageBox::critical(0, tr("Database error"), tr("Failed to fetch wine"));
		return core->whichBin("wine");
			}
q.first();
QString wine  = q.value(0).toString();
if (wine.trimmed().isEmpty())
{
//	qDebug << "Can not get a wine, use system";
  wine = core->whichBin("wine");
}
env.insert("WINE", wine); //hack
return wine;
}

void Prefix::lauch_c()
{
	QDesktopServices::openUrl(QUrl(_path + "/drive_c")); //bad func. WINEPREFIX and WINE not set
}
void Prefix::lauchWinetricks(QStringList args)
{
    /// так как производится установка компонентов, чтобы юзер ничего не натворил, запускаем winetricks в том же потоке.
    QProcess *p = new QProcess (this);
    p->setProcessEnvironment(env);
    qDebug() << tr("engine: [prefix]: starting winetricks");
	args.prepend("-q");
    p->start(corelib::whichBin("winetricks"), args);
     p->waitForFinished(-1);
}


QString Prefix::standardExe()
{
    s->beginGroup("application");
    QString exe = s->value("exepath").toString();
    s->endGroup();
    return exe;
}

QString Prefix::name()
{
    //ищем файл .name в данном _workdir
    QFile file (_workdir + QDir::separator() + ".name." + QLocale::system().name() );
    if (file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString ret = file.readAll();
        file.close();
        return ret;
    }
    else
    {
 //cначала попробуем открыть просто .name
        file.setFileName(_workdir + QDir::separator() + ".name");
        if (file.exists())
        {
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QString ret = file.readAll();
            file.close();
            return ret;
        }
        else
        {
            QDir dir (_workdir);
            return dir.dirName();
        }
    }
}

QString Prefix::note()
{
    QString fileName;
    if (QFile::exists(_workdir + "/.note." + QLocale::system().name())) //читаем локализованное примечание
        fileName = _workdir + "/.note." + QLocale::system().name();
    else if (QFile::exists((_workdir + "/.note")))
        fileName = _workdir + "/.note";
    else
        return QString();
QFile file (fileName);
file.open(QIODevice::Text | QIODevice::ReadOnly);
QString note = file.readAll();
file.close();
return note;

}

QIcon Prefix::icon()
{
            if (QFile::exists(_workdir + "/icon"))
        {
            QIcon icon (_workdir + "/icon");
            return icon;
    }
        else
            return QIcon::fromTheme("application-default-icon");

    }
bool Prefix::isPreset()
{
    return s->value("wine/preset").toBool();
}


bool Prefix::checkWineDistr()
{
   /// проверяет дистрибутив Wine для префикса. Если проверка не удается, загружает дистрибутив заново.
    //Если у нас системный Wine, то и делать неча.
if (s->value("wine/distr").isNull())
	return true;
    QFile file (_path + QDir::separator() + ".wine");
    QTextStream stream (&file);

	if ((!file.exists()) || (!QFile::exists(wine())))
    {
        //Загружаем Wine
        QString wineUrl = downloadWine();
		if (wineUrl.isEmpty())
			return false;
        //записываем Wine в .wine
		file.open(QIODevice::WriteOnly | QIODevice::Text);
        stream << wineUrl;
        qDebug() << "Prefix: writing wine into" << file.fileName();
        file.close();

    }
    else
    {
        //Открываем файл для чтения
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return false;

        QString installedWine = stream.readAll().trimmed();
		file.close();
        if (installedWine != s->value("wine/distr").toString())
        {
			file.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);
			stream << s->value("wine/distr").toString();
			file.close();
        }


    }
	return true;
}

QString Prefix::downloadWine() {
    QString wineBinary;
    if (!s->value("wine/distr").toString().isEmpty())
    {
        QString distr = s->value("wine/distr").toString();
    //здесь запускаем процесс закачки и распаковки данного дистрибутива Wine
		QString destination = core->wineDir()+ QString("/wines/") + prefixName();
		QDir dir (core->wineDir()+ "/wines");
        if (!dir.exists())
            dir.mkdir(dir.path());
		qDebug() << "WINE IS DOWNLOADING FROM" << distr << "to" <<QDir::tempPath();
		  corelib * core = new corelib(this);
		QString distrname =   core->downloadWine(distr);
		if (distrname.isEmpty())
			return "";
            qDebug() << "WINE IS UNPACKING TO " << destination << "FROM" << distrname;
			if (!core->unpackWine(distrname, destination))
				return "";
        qDebug() << "wine distribution is" << distr;
		return distr;
   }
    else
    {
wineBinary = wine();
    }
    //если wineBinary все еще не установлен
return "";
}

bool Prefix::installFirstApplication()
{
	/// Проверяем Wine, загружаем его если надо, маркируем приложение как установленное и передаем управление движку'
	// Данная процедура не нужна, если мы используем презет; тут все дефолтно.
	if (s->value("application/prefix").isNull())
		return true; //skip this step.
QDir dir (_path);
if (!dir.exists())
	dir.mkpath(dir.path());

	if (!checkWineDistr())
		return false;
	//Db Working
	QSqlQuery q (db);
	q.prepare("INSERT INTO Apps (prefix, wineprefix, wine) VALUES (:prefix, :wineprefix, :wine)");
	q.bindValue(":prefix", _prefix );
	q.bindValue(":wineprefix", _path);
	QString wine;
	s->beginGroup("wine");
	QString distr = s->value("distr").toString();
	s->endGroup();
	if (distr.isEmpty())
	   {
		wine = corelib::whichBin("wine");
	}
	   else
	   {
		   wine = core->wineDir()+ "/wines/" + _prefix + "/usr/bin/wine";
	   }
	   q.bindValue(":wine", wine);
	   if (!q.exec())
	   {
		   QMessageBox::critical(0, tr("Database error"), tr("Failed to execute query for application. See errors on console"));
		   qDebug() << "DB: Query error " << q.lastError().text();
		   return false;
	   }
	   return true;
}

bool Prefix::hasDBEntry()
{
	QSqlQuery q (db);

	q.prepare("SELECT * FROM Apps WHERE prefix=:pr");
	q.bindValue(":pr", _prefix);
	q.exec();
	return q.first();
}

void Prefix::setMemory()
{
	if (!s->value("wine/memory").toBool())
		return;
QTemporaryFile f (this);


	QTextStream stream (&f);
	f.open();
	stream << "\n";
	stream << "REGEDIT4\n";
	stream << "[HKEY_CURRENT_USER\\Software\\Wine\\Direct3D]";
	stream << "\n";
	stream << "\"VideoMemorySize\"=";
	stream << tr("\"%1\"").arg(core->videoMemory());
	stream << "\n";
	f.close();
QProcess p (this);
QStringList args;
args << "regedit";
args << f.fileName();
p.setProcessEnvironment(env);
p.start(wine(), args);
p.waitForFinished(-1);
f.remove();
}

void Prefix::getPrefixPath()
{
	if (isPreset() || _prefix.trimmed().isEmpty())
		return;

	if (hasDBEntry())
	{
		//пытаемся получить инфу из бд.
		QSqlQuery q(db);
		q.prepare("SELECT wineprefix FROM Apps WHERE prefix=:prefix");
		q.bindValue(":prefix", _prefix);
		if (q.exec())
		{
			q.first();
			_path=q.value(0).toString();
			if (!_path.isEmpty())
				return;
		}
		else
		{
			_path = core->wineDir() + QDir::separator() + _prefix;
			return;
		}
	}
		//ничего у нас не получилось, используем старый fallback-метод
		_path = core->wineDir() + QDir::separator() + _prefix;
	}

void Prefix::makeDesktopIcon(const QString &path, const QString &name)
{
	//For now, we use QDesktopServices::desktop
	QFile file  (QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + QDir::separator() + name + ".desktop");
	QTextStream str (&file);
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	str << "[Desktop Entry]\n";
	str << QString ("Name=%1\n").arg(name);
	str << "Type=Application\n";
	str << QString("Exec=env WINEPREFIX=\"%1\" winegame -r \"%2\"\n").arg(_path).arg(path);
	str << QString ("Icon=%1\n").arg(_workdir + QDir::separator() + "icon");
	str << QString ("Comment=%1\n").arg(note());
	str << QString("Categories=%1\n").arg("Game;"); //temporalily,
	file.close();
}

QString Prefix::getRunnableExe()
{
	QString exe;
	QString diskpath;
	//Получаем diskpath
	if (qApp->arguments().length() > 1)
	{
	   //Анализируем значение 2-го аргумента
		QFileInfo info (qApp->arguments().at(1));
		qDebug() << qApp->arguments().at(1) << "is the path";

		if (info.isDir()) //Нас вызвали с директорией.
			diskpath = info.absoluteFilePath();
		else if (info.isFile()) //Логично, это образ ISO/MDF/BIN/NRG
			diskpath = core->mountDir();

		if (diskpath.isEmpty())
			return selectExe();
		//force application/setup
			if (!s->value("application/setup").isNull()){
		exe = diskpath + QDir::separator() +  s->value("application/setup").toString();
		if (QFile::exists(exe))
			return exe;
	}

		//Теперь просмотрим AutoRun
		if (!core->autorun(diskpath).isEmpty())
		{
			QSettings autorun(core->autorun(diskpath), QSettings::IniFormat, this);
			autorun.beginGroup("autorun");
			if (!autorun.value("open").toString().isEmpty())
			{
				exe = diskpath + QDir::separator() + autorun.value("open").toString();
		}
			if (QFile::exists(exe))
				return exe;
		}
	}
	//А теперь спросим EXE у пользователя.
return selectExe();
}

QString Prefix::selectExe()
{
	QString
	exe = QFileDialog::getOpenFileName(0,  tr("Select EXE file"), QDir::homePath(), tr("Windows executables (*.exe)"));
	return exe;
}

bool Prefix::runApplication(QString exe)
{
	if (hasDBEntry())
	{
		runProgram(exe);
		return true;
	}
	//перемещаем все из метода engine сюда
}
