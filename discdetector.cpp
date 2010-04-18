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
        QString filename = gamepath + QDir::separator() + dir + "/.cdrom." + QLocale::system().name();
        if (!QFile::exists(filename))
            filename = gamepath + QDir::separator() + dir + "/.cdrom"; //ну ведь в разных странах разные релизы игр, правда? Вот и мы можем сделать разные cdrom
        QFile file (filename);
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
        if (i>=disclist.count() -2) //для всяких кряков, чтобы их не включать в .cdrom, но диски определялис
        {
            gamefolder = gamepath + QDir::separator() + dir;
            //Смотрим, мб. нам попался автопакет. Если так, ставим автоматический режим в детекторе дисков.
            return true;
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
    QDir prdir (QDir::homePath() + winepath + QDir::separator() + engine::getPrefixName(gamefolder));
/*    if (isAuto)
    {
        //Передаем управление системе автопакетов
        qDebug() << "loading autopackage";
      AutoPackage apkg;
      apkg.setWorkingDirectory(gamefolder);
      apkg.load();
        return;
    }
    qDebug() << "not an autopackage";
    */
    if (prdir.exists())
    {
        //Может быть, нам стоит запустить AutoRun?
        if (QFile::exists(cdroot + "/autorun.inf"))
        {
            QSettings stg (cdroot + "/autorun.inf", QSettings::IniFormat, this);
            stg.beginGroup("autorun");
            QString myExe = stg.value("open").toString();
             QString myWine = engine::getWine(gamefolder);
            qDebug() << "DDT: Starting Autorun: " << myWine << myExe;
            QProcess p;
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("WINEPREFIX", engine::prefixPath(gamefolder));
            env.insert("WINEDEBUG", "-all");
            p.setProcessEnvironment(env);
            p.setWorkingDirectory(engine::getExeWorkingDirectory(myExe));
            p.start(myWine, QStringList (myExe));
            p.waitForFinished(-1);

          }
        else {
        //TODO: создаем диалог редактирования виртуальной Windows
PrefixDialog *pdlg = new PrefixDialog (0, gamefolder);
pdlg->exec();
        delete pdlg;

    }
    }
    else{
    //создаем объект движка
    engine *eng = new engine (this);
    eng->setCdMode(true);
    eng->setDiskpath(cdroot);
    eng->lauch(gamefolder, false); //мы не будем показывать сообщение (установить еще) в конце.
}
}
dlg->deleteLater();
}
