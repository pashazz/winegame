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
const gamepath = "/usr/share/winegame";
#endif // LINUX_H
