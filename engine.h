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
#include <QIcon>
#include <QtNetwork>
#include "linux.h"
class engine : public QObject
{
Q_OBJECT
public:
    explicit engine(QObject *parent = 0);
 void lauch (QString workdir, bool msg = true);
 void setCdMode (bool mode){cdMode=mode;}
 void setDiskpath (QString path){diskpath = path;}
 static QString getName (QString path);
 static QString getNote(QString path);
 static QIcon getIcon (QString path);
 static QString getPrefixName (QString path);
 static QString getWine (QString path);
 static void showNotify (QString, QString);
 signals:

private slots:
 void error (QNetworkReply::NetworkError);
 void setRange (qint64, qint64); //заглушка для QProgressDialog
 void exitApp ();
private:
static  QString getVariableValue (QString value, const QStringList &vars);
bool cdMode;
QString diskpath;
QProgressDialog *progress;
void doPkgs (QString pkgs, const QProcessEnvironment &env);
//Функции собственно движка (исп. с QtConcurrent) объявлены в enginefunc.h
 QString downloadWine(QString url);
};

#endif // ENGINE_H
