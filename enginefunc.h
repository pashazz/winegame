#ifndef ENGINEFUNC_H
#define ENGINEFUNC_H
#include <QtCore>
#include <QDebug>
#include <QMessageBox>


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
 QString unpackLine =  QObject::tr ("tar xvpf %1 -C %2").arg(distr).arg(destination);
 proc->start(unpackLine);
  proc->waitForFinished(-1);
  qDebug() << "DEBUG: Unpack commandline is " << unpackLine;
 qDebug() << QObject::tr("engine: Wine distribution %1 unpacked to %2").arg(distr).arg(destination);
}


#endif // ENGINEFUNC_H
