#include "engine.h"
#include "linux.h"
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
//здесь запускаем процесс закачки и распаковки данного дистрибутива Wine

}
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
    return "";
}
}
