#ifndef PREFIX_H
#define PREFIX_H

#include <QObject>
#include "linux.h"
#include "engine.h"
class Prefix : public QObject
{
Q_OBJECT
public:
    explicit Prefix(QObject *parent, QString path); //path-путь к директории с описанием
    QString getPrefix ()  {return wineprefix;}
    QProcessEnvironment getEnv () {return env;}
    QString getWine () {return wine;}
    QString getName () {return  _name;}
    QString getNote () {return _note;}
    void runProgram  (QString path);
    void lauchWinetricks (QStringList args);
    void removePrefix();
signals:

public slots:
    void lauch_c();


private:
       QString _name;
    QString _note;
    QString wine; //wine binary
    QString wineprefix; //wineprefix
    QProcessEnvironment env; //process environment with $WINEPREFIX and $WINE
};

#endif // PREFIX_H
