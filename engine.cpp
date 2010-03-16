#include "engine.h"
#include "enginefunc.h"
#include <QtDebug>
using namespace QtConcurrent;
engine::engine(QObject *parent) : //сейчас мы не делаем ничего
    QObject(parent)
{

}
void engine::lauch(QString workdir)
{
    QLabel *myLabel = new QLabel (0);
    myLabel->setWindowModality(Qt::ApplicationModal);
    myLabel->setCursor(Qt::BusyCursor);
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
QString note = getVariableValue("NOTE", vars);
if (note.isEmpty())
    note = tr("This application has no notes");
myLabel->setText (tr("Please wait while WineGame is doing operations.<br>Read this note:<br>%1").arg(note));
//найдем переменную WINEDISTR для развертывания Wine (если есть)
if (!getVariableValue("WINEDISTR", vars).isEmpty())
{
    QString distr = getVariableValue("WINEDISTR", vars);

//здесь запускаем процесс закачки и распаковки данного дистрибутива Wine

  //  QFuture<QString> fWine = run (downloadWine, distr);
//QString distrname = fWine.result();
    myLabel->show();
    QString distrname =     downloadWine(distr);
   myLabel->hide();
QString destination = QDir::homePath() + winepath + "/wines/" + getVariableValue("PREFIX", vars);
QDir dir (QDir::homePath() + winepath + "/wines");
if (!dir.exists())
    dir.mkdir(dir.path());
if (!dir.exists(getVariableValue("PREFIX", vars)))
    dir.mkdir(getVariableValue("PREFIX", vars));
//а может быть Wine уже распакован, м?
//теперь устанавливаем переменную winebin
winebin = destination + "/usr/bin/wine"; //дададада!
qDebug() << "engine: setting wine binary to " << winebin;
if (!QFile::exists(winebin))
{
    myLabel->show();
    unpackWine(distrname, destination);
    myLabel->hide();
}
//выходим из условия

}
else
{
    //запускаем процесс which wine
    QProcess *proc = new QProcess (this);
    proc->start ("which wine");
    proc->waitForFinished();

     winebin = proc->readAll();
     qDebug() << "engine: setting wine binary to " << winebin; // да я знаю, что можно было обойтись одной строкой

}
//если winebin все еще не установлен
if (winebin.isEmpty())
{
    QMessageBox::warning(0, tr("Error"), tr("Wine installation not found."));
    qDebug() << "engine: exiting";
    return;
}
//что делать теперь? Хм
//запускаем preinst с набором variables
QStringList myEnv = QProcess::systemEnvironment();
QString prefixdir = QDir::homePath() + winepath  + QDir::separator() + getVariableValue("PREFIX", vars);
qDebug() << tr("engine: setting Prefix: %1").arg(prefixdir);
myEnv.append(tr("FILESDIR=%1").arg(workdir + "/files"));
myEnv.append(tr("RUN=%1").arg(winebin));
myEnv.append(tr("WINEPREFIX=%1").arg(prefixdir)); //для совместимости
myEnv.append(tr("PREFIX=%1").arg(prefixdir));
//запускаем скрипт Preinst
QProcess *proc = new QProcess (this);
proc->setEnvironment(myEnv);
proc->setWorkingDirectory(workdir);
qDebug() << "DEBUG: Workdir is" << workdir;
if (QFile::exists(workdir + "/preinst"))
{
    qDebug() << "DEBUG: Preinst commandline is " << workdir + "/preinst";
    proc->start ("\"" +workdir + "/preinst\"");
proc->waitForFinished(-1);
qDebug() << tr("engine: preinst script returned this: %1").arg(QString (proc->readAll()));
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
qDebug() << tr("engine: starting Windows program %1").arg(exe);
proc->start(winebin, QStringList(exe));
proc->waitForFinished(-1);

//ну а теперь финальная часть, запуск postinst
if (QFile::exists(workdir + "/postinst"))
{
    qDebug() << "DEBUG: Postinst commandline is" << workdir +  "/postinst";
proc->start("\"" + workdir + "/postinst\"");
proc->waitForFinished(-1);
qDebug() << tr("engine: postinst script returned this: %1").arg(QString (proc->readAll()));

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

