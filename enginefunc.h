#ifndef ENGINEFUNC_H
#define ENGINEFUNC_H
#include <QtCore>
#include <QDebug>
#include <QMessageBox>
void showNotify (QString header, QString body, int secs) //функция НУ СОВСЕМ не доделана.
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

 void unpackWine (QString distr, QString destination)
{
     QDir dir (destination);
     if (!dir.exists())
         dir.mkdir(dir.path());
     else
     {
         int result = QMessageBox::question(0, QObject::tr("Question"), QObject::tr("I see, that Wine did installed previously for this application (Directory %1 exists).<br>Would you like to install wine now?").arg(destination), QMessageBox::Yes, QMessageBox::No);
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
