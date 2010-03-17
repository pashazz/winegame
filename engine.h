#ifndef ENGINE_H
#define ENGINE_H

#include <QMessageBox> //для показа сообщений об ошибках
#include <QString>
#include <QLabel>
#include <QTextStream>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QtCore>
#include <QUrl>
#include <QProgressDialog>
#include "linux.h"
class engine : public QObject
{
Q_OBJECT
public:
    explicit engine(QObject *parent = 0);
 void lauch (QString workdir);
 void setCdMode (bool mode){cdMode=mode;}
 void setDiskpath (QString path){diskpath = path;}
 static QString getName (QString path);
 static QString getNote(QString path);
signals:

public slots:

private:
// QString getExe ();
static  QString getVariableValue (QString value, const QStringList &vars);
bool cdMode;
QString diskpath;
void getPrefix (QString url);
void doPkgs (QString pkgs, const QProcessEnvironment &env);
//Функции собственно движка (исп. с QtConcurrent) объявлены в enginefunc.h

};

#endif // ENGINE_H
