#ifndef ENGINEFUNC_H
#define ENGINEFUNC_H
#include <QtCore>
#include <iostream>
using namespace std;

extern QString downloadWine(QString url)
{
    //сначала отделим имя бинаря
 QUrl myurl (url);
QFileInfo inf (myurl.path());
QString wineFileName = inf.completeBaseName();
//наш процесс
    QProcess *proc = new QProcess (0);
    //не меняем переменные окружения (ну кроме PWD :D)
    proc->setWorkingDirectory(TMP);
 proc->start(GET, QStringList(url));
 proc->waitForFinished(-1);
 delete proc;
 cout << QObject::tr("engine: wine downloading finished, file %1 in directory %2").arg(wineFileName).arg(TMP).toStdString();
 return wineFileName;
}
#endif // ENGINEFUNC_H
