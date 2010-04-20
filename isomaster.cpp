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

    mount = QString("fuseiso \"%1\" \"%2\"").arg(imageFile).arg(mountpoint);
    umount = QString("fusermount -u \"%1\"").arg(mountpoint);

}
bool IsoMaster::lauchApp()
{
    QProcess p (this);
    qDebug() << mount;
    p.start(mount);
    p.waitForFinished(-1);
    if (p.exitCode() != 0)
        return false;
    DiscDetector det (this);
    if (det.tryDetect(mountpoint))
    {
        det.lauchApp();
      p.start(umount);
   p.waitForFinished(-1);
   return (p.exitCode() == 0);
    }
    else
    {
        QMessageBox::critical(0, tr("Warning"), tr("I can not get Windows application from this image"));
        //TODO: сделать небольшой диалог, чтобы юзер выбирал из списка пресетов
    }
    p.start(umount);
 p.waitForFinished(-1);
 return true;
}
