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
DiscDetector::DiscDetector(corelib *lib) :
	QObject(0), core(lib)
{
}
bool DiscDetector::tryDetect(QString path)
{
    /// path - путь к корню CD/DVD
    cdroot = path;
    // для начала просмотрим все папочки в core->packageDir()
    QDir dir (core->packageDir());
    foreach (QString dirName, dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
    {
        QDir myDir (dir.path() + QDir::separator() + dirName + "/cdrom.d");
        foreach (QFileInfo info, myDir.entryInfoList(QDir::Files | QDir::Readable))
        {
        //читаем файл .cdrom
        QFile file (info.absoluteFilePath());
        QTextStream stream (&file);
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
            {
                i++;
            }
                }
        if (i == disclist.count())
        {
            gamefolder = core->packageDir() + QDir::separator() + dirName;
            //Смотрим, мб. нам попался автопакет. Если так, ставим автоматический режим в детекторе дисков.
            return true;
        }
    }

}
    //if (!dir.exists(cdroot)) /// пока я не знаю, что делать с этим
        return false;
    }

void DiscDetector::lauchApp()
{

GameDialog *dlg = new GameDialog(0, gamefolder);
if (dlg->exec() == QDialog::Accepted)
{
	Prefix *prefix = new Prefix (this, gamefolder, core);
    QDir prdir (prefix->prefixPath());

    if (prdir.exists())
    {
        /*!
          Хардкорный хак для FIFA 2010,  там некорректный авторан, но он блять прописан!! Я негодую
          */
            if (prefix->prefixName() == "fifa10") {goto dialog;}
        //Может быть, нам стоит запустить AutoRun?
		if (!core->autorun(cdroot).isEmpty())
		{
			QSettings stg (core->autorun(cdroot), QSettings::IniFormat, this);
            stg.beginGroup("autorun");
            QString myExe = cdroot + QDir::separator() + stg.value("open").toString();
             QString myWine = prefix->wine();
            qDebug() << "DDT: Starting Autorun: " << myWine << myExe;
            QProcess p;
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("WINEPREFIX", prefix->prefixPath());
            env.insert("WINEDEBUG", "-all");
            p.setProcessEnvironment(env);
			p.setWorkingDirectory(prefix->getExeWorkingDirectory(myExe));
            p.start(myWine, QStringList (myExe));
            p.waitForFinished(-1);

          }
        else {
        //TODO: создаем диалог редактирования виртуальной Windows
            dialog:
PrefixDialog *pdlg = new PrefixDialog (0, gamefolder, core);
pdlg->exec();
        delete pdlg;

    }
    }
    else{
    //создаем объект движка
		//New prefix obj init
}
}
dlg->deleteLater();
}
