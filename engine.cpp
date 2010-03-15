#include "engine.h"

engine::engine(QObject *parent) : //сейчас мы не делаем ничего
    QObject(parent)
{
}
void engine::lauch(QString workdir)
{
    QString winebin;
    QFile file (workdir + QDir::separator() + "control");
    //хех, прочитаем файл
    QTextStream in (&file);
    QStringList vars;
    if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        QMessageBox::warning(0, tr("File open error"), tr ("Unable to open file %1. <br>Error: %2").arg(file.fileName()).arg(file.errorString()));
        return;
    }
while (!in.atEnd())
    {
    vars.append(in.readLine());
}
//найдем переменную WINEDISTR для развертывания Wine (если есть)
if (!getVariableValue("WINEDISTR", vars).isEmpty())
{
    QString distr = getVariableValue("WINEDISTR", vars);
//здесь запускаем процесс закачки и распаковки данного дистрибутива Wine
    QProcess *proc =  new QProcess(this);
    lout << "Process starts: " << GET + " " +  distr;
    proc->setWorkingDirectory(TMP);
    proc->start(GET + " " +  distr);
    proc->waitForFinished();
    QString destination  = QDir::homePath() + winepath + "/wines/" + getVariableValue("PREFIX", vars);
    lout << "engine: archive unpack destination is " << destination;
//получаем значение переменной WINEDISTRNAME - имени архива.
QString distrname =     getVariableValue("WINEDISTRNAME", vars);
if (distrname.isEmpty())
{
    lout << "engine: Getting DISTRNAME failed. Set in in control file, exiting engine";
    return;
}
proc->start(tr("tar xvf %1 -C %2").arg(distrname).arg(destination));
proc->waitForFinished();
//теперь устанавливаем переменную winebin
winebin = destination + "/usr/bin/wine"; //дададада!
lout << "engine: setting wine binary to " << winebin;
//выходим из условия

}
else
{
    //запускаем процесс which wine
    QProcess *proc = new QProcess (this);
    proc->start ("which wine");
    proc->waitForFinished();

     winebin = proc->readAll();
     lout << "engine: setting wine binary to " << winebin; // да я знаю, что можно было обойтись одной строкой

}
//если winebin все еще не установлен
if (winebin.isEmpty())
{
    QMessageBox::warning(0, tr("Error"), tr("Wine installation not found."));
    lout << "engine: exiting";
    return;
}
//ждем продолжения.
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
