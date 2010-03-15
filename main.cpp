#include <QtGui>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifndef Q_WS_X11
    QMessageBox::critical(0, tr("This is not for Windows or Mac OSX"), tr("This program is not suitable to run on Windows or Mac OSX. <br> If you want to run WineGame on Win or Mac. pls remove this warning in main.cpp")
#endif
    //set Linux console encoding to UTF-8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
      QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    MainWindow w;
    w.show();
    return a.exec();
}
