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

#include "engine.h"
#include "prefix.h"
#include <QtDebug>
#include <QPushButton>
#include <QInputDialog>
using namespace QtConcurrent;
engine::engine(QObject *parent) : //сейчас мы не делаем ничего
    QObject(parent)
{
core = new corelib (this);
}
void engine::lauch(QString workdir, bool msg)
{
	QSettings s (workdir + "/control",  QSettings::IniFormat, this);
	if (QFile::exists(workdir + "/control"))
		controlFile = workdir + "/control";
    else
        return;
   Prefix *wPrefix = new Prefix (this, workdir);

    QString prefixName = myPrefixName(); //собственно имя префикса
	if (prefixName.isEmpty())
	{
		QMessageBox::critical(0, tr("Critical error"), tr("No suitable prefix set"));
		return;
	}
	prefix = core->wineDir() + QDir::separator() + prefixName; //путь к префиксу
  QString exe = getRunnableExe();
  qDebug() << "EXE FOR RUN" << exe;
  if (exe.isEmpty())
  {
         QMessageBox::warning(0, tr("No EXE file found"), tr("Operation cancelled: No EXE file selected"));
            return;
     }
  wPrefix->installFirstApplication();
//ищем контейнер префикса
if (!s.value("application/container").toString().isEmpty())
{
    QString container = core->downloadWine(s.value("application/container").toString());
	core->unpackWine(QDir::tempPath() + QDir::separator() + container, prefix);
}

//проверяем дистрибутив Wine
 name = wPrefix->name();
note = wPrefix->note();
wineBinary = wPrefix->wine();
program = wPrefix->standardExe();
QProcessEnvironment myEnv = QProcessEnvironment::systemEnvironment();
qDebug() << tr("engine: setting Prefix: %1").arg(prefix);
myEnv.insert("FILESDIR", workdir + "/files");
myEnv.insert("WINEPREFIX", prefix);
myEnv.insert("WINE", wineBinary);
myEnv.insert("CDROOT", this->diskpath);
myEnv.insert("WINEDEBUG", "-all");

/// Устанавливаем компоненты Microsoft здесь. Вы можете скачать их в ~/.winetrickscache, чтобы программа не загружала их сама
if (!s.value("wine/components").toString().isEmpty())
    qDebug() << "installing components" << s.value("wine/components").toString();
    doPkgs(s.value("wine/components").toString(),myEnv);

    //Делаем ссылки на CD-диск.
     makecdlink();

    //запускаем скрипт Preinst
QProcess *proc = new QProcess (this);
proc->setProcessEnvironment(myEnv);
proc->setWorkingDirectory(getExeWorkingDirectory(exe));
if (QFile::exists(workdir + "/preinst"))
{
    proc->start ("\"" +workdir + "/preinst\"");
proc->waitForFinished(-1);
}

qDebug() << tr("engine: starting Windows program %1 with wine binary %2").arg(exe).arg(wineBinary) << proc->workingDirectory();
qDebug() << wineBinary + " \"" + exe  +"\"" ;
proc->start(wineBinary + " \"" + exe  +"\"" );
proc->waitForFinished(-1);

//ну а теперь финальная часть, запуск postinst
if (QFile::exists(workdir + "/postinst"))
{
proc->start("\"" + workdir + "/postinst\"");
proc->waitForFinished(-1);
makefix(prefix);
}
wPrefix->setMemory();
if (msg) {
int result = QMessageBox::question(0, tr("Question"), tr("Would you like to install a new game?"), QMessageBox::Yes, QMessageBox::No);
if (result == QMessageBox::No)
    qApp->quit();
}
              }

void engine::doPkgs(QString pkgs, const QProcessEnvironment &env)
{
    if (pkgs.trimmed().isEmpty())
        return;
    corelib::showNotify(tr("Downloading packages..."), tr("Now we will install Microsoft components"));
    QProcess p (this);
  p.setProcessEnvironment(env);
    p.start(core->whichBin("winetricks") + " " + pkgs);
    p.waitForFinished(-1);
}


