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
#include "prefixdialog.h"
#include "discdetector.h"
DiscDetector::DiscDetector(QObject *parent) :
    QObject(parent)
{
}
bool DiscDetector::tryDetect(QString path)
{
    /// path - путь к корню CD/DVD
    cdroot = path;
    // для начала просмотрим все папочки в gamepath
    qDebug() << tr("DDT: [scan] scanning dir %1").arg(path);
    QDir dir (gamepath);
    foreach (QString dir, dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
    {
        //читаем файл .cdrom
        qDebug() <<  tr("DDT: scanning file %1").arg(gamepath + QDir::separator() + dir + "/.cdrom");
        QFile file (gamepath + QDir::separator() + dir + "/.cdrom");
        QTextStream stream (&file);

        if (!file.exists())
            continue;
        if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
        {
            qDebug() << tr("DDT: [warning] unable to open file %1: error %2").arg(file.fileName()).arg(file.errorString());
            continue;
        }
        QStringList list;
        while (!stream.atEnd())
        {
            list.append(stream.readLine());
        }
        file.close();
        //работаем со списком
        QDir disc (path);
        QStringList disclist = disc.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
        //посчитаем кол-во эквивалентов
        int i = 0;
        foreach (QString str, list)
        {
            if (disclist.contains(str, Qt::CaseInsensitive))
                i++;
        }
        if (i>4)
        {
            gamefolder = gamepath + QDir::separator() + dir;
            return true;
        }
    }
    return false;
}

void DiscDetector::lauchApp()
{
GameDialog *dlg = new GameDialog(0, gamefolder);
if (dlg->exec() == QDialog::Accepted)
{
    QDir prdir (QDir::homePath() + winepath + QDir::separator() + engine::getPrefixName(gamefolder));
    if (prdir.exists())
    {
        //TODO: создаем диалог редактирования виртуальной Windows
PrefixDialog *pdlg = new PrefixDialog (0, gamefolder);
pdlg->exec();
        delete dlg;
        delete pdlg;
        return;
    }
    //создаем объект движка
    engine *eng = new engine (this);
    eng->setCdMode(true);
    eng->setDiskpath(cdroot);
    eng->lauch(gamefolder, false); //мы не будем показывать сообщение (установить еще) в конце.

}
delete dlg;
}
