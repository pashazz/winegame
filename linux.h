#ifndef LINUX_H
#define LINUX_H

#include <QString>
#include <QTextStream>
//Linux text console output
QTextStream lout (stdout);

//CDROM path
QString diskpath;

//check CD, or no
bool cdMode;

//важные константы
const QString gamepath = "/usr/share/winegame";
const QString winepath = "/Windows";
const QString GET = "wget"; //you can replace with curl,  etc
const QString TMP = "/tmp";
#endif // LINUX_H
