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


#include "isomaster.h"
#include "discdetector.h"
IsoMaster::IsoMaster(QObject *parent, QString imageFile) : //просто конструктор
    QObject(parent)
{

    mountpoint = QDir::homePath() + MOUNT_DIR;
    QDir dir (mountpoint);
    if (!dir.exists())
        dir.mkdir(dir.path());
    if (detectSudo().isEmpty())
    {
    mount = QString("fuseiso \"%1\" \"%2\"").arg(imageFile).arg(mountpoint);
    umount = QString("fusermount -u \"%1\"").arg(mountpoint);
}
    else
        mount = QString("%1 mount -o loop \"%2\" \"%3\"").arg(detectSudo()).arg(imageFile).arg(mountpoint);
    umount = QString ("%1 umount \"%2\"").arg(imageFile).arg(mountpoint);

}
void IsoMaster::lauchApp()
{
    QProcess p (this);
    p.start(mount);
    p.waitForFinished(-1);
    DiscDetector det (this);
    if (det.tryDetect(mountpoint))
    {
        det.lauchApp();
      p.start(umount);
   p.waitForFinished(-1);
        return;
    }
    else
    {
        QMessageBox::critical(0, tr("Warning"), tr("I can not get Windows application from this image"));
        //TODO: сделать небольшой диалог, чтобы юзер выбирал из списка пресетов
    }
    p.start(umount);
 p.waitForFinished(-1);
}

QString IsoMaster::detectSudo()
{
    if (QFile::exists(corelib::whichBin("kdesu")))
        return "kdesu";
    else if (QFile::exists(corelib::whichBin("gksu")))
        return "gksu";
    else
        return "";
}
