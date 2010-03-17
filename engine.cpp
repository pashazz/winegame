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
showNotify(tr("Please read this note about your application"), note, 20);
//найдем переменную WINEDISTR для развертывания Wine (если есть)
if (!getVariableValue("WINEDISTR", vars).isEmpty())
{
    QString distr = getVariableValue("WINEDISTR", vars);

//здесь запускаем процесс закачки и распаковки данного дистрибутива Wine

  //  QFuture<QString> fWine = run (downloadWine, distr);
//QString distrname = fWine.result();
    QString distrname =     downloadWine(distr);

QString destination = QDir::homePath() + winepath + "/wines/" + getVariableValue("PREFIX", vars);
QDir dir (QDir::homePath() + winepath + "/wines");
if (!dir.exists())
    dir.mkdir(dir.path());
//а может быть Wine уже распакован, м?
//теперь устанавливаем переменную winebin
winebin = destination + "/usr/bin/wine"; //дададада!
qDebug() << "engine: setting wine binary to " << winebin;
if (!QFile::exists(winebin))
{
    unpackWine(distrname, destination);
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
QProcessEnvironment myEnv = QProcessEnvironment::systemEnvironment();
QString prefixdir = QDir::homePath() + winepath  + QDir::separator() + getVariableValue("PREFIX", vars);
qDebug() << tr("engine: setting Prefix: %1").arg(prefixdir);
myEnv.insert("FILESDIR", workdir + "/files");
myEnv.insert("WINEPREFIX", prefixdir);
myEnv.insert("WINE", winebin);
myEnv.insert("CDROOT", this->diskpath);
/// Устанавливаем компоненты Microsoft здесь. Вы можете скачать их в ~/.winetrickscache, чтобы программа не загружала их сама
if (!getVariableValue("PKGS", vars).isEmpty())
    doPkgs(getVariableValue("PKGS", vars), myEnv);
//запускаем скрипт Preinst
QProcess *proc = new QProcess (this);
proc->setProcessEnvironment(myEnv);
proc->setWorkingDirectory(workdir);
if (QFile::exists(workdir + "/preinst"))
{
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
   if (cdMode)
    dlg->setDirectory(diskpath);
   dlg->setFileMode(QFileDialog::ExistingFile);
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
qDebug() << tr("engine: starting Windows program %1 with wine binary %2").arg(exe).arg(winebin);
proc->start(winebin, QStringList(exe));
proc->waitForFinished(-1);

//ну а теперь финальная часть, запуск postinst
if (QFile::exists(workdir + "/postinst"))
{
proc->start("\"" + workdir + "/postinst\"");
proc->waitForFinished(-1);
qDebug() << tr("engine: postinst script returned this: %1").arg(QString (proc->readAll()));

}
//А здесь должен быть вызов ф-ции по работе с desktop-файлами... эх

int result = QMessageBox::question(0, tr("Question"), tr("Would you like to install a new game?"), QMessageBox::Yes, QMessageBox::No);
if (result == QMessageBox::No)
    qApp->quit();
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

QString engine::getName(QString path)
{
    //ищем файл .name в данном path
    QFile file (path + QDir::separator() + ".name");
    if (file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString ret = file.readAll();
        file.close();
        return ret;
    }
    else
    {
       QDir dir (path);
       return dir.dirName();
    }
}
QString engine::getNote(QString path)
{
    //read control file
    QFile file  (path + QDir::separator() + "control");
    if (file.exists()) {file.open(QIODevice::ReadOnly | QIODevice::Text);}
    else {return QString();}

    QTextStream stream (&file);
    QStringList lines;
    while (!stream.atEnd())
        lines.append(stream.readLine());

    file.close();
    return getVariableValue("NOTE", lines);
}
void engine::doPkgs(QString pkgs, const QProcessEnvironment &env)
{
    QMessageBox::warning(qobject_cast<QWidget*>(parent()), tr("Warning"), tr("Now WineGame will download required packages (from Microsoft site). Please establish your Internets"));
    QProcess *p = new QProcess(this);
  p->setProcessEnvironment(env);
    p->start("/usr/bin/winetricks", QStringList (pkgs));
    p->waitForFinished(-1);
 }
