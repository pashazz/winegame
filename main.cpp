#include <QtGui>
#include "mainwindow.h"
#include "linux.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //set Linux console encoding to UTF-8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
      QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
      //also check for argument
      if (a.arguments().empty())
          cdMode = false; //Not to check CD
      else
      {
          QString cdpath = a.arguments().at(0);
          QDir td (QDir::rootPath());
          cdMode = td.exists(cdpath);
          diskpath = cdpath;
      }
    MainWindow w;
    w.show();
    return a.exec();
}
