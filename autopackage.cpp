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


#include "autopackage.h"

AutoPackage::AutoPackage()
{
    //Проверяем тип второго аргумента WineGame. Если это директория, то используем режим DVD;
   QFileInfo info (qApp->arguments().at(1));
   //Я гарантирую, что класс AutoPackage будет использоваться только в DiscDetector
   if (info.isDir())
       dvd = true;
   else
   {
       dvd = false;
       //временнная конструкция
       qDebug() << "CRITICAL: Currently we dont support an ISO in AutoPackage";
    }
}
bool AutoPackage::isAutoPackage(QString dir)
{
    QSettings stg (dir + CTRL,  QSettings::IniFormat, 0);
    stg.beginGroup("application");
     //Проверяем, что значение game корректно.
    return !stg.value("game", "").toString().isEmpty();
}

void AutoPackage::load()
{
    QSettings stg (_dir + CTRL, QSettings::IniFormat, this);
   QProcessEnvironment  env= QProcessEnvironment::systemEnvironment();

    stg.beginGroup("application");
    _prefix = QDir::homePath() + winepath + QDir::separator() +  stg.value("prefix").toString();
    _game = stg.value("game").toString();
    qDebug() << "debug: myPrefix is" << _prefix <<"myGame is" << _game;
    env.insert("WINEPREFIX", _prefix);
    env.insert("WINEDEBUG", "-all");
    QProcess *p = new  QProcess (this);
   p->setProcessEnvironment(env);
    QString cdkey;
   if (stg.value("cdkey", true).toBool()) //запрашиваем CDKEY
    {
       cdkey = QInputDialog::getText(0, tr("Insert CD-KEY for this game"), tr("Please, enter CD-KEY and click OK"));
   }
   QString myArg;
    if (dvd)
    {
        //Code when we need to load DVD
                if (cdkey.isEmpty())
                    myArg="load";
     else
                    myArg = "load=" + cdkey;
    }
    else
    {
        // FIXME: No code yet
    }
    corelib::showNotify(tr("Starting disc clone"), tr("Don`t worry! WineGame clones your DVD into image. Wait ~5 mins."));
    qDebug() << "starting  wisotool" << myArg;
    p->start("wisotool " + myArg);

  p->waitForFinished(-1);
  engine::makefix(_prefix);
   startInstall(p);
}

void AutoPackage::startInstall(QProcess *p)
{
    QStringList args;
    args << _game;
    qDebug() << "/usr/bin/wisotool" << args;
    corelib::showNotify(tr("Ready to install"), tr("Now we will install your game and all needed software."));
    p->start("wisotool", args);
    p->waitForFinished(-1);
    //Now we`re set memory
    //Прочитаем значение видеопамяти.
    QSettings stg (QDir::homePath() + config, QSettings::IniFormat, this);
    QString mem = stg.value("VideoMemory").toString();
    const QString reg = "/tmp/win.reg";
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
    p->start("regedit " + reg);
    p->waitForFinished(-1);
    f.remove();
}
