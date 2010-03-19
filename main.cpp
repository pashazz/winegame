#include "mainwindow.h"
#include "linux.h"
#include "discdetector.h"
#include "QDir"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//загружаем локализацию
    QTranslator qt;
    qt.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qt);
    QTranslator app;
    app.load(":/ts/l10n/wg_" + QLocale::system().name());
    a.installTranslator(&app);
#ifndef Q_WS_X11
    QMessageBox::critical(0, QObject::tr("This is not for Windows or Mac OSX"), QObject::tr("This program is not suitable to run on Windows or Mac OSX. <br> If you want to run WineGame on Win or Mac. pls remove this warning in main.cpp");
#endif
    //set Linux console encoding to UTF-8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
      QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
//Проверяем, установлен ли Wine
//      QProcess *proc = new QProcess (0);
//      proc->start("which wine");
//      proc->waitForFinished();
//      QString wine = proc->readAll();
//      if (!QFile::exists(wine))
//      {
//          QMessageBox::critical(0,"Error", QObject::tr("Wine installation not found (%1)! Exiting. ").arg(wine));
//          return -4;
//      }
      QDir dir (QDir::homePath() + QDir::separator() + winepath);
if (!dir.exists())
    dir.mkdir(dir.path()); //проверяем главную папочку  WineGame
// детектинг диска
if (a.arguments().length() > 1) {
    DiscDetector det;
    if (det.tryDetect(a.arguments().at(1)))
    {
        det.lauchApp();
        return 0;
    }
}
    MainWindow w;
    w.show();
    return a.exec();

}
