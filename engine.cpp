#include "engine.h"
#include "enginefunc.h"
#include <iostream>
using namespace std;
using namespace QtConcurrent;
engine::engine(QObject *parent) : //сейчас мы не делаем ничего
    QObject(parent)
{

}
void engine::lauch(QString workdir)
{
    QString winebin;
    QFile file (workdir + QDir::separator() + "control");
    //хех, прочитаем файл
    QStringList vars;
    if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        QMessageBox::warning(0, tr("File open error"), tr ("Unable to open file %1. <br>Error: %2").arg(file.fileName()).arg(file.errorString()));
        return;
    }
    QTextStream in (&file);

while (!in.atEnd())
    {
    vars.append(in.readLine());
}
//найдем переменную WINEDISTR для развертывания Wine (если есть)
if (!getVariableValue("WINEDISTR", vars).isEmpty())
{
    QString distr = getVariableValue("WINEDISTR", vars);

//здесь запускаем процесс закачки и распаковки данного дистрибутива Wine
    QString distrname ;

    QFuture<QString> fWine = QtConcurrent::run (downloadWine, distr);

QString destination = QDir::homePath() + winepath + "/wines/" + getVariableValue("PREFIX", vars);
proc->start(tr("tar xvf %1 -C %2").arg(distrname).arg(destination));
proc->waitForFinished();
//теперь устанавливаем переменную winebin
winebin = destination + "/usr/bin/wine"; //дададада!
std::cout << "engine: setting wine binary to " << winebin.toStdString();
//выходим из условия

}
else
{
    //запускаем процесс which wine
    QProcess *proc = new QProcess (this);
    proc->start ("which wine");
    proc->waitForFinished();

     winebin = proc->readAll();
     std::cout << "engine: setting wine binary to " << winebin.toStdString(); // да я знаю, что можно было обойтись одной строкой

}
//если winebin все еще не установлен
if (winebin.isEmpty())
{
    QMessageBox::warning(0, tr("Error"), tr("Wine installation not found."));
    cout << "engine: exiting";
    return;
}
//что делать теперь? Хм
//запускаем preinst с набором variables
QStringList myEnv = QProcess::systemEnvironment();
QString prefixdir = QDir::homePath() + winepath  + QDir::separator() + getVariableValue("PREFIX", vars);
cout << tr("engine: setting Prefix: %1").arg(prefixdir).toStdString();
myEnv.append(tr("FILESDIR=%1").arg(workdir + "/files"));
myEnv.append(tr("RUN=%1").arg(winebin));
myEnv.append(tr("WINEPREFIX=%1").arg(prefixdir)); //для совместимости
myEnv.append(tr("PREFIX=%1").arg(prefixdir));
//запускаем скрипт Preinst
QProcess *proc = new QProcess (this);
proc->setEnvironment(myEnv);
proc->setWorkingDirectory(workdir);
if (QFile::exists(workdir + "/preinst"))
{
proc->start (workdir + "/preinst");
proc->waitForFinished();
cout << tr("engine: preinst script returned this: %1").arg(QString (proc->readAll())).toStdString();
}
//теперь ищем упоминание о запускаемом файле в control (e.g. vars)
QString exefile =   getVariableValue("SETUP", vars);
QString exe; //вот это-сформированный путь к EXE
if (cdMode && QFile::exists(diskpath + QDir::separator() +exefile))
{
   //пробуем подставить exefile к diskpath
   exe = diskpath + QDir::separator() +exefile;
}
else
{
    //спросим-ка у пользователя путь к EXE файлику (QFileDialog)
   QFileDialog *dlg = new QFileDialog (0);
   dlg->setDirectory(diskpath);
   dlg->setFileMode(QFileDialog::ExistingFile);
   dlg->setViewMode(QFileDialog::Detail);
   dlg->setNameFilter(tr("Windows  executables (*.exe)"));
   if (dlg->exec() == QDialog::Accepted)
   {
       exe = dlg->selectedFiles().at(0);
   }
   else
   {
       QMessageBox::warning(0, tr("No EXE file found"), tr("Operation cancelled: No EXE file selected"));
       return;
   }
}
cout << tr("engine: starting Windows program %1").arg(exe).toStdString();
proc->start(winebin, QStringList(exe));
proc->waitForFinished();

//ну а теперь финальная часть, запуск postinst
if (QFile::exists(workdir + "/postinst"))
{
proc->start(workdir + "/postinst");
proc->waitForFinished();
cout << tr("engine: postinst script returned this: %1").arg(QString (proc->readAll())).toStdString();

}
//А здесь должен быть вызов ф-ции по работе с desktop-файлами... эх
              }
QString engine::getVariableValue(QString value, const QStringList &vars)
{
QString ret;
foreach (QString item, vars)
{
    if (item.contains(value))
    {
        ret= item.split("=").at(1); //правую часть после  = выделяем мы
        return ret;
    }

}
return QString();
}

