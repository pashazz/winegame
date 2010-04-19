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
#include "enginefunc.h"
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
    QSettings s (workdir + CTRL,  QSettings::IniFormat, this);
    if (QFile::exists(workdir + CTRL))
        controlFile = workdir + CTRL;
    else
        return;
   Prefix *wPrefix = new Prefix (this, workdir);

    QString prefixName = myPrefixName(); //собственно имя префикса
    prefix = QDir::homePath() + winepath + QDir::separator() + prefixName; //путь к префиксу
  if (prefixName.isEmpty())
  {
      QMessageBox::critical(0, tr("Critical error"), tr("No suitable prefix set"));
      return;
  }
  QString exe = getRunnableExe();
  if (exe.isEmpty())
  {
         QMessageBox::warning(0, tr("No EXE file found"), tr("Operation cancelled: No EXE file selected"));
            return;
     }
//ищем контейнер префикса
if (!s.value("application/container").toString().isEmpty())
{
    QString container = core->downloadWine(s.value("application/container").toString());
    core->unpackWine(TMP + QDir::separator() + container, prefix);
}

//найдем переменную WINEDISTR для развертывания Wine (если есть)
if (!s.value("wine/distr").toString().isEmpty())
{
    QString distr = s.value("wine/distr").toString();
//здесь запускаем процесс закачки и распаковки данного дистрибутива Wine
    QString destination = QDir::homePath() + winepath + "/wines/" + prefixName;
    QDir dir (QDir::homePath() + winepath + "/wines");
    if (!dir.exists())
        dir.mkdir(dir.path());
//а может быть Wine уже распакован, м
//теперь устанавливаем переменную wineBinary
wineBinary = destination + "/usr/bin/wine"; //дададада!
qDebug() << "engine: setting wine binary to " << wineBinary;
if (!QFile::exists(wineBinary))
{
    QString distrname =   core->downloadWine(distr);
    core->unpackWine(distrname, destination);
}
//выходим из условия
}
else
{
    //запускаем процесс which wine
    wineBinary =  core->whichBin("wine");
            qDebug() << "engine: setting wine binary to " << wineBinary; // да я знаю, что можно было обойтись одной строкой

}
//если wineBinary все еще не установлен
if (wineBinary.isEmpty())
{
    QMessageBox::warning(0, tr("Error"), tr("Wine installation not found."));
    qDebug() << "engine: exiting";
    return;
}
 name = wPrefix->name();
note = wPrefix->note();
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
/// http://bugs.winehq.org/show_bug.cgi?id=22069 (wine bug - working with desktop files)
if (s.value("wine/memory", false).toBool())
{
    //получаем видеопамять.
    QSettings stg (QDir::homePath() + config, QSettings::IniFormat, this);
   //для совместимости, берем значение в int (а вдруг там нам не int подсунули, или <= 0
    QString mem = stg.value("VideoMemory", -1).toString();
    if (mem.toInt() > 0)
    {
        setMemory(mem);
    }
}

program = s.value("application/exepath").toString();
if (!program.isEmpty())
{
    //получаем иконку
    QString icon;
    if (cdMode)
    {
    QSettings stg (diskpath+ "/autorun.inf", QSettings::IniFormat, this);
    stg.beginGroup("autorun");
     icon = diskpath + QDir::separator() + stg.value("Icon").toString();
    qDebug() << "engine: ico file detected" << icon;
}
    else
    {
        if (QFile::exists(workdir+"/icon"))
        icon = workdir+"/icon";
    }
    this->iconPath = icon;
    doDesktop(s.value("application/prefix").toString());
}
if (msg) {
int result = QMessageBox::question(0, tr("Question"), tr("Would you like to install a new game?"), QMessageBox::Yes, QMessageBox::No);
if (result == QMessageBox::No)
    qApp->quit();
}
              }

