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
        QObject(parent), _workdir (workdir),  s (new QSettings (_workdir + CTRL, QSettings::IniFormat, this))
{
_prefix = s->value("application/prefix").toString();
_path = QDir::homePath() + winepath + QDir::separator() + _prefix;
//Настраиваем QProcessEnvironment
env = QProcessEnvironment::systemEnvironment();
env.insert("WINEDEBUG", "-all");
env.insert("WINEPREFIX", _path);
env.insert("WINE", wine());
}

void rp(QString path, QProcessEnvironment env)
{
    QProcess *p = new QProcess (0);
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
}
QString Prefix::wine()
{
    s->beginGroup("wine");
    QString distr = s->value("distr").toString();
    s->endGroup();
    if (distr.isEmpty())
       {
        return corelib::whichBin("wine");
    }
       else
       {
           return QDir::homePath() + winepath + "/wines/" + _prefix + "/usr/bin/wine";
       }
}

void Prefix::lauch_c()
{
    QDesktopServices::openUrl(QUrl(_path + "/drive_c"));
}
void Prefix::lauchWinetricks(QStringList args)
{
    /// так как производится установка компонентов, чтобы юзер ничего не натворил, запускаем winetricks в том же потоке.
    QProcess *p = new QProcess (this);
    p->setProcessEnvironment(env);
    qDebug() << tr("engine: [prefix]: starting winetricks");
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
