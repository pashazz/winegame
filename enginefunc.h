#ifndef ENGINEFUNC_H
#define ENGINEFUNC_H
#include <QtCore>
#include <QDebug>


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
    QProcess *proc = new QProcess (0);
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
 QProcess *proc = new QProcess (0); //не забываем удалять
 proc->setWorkingDirectory(destination);
 proc->start("tar xvf "+ TMP  + QDir::separator() + distr + " -C " + destination );
 proc->waitForFinished(-1);
 qDebug() << QObject::tr("engine: Wine distribution %1 unpacked to %2").arg(distr).arg(destination);
}


#endif // ENGINEFUNC_H
