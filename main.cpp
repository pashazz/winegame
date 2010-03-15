#include "mainwindow.h"
#include "linux.h"
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
      if ((!QFile::exists(wine)) || wine.isEmpty())
      {
          QMessageBox::critical(0,"Error", "Wine installation not found! Exiting.");
          return -4;
      }

      //also check for argument
      if (qApp->arguments().empty())
          cdMode = false; //Not to check CD
      else
      {
          QString cdpath = qApp->arguments().at(0);
          QDir td (QDir::rootPath());
          cdMode = td.exists(cdpath);
          diskpath = cdpath;
      }
    MainWindow w;
    w.show();
    return a.exec();
}
