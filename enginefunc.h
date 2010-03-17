#ifndef ENGINEFUNC_H
#define ENGINEFUNC_H
#include <QtCore>
#include <QDebug>
#include <QMessageBox>

void showNotify (QString header, QString body, int secs)
{
    //Смотрим, DE. Если это KDE, выводим месаги через KDialog, иначе через Libnotify-bin
    QString runline;
    QStringList env = QProcess::systemEnvironment();
    if (env.contains("KDE_FULL_SESSION"))
    {
        runline = QObject::tr("kdialog --passivepopup '%1' %2 --title '%3'").arg(body).arg(secs).arg(header);
    }
    else
    {
        runline = QObject::tr("notify-send '%1' '%2' -t %3").arg(header).arg(body).arg(secs);
    }
    qDebug() << "engine: show PopUp: " << runline;
    QProcess *p = new QProcess (0);
    p->start(runline);
    p->waitForFinished(-1);
   }
// нахуй нотификации. Забью пока.
 QString downloadWine(QString url)
{
    //сначала отделим имя бинаря
 QUrl myurl (url);
QFileInfo inf (myurl.path());
QString wineFileName = inf.fileName();
//проверяем, есть ли у нас данный файл
if (QFile::exists(TMP + QDir::separator() + wineFileName))
    return wineFileName;
//наш процесс
QMessageBox::information(0,QObject::tr("WineGame"), QObject::tr("Downloading of some required components will be start now. It`s near 20-40 Mb. Please establish your internet connection!"));

    QProcess *proc = new QProcess (0);
    //показываем нотификацию
    showNotify(QObject::tr("Downloading required components"), QObject::tr("It`s near 40 MB. Please establish your Internet connection"), 40);
    //не меняем переменные окружения (ну кроме PWD :D)
    proc->setWorkingDirectory(TMP);
 proc->start(GET, QStringList(url));
 proc->waitForFinished(-1);
 delete proc;
 qDebug() << QObject::tr("engine: wine downloading finished, file %1 in directory %2").arg(wineFileName).arg(TMP);
 return wineFileName;
}
 void unpackWine (QString distr, QString destination)
{
     QDir dir (destination);
     if (!dir.exists())
         dir.mkdir(dir.path());
     else
     {
         int result = QMessageBox::question(0, QObject::tr("Question"), QObject::tr("I see, that Wine did installed previously for this application (Directory %1 exists).<br>Would you like to install wine now?"), QMessageBox::Yes, QMessageBox::No);
         if (result == QMessageBox::No)
             return;

     }
 QProcess *proc = new QProcess (0); //не забываем удалять
 proc->setWorkingDirectory(destination);
 QString unpackLine =  QObject::tr ("tar xvpf %1/%2 -C %3").arg(TMP).arg(distr).arg(destination);
 proc->start(unpackLine);
  proc->waitForFinished(-1);
  qDebug() << "DEBUG: Unpack commandline is " << unpackLine;
 qDebug() << QObject::tr("engine: Wine distribution %1 unpacked to %2").arg(distr).arg(destination);
}


#endif // ENGINEFUNC_H
