#ifndef ENGINE_H
#define ENGINE_H

#include <QMessageBox> //для показа сообщений об ошибках
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QDir>
#include "linux.h"
class engine : public QObject
{
Q_OBJECT
public:
    explicit engine(QObject *parent = 0);
 void lauch (QString workdir);
signals:

public slots:

private:
 QString getExe ();
 QString getVariableValue (QString value, const QStringList &vars);

};

#endif // ENGINE_H