void engine::makefix(QString prefix)
{
    QFile file (prefix + "/system.reg");
    QTextStream stream (&file);
   //сначала откроем реестр для чтения
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString registry = stream.readAll();
    file.close();
    registry.replace ("winebrowser.exe -nohome", "winebrowser.exe -nohome %1");
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    stream << registry;
    file.close();
}

void engine::makecdlink()
{
  //Запустили меня с ISO или нет?
    if (!cdMode)
        return;
    if (qApp->arguments().count() < 2)
        return;
    QFileInfo info (qApp->arguments().at(1));
    if (info.isDir())
    {
        //делаем ссылки: d:: на /dev/cdrom и d: на нашу папку.
        QString devcdrom;
        if (QFile::exists("/dev/cdrom"))
            devcdrom = "/dev/cdrom";
        else  //берем /dev/sr0
            devcdrom = "/dev/sr0";

        QFile::link(devcdrom, prefix + "/dosdevices/d::");
        QFile::link(info.absoluteFilePath(), prefix + "/dosdevices/d:");

        //Мы должны зашить D:/ в программу
        //потому что это стандарт для скриптов (preinst/postinst)
    }
    else
    {
        //Значит, мы имеем дело с файлом .iso/.mdf/.nrg
        QFile::link(qApp->arguments().at(1), prefix + "/dosdevices/d::");
		QFile::link(core->mountDir(),  prefix + "/dosdevices/d:");
    }

}

QString engine::getRunnableExe()
{
    QString exe;
	if (cdMode) {
	//force application/setup
	QSettings stg (controlFile, QSettings::IniFormat, this);
	exe = diskpath + QDir::separator() +  stg.value("application/setup").toString();
	if (QFile::exists(exe) && (!exe.isEmpty()))
		return exe;
    //Теперь просмотрим AutoRun
	qDebug() << core->autorun(diskpath);
	if (!core->autorun(diskpath).isEmpty())
    {
		QSettings autorun(core->autorun(diskpath), QSettings::IniFormat, this);
        autorun.beginGroup("autorun");
		if (!autorun.value("open").isNull())
			exe = diskpath + QDir::separator() + autorun.value("open").toString();
		qDebug() << exe;
		if (QFile::exists(exe) && (!exe.isEmpty()))
            return exe;
        else
            exe = ""; //дальше поехали
    }
}
    //А теперь спросим EXE у пользователя.
	exe = QFileDialog::getOpenFileName(0,  tr("Select EXE file"), QDir::homePath(), tr("Windows executables (*.exe)"));
    return exe;
}
QString engine::myPrefixName ()
{
    QSettings stg (controlFile, QSettings::IniFormat, this);
    if (stg.value("wine/preset").toBool())
    {
        //Необходимо получить имя префикса
        //Get PREFIX variable
           dialog:
           QString myPrefix =QInputDialog::getText(0, tr("Give a name for your application"), tr ("Give a short latin name for your application. <br> It will be installed in %1/Windows/APPLICATION NAME/drive_c").arg(QDir::homePath()));
           if (myPrefix.isEmpty())
           {
               QMessageBox::critical(0, tr("No name specified"), tr ("Abort. Reason: No application name specified"));
               return "";
           }
           //проверим, есть ли такой префикс уже.
           QDir dir;
		   dir.setPath(core->wineDir() + QDir::separator() + myPrefix);
           if (dir.exists()){

			   QMessageBox::warning(0, tr("Application with this name is already installed."), tr("To force installation process, remove directory %1.").arg(core->wineDir() +QDir::separator() + myPrefix));
               //я знаю, это плохо
               goto dialog;
           }
           if (!corelib::checkPrefixName(myPrefix))
               goto dialog;
      return myPrefix;
    }
    else
    {
        return stg.value("application/prefix").toString();
    }
}

QString engine::getExeWorkingDirectory(QString exe)
{
    QFileInfo info (exe);
    return info.absolutePath();
}