void engine::doPkgs(QString pkgs, const QProcessEnvironment &env)
{
    corelib::showNotify(tr("Downloading packages..."), tr("Now we will install Microsoft components"));
    QProcess p (this);
  p.setProcessEnvironment(env);
    p.start(core->whichBin("winetricks") + " " + pkgs);
    p.waitForFinished(-1);
}


void engine::doDesktop(QString workname)
     {
    /// эта функция - надеюсь, временная. Используется с переменной EXEPATH из control. Все из-за бага Wine: http://bugs.winehq.org/show_bug.cgi?id=17055
    // проголосуйте за исправление этого бага, пожалуйста. Тогда я удалю эту ф-цию к черту.
    QString desktopFile =QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + QDir::separator()+ workname + ".desktop"; //создаем ярлык на десктопе
    qDebug() << "debug: writing to" << desktopFile;

   QFile file (desktopFile);
   file.open(QIODevice::WriteOnly | QIODevice::Truncate);
   QTextStream stream (&file);
   stream <<  "[Desktop Entry]\n";
    QString exec = QObject::tr("env WINEPREFIX='%1' '%2'  '%3'").arg(prefix).arg(wineBinary).arg(program);
    stream << tr("Exec=%1\n").arg(exec);
    stream << tr("Name=%1\n").arg(name);
    stream << tr("Comment=%1\n").arg(note);
    stream << "Type=Application\n";
    stream << "Terminal=false\n";
       QFile iconFile (iconPath);
    QFileInfo info (iconFile);
    if (iconFile.exists()) {
    iconFile.copy(QDir::homePath() + "/.local/share/icons/" + info.fileName());
    stream << tr("Icon=%1").arg(QDir::homePath() + "/.local/share/icons/" + info.fileName());
}
 file.close();
}
void engine::setMemory(QString mem)
{
    QStringList args;
    args << "regedit";
    args << reg;
    QFile f (reg);
    QTextStream stream (&f);
    f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    stream << "\n";
    stream << "REGEDIT4\n";
    stream << "[HKEY_CURRENT_USER\\Software\\Wine\\Direct3D]";
    stream << "\n";
    stream << "\"VideoMemorySize\"=";
    stream << tr("\"%1\"").arg(mem);
    stream << "\n";
    f.close();
QProcess p (this);
QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
env.insert("WINEPREFIX", prefix);
p.setProcessEnvironment(env);
p.start(wineBinary, args);
p.waitForFinished(-1);
f.remove();
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
        QFile::link(QDir::homePath() + MOUNT_DIR,  prefix + "/dosdevices/d:");
    }

}

QString engine::getRunnableExe()
{
    QSettings stg (controlFile, QSettings::IniFormat, this);
    //Для начала посмотрим application/setup
    QString exe;
    if ((!stg.value("application/setup").toString().isEmpty())  && QFile::exists(diskpath + QDir::separator() + stg.value("application/setup").toString()) )
    {
        exe = diskpath + QDir::separator() + stg.value("application/setup").toString();
        return exe;
    }
    //Теперь просмотрим AutoRun
    if (QFile::exists(diskpath + "/autorun.inf"))
    {
        QSettings autorun(diskpath + "/autorun.inf", QSettings::IniFormat, this);
        autorun.beginGroup("autorun");
        exe = diskpath + QDir::separator() + stg.value("open").toString();
        if (QFile::exists(exe))
            return exe;
        else
            exe = ""; //дальше поехали
    }
    //А теперь спросим EXE у пользователя.
    exe = QFileDialog::getOpenFileName(0,  tr("Выберите EXE файл"), QDir::homePath(), tr("Windows executables (*.exe)"));
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
           dir.setPath(QDir::homePath() + winepath + QDir::separator() + myPrefix);
           if (dir.exists()){

               QMessageBox::warning(0, tr("Application with this name is already installed."), tr(" To force installation process, remove directory %1.").arg(QDir::homePath() + winepath +QDir::separator() + myPrefix));
               //я знаю, это плохо
               goto dialog;
           }
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
