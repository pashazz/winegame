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
IsoMaster::IsoMaster(corelib *lib, QString imageFile) : //просто конструктор
	QObject(0), core (lib)
{

	QDir dir (core->mountDir());
    if (!dir.exists())
        dir.mkdir(dir.path());
	if (core->getSudoProg().isEmpty() || core->forceFuseiso()) {
	mount = QString("fuseiso \"%1\" \"%2\"").arg(imageFile).arg(core->mountDir());
	umount = QString("fusermount -u \"%1\"").arg(core->mountDir());
}
	else
	{
		QString sudo = core->getSudoProg();
		QString mountString = QString ("mount -o loop \"%1\" \"%2\"").arg(imageFile).arg(core->mountDir());
		QString umountString = QString ("umount \"%1\"").arg(core->mountDir());
		if (sudo == "kdesu" && QProcessEnvironment::systemEnvironment().contains("KDE_FULL_SESSION"))
		{
			mount = QString ("kdesu -i %1 \"%2\"").arg(qApp->applicationName().toLower()).arg(mountString);
			umount = QString ("kdesu -i %1 \"%2\"").arg(qApp->applicationName().toLower()).arg(umountString);
		}
		else if (sudo == "gksu")
		{
		 QString mountMsg = tr("Enter password to mount ISO image");
		 QString umountMsg = tr("Enter password to unmount ISO image");
		 mount = QString ("gksu -m \"%1\" -D %2 \"%3\"").arg(mountMsg).arg(qApp->applicationName()).arg(mountString);
		 umount = QString ("gksu -m \"%1\" -D %2 \"%3\"").arg(umountMsg).arg(qApp->applicationName()).arg(umountString);
			  }
		else if (sudo == "xdg-su")
		{
			mount = QString ("xdg-su -c \"%1\"").arg(mountString);
			umount = QString ("xdg-su -c \"%1\"").arg(umountString);
		}
		else
		{
			//force fuseiso
			mount = QString("fuseiso \"%1\" \"%2\"").arg(imageFile).arg(core->mountDir());
			umount = QString("fusermount -u \"%1\"").arg(core->mountDir());
		}
	}

}
bool IsoMaster::lauchApp()
{
    QProcess p (this);
    qDebug() << mount;
    p.start(mount);
    p.waitForFinished(-1);
    if (p.exitCode() != 0)
        return false;
	DiscDetector det (core);
	if (det.tryDetect(core->mountDir()))
    {
        det.lauchApp();
      p.start(umount);
   p.waitForFinished(-1);
   return (p.exitCode() == 0);
    }
    else
    {
		DiskDialog *dlg = new DiskDialog (0, core, core->mountDir());
		//run diskdialog and exit
		dlg->exec();
		  }
    p.start(umount);
 p.waitForFinished(-1);
 return true;
}
