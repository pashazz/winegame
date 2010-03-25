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
void rp(QString path, QProcessEnvironment env)
{
    QProcess *p = new QProcess (0);
    p->setProcessEnvironment(env);
    p->start(path);
    p->waitForFinished(-1);
    delete p;
}
void Prefix::removePrefix()
{
    rp("rm -rf " + this->wineprefix, QProcessEnvironment::systemEnvironment());
}

Prefix::Prefix(QObject *parent, QString path) :
    QObject(parent)
{
    //прежде всего, прочтем файл Control (с помощью ф-ций  движка) и выведем значения  переменных
    _name = engine::getName(path);
    _note = engine::getName(path);
    wine = engine::getWine(path);
    qDebug() << tr("engine: setting wine binary to %1").arg(wine);
    wineprefix = QDir::homePath() + winepath + QDir::separator() + engine::getPrefixName(path);
    env = QProcessEnvironment::systemEnvironment();
    env.insert("WINE", wine);
    env.insert("WINEPREFIX", wineprefix);
    env.insert("WINEDEBUG", "-all");

}
void Prefix::runProgram(QString exepath)
{
QFuture <void> fProc = run(rp, tr("%1  \"%2\"").arg(wine).arg(exepath),  this->env);
//fProc.waitForFinished();
}
void Prefix::lauch_c()
{
  QFuture <void> fProc = run(rp, "xdg-open " + wineprefix + "/drive_c", env);
}
void Prefix::lauchWinetricks(QStringList args)
{
    /// так как производится установка компонентов, чтобы юзер ничего не натворил, запускаем winetricks в том же потоке.
    QProcess *p = new QProcess (this);
    p->setProcessEnvironment(env);
    qDebug() << tr("engine: [prefix]: starting winetricks");
    p->start("/usr/bin/winetricks", args);
     p->waitForFinished(-1);
}

