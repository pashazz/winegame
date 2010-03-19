#include "prefix.h"
using namespace QtConcurrent;
void rp(QString path, QProcessEnvironment env)
{
    QProcess *p = new QProcess (0);
    p->setProcessEnvironment(env);
    p->start(path);
    p->waitForFinished(-1);
    delete p;
}
void Prefix::removePrefix()
{
    rp("rm -rf " + this->wineprefix, QProcessEnvironment::systemEnvironment());
}

Prefix::Prefix(QObject *parent, QString path) :
    QObject(parent)
{
    //прежде всего, прочтем файл Control (с помощью ф-ций  движка) и выведем значения  переменных
    _name = engine::getName(path);
    _note = engine::getName(path);
    wine = engine::getWine(path);
    qDebug() << tr("engine: setting wine binary to %1").arg(wine);
    wineprefix = QDir::homePath() + winepath + QDir::separator() + engine::getPrefixName(path);
    env = QProcessEnvironment::systemEnvironment();
    env.insert("WINE", wine);
    env.insert("WINEPREFIX", wineprefix);


}
void Prefix::runProgram(QString exepath)
{
QFuture <void> fProc = run(rp, tr("%1  \"%2\"").arg(wine).arg(exepath),  this->env);
//fProc.waitForFinished();
}
void Prefix::lauch_c()
{
  QFuture <void> fProc = run(rp, "xdg-open " + wineprefix + "/drive_c", env);
}
void Prefix::lauchWinetricks(QStringList args)
{
    /// так как производится установка компонентов, чтобы юзер ничего не натворил, запускаем winetricks в том же потоке.
    QProcess *p = new QProcess (this);
    p->setProcessEnvironment(env);
    qDebug() << tr("engine: [prefix]: starting winetricks");
    p->start("/usr/bin/winetricks", args);
     p->waitForFinished(-1);
}

