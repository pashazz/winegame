#include "mainwindow.h"
#include "linux.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifndef Q_WS_X11
    QMessageBox::critical(0, tr("This is not for Windows or Mac OSX"), tr("This program is not suitable to run on Windows or Mac OSX. <br> If you want to run WineGame on Win or Mac. pls remove this warning in main.cpp")
#endif
    //set Linux console encoding to UTF-8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
      QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
//Проверяем, установлен ли Wine
      QProcess *proc = new QProcess (0);
      proc->start("which wine");
      proc->waitForFinished();
      QString wine = proc->readAll();
//      if (!QFile::exists(wine))
//      {
//          QMessageBox::critical(0,"Error", QObject::tr("Wine installation not found (%1)! Exiting. ").arg(wine));
//          return -4;
//      }

    MainWindow w;
    w.show();
    return a.exec();
}
