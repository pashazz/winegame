#include "engine.h"
#include "enginefunc.h"
#include <QtDebug>
#include <QPushButton>
using namespace QtConcurrent;
engine::engine(QObject *parent) : //сейчас мы не делаем ничего
    QObject(parent)
{

}
void engine::lauch(QString workdir, bool msg)
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

QString prefixdir = QDir::homePath() + winepath  + QDir::separator() + getVariableValue("PREFIX", vars);
/*
//QDir prdir (prefixdir);
//if (prdir.exists())
//{
//    int ret = QMessageBox::question(0, tr("This application is installed on your computer"),  tr("You did requested to install application <b>%1</b>, but it`s already installed. Do you want to remove application`s directory and reinstall it?").arg(engine::getName(workdir)), QMessageBox::Yes, QMessageBox::No);
//    if (ret == QMessageBox::Yes)
//    {
//       prdir.remove(prdir.path());
//    }
//    else if (ret == QMessageBox::No)
//        return;
//}
  */
//ищем контейнер префикса
if (!getVariableValue("CONTAINER", vars).isEmpty())
{
    QString container = downloadWine(getVariableValue("CONTAINER", vars));
    unpackWine(TMP + QDir::separator() + container, prefixdir);
}

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
if (exefile.isEmpty())
    exefile = "fucking exe"; // ))))
QString exe; //вот это-сформированный путь к EXE
if (cdMode)
{
    if (QFile::exists(diskpath + QDir::separator() + exefile))
    {
   exe = diskpath +exefile;
}
    else
    {
        //получим инфу о запускаемом файле из autorun.inf
        QSettings stg  (diskpath + "/autorun.inf", QSettings::IniFormat, this);
        stg.beginGroup("autorun");
        exe = diskpath + QDir::separator() + stg.value("open").toString();
    }
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
       QProcess p (this);
       p.start(tr("rm -rf %1").arg(prefixdir));
       return;
   }
}
qDebug() << tr("engine: starting Windows program %1 with wine binary %2").arg(exe).arg(winebin);
proc->start(winebin + " \"" + exe  +"\"" );
proc->waitForFinished(-1);

//ну а теперь финальная часть, запуск postinst
if (QFile::exists(workdir + "/postinst"))
{
proc->start("\"" + workdir + "/postinst\"");
proc->waitForFinished(-1);
qDebug() << tr("engine: postinst script returned this: %1").arg(QString (proc->readAll()));

}
/// http://bugs.winehq.org/show_bug.cgi?id=22069 (wine bug - working with desktop files)
if (msg) {
int result = QMessageBox::question(0, tr("Question"), tr("Would you like to install a new game?"), QMessageBox::Yes, QMessageBox::No);
if (result == QMessageBox::No)
    qApp->quit();
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

}
return QString();
}

QString engine::getName(QString path)
{
    //попробуем найти локализованный файл name сначала

    //ищем файл .name в данном path
    QFile file (path + QDir::separator() + ".name." + QLocale::system().name() );
    if (file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString ret = file.readAll();
        file.close();
        return ret;
    }
    else
    {
 //cначала попробуем открыть просто .name
        file.setFileName(path + QDir::separator() + ".name");
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
}
QString engine::getNote(QString path)
{
    /// не забываем, что в note можно вставлять HTML-код!
    QString fileName;
    if (QFile::exists(path + "/.note." + QLocale::system().name())) //читаем локализованное примечание
        fileName =path + "/.note." + QLocale::system().name();
    else if (QFile::exists((path + "/.note")))
        fileName = path + "/.note";
    else
        return QString();
QFile file (fileName);
file.open(QIODevice::Text | QIODevice::ReadOnly);
QString note = file.readAll();
file.close();
return note;

}
void engine::doPkgs(QString pkgs, const QProcessEnvironment &env)
{
    showNotify(tr("Downloading packages..."), tr("Now we will install Microsoft components"));
    QProcess *p = new QProcess(this);
  p->setProcessEnvironment(env);
  QStringList plist = pkgs.split(" ");
  qDebug() << "engine: Installing packages " << plist;
    p->start("/usr/bin/winetricks", plist);
    p->waitForFinished(-1);
 }

QIcon engine::getIcon(QString path)
{
    if (QFile::exists(path + "/icon"))
    {
        QIcon icon (path + "/icon");
        return icon;
}
    else
        return QIcon();
}
QString engine::getPrefixName(QString path)
{
//#warning "code dublicate from getNote!"
    //read control file
    QFile file  (path + QDir::separator() + "control");
    if (file.exists()) {file.open(QIODevice::ReadOnly | QIODevice::Text);}
    else {return QString();}

    QTextStream stream (&file);
    QStringList lines;
    while (!stream.atEnd())
        lines.append(stream.readLine());

    file.close();
    return getVariableValue("PREFIX", lines);
}

QString engine::getWine(QString path)
{
    QFile file  (path + QDir::separator() + "control");
    if (file.exists()) {file.open(QIODevice::ReadOnly | QIODevice::Text);}
    else {return QString();}

    QTextStream stream (&file);
    QStringList lines;
    while (!stream.atEnd())
        lines.append(stream.readLine());

    file.close();
    if (getVariableValue("WINEDISTR", lines).isEmpty())
    {
        //which wine process
              QProcess *proc = new QProcess (0);
              proc->start("which wine");
              proc->waitForFinished();
              QString wine = proc->readAll();
               return wine; //я гарантирую, шо Wine установлен

    }
    else
    {
        QString prefix = getPrefixName(path);
        return QDir::homePath() + winepath + "/wines/" + prefix + "/usr/bin/wine";
    }
}

QString engine::downloadWine(QString url) //TODO: проверка на ошибки.
{
    showNotify(tr("Don`t worry!"), tr("Now WineGame will download some files, that will need for get your applicaton running"));
    QUrl myurl = QUrl(url);
    QFileInfo inf (myurl.path());
    QString wineFileName =TMP + QDir::separator() +  inf.fileName();
    //проверяем, есть ли у нас данный файл
    if (QFile::exists(wineFileName))
        return wineFileName;
  QProgressDialog *progress = new QProgressDialog(0);
     QEventLoop loop;
QNetworkAccessManager *manager = new QNetworkAccessManager (this);
QNetworkRequest req; //request для Url
this->progress = progress;
req.setUrl(QUrl(url));
req.setRawHeader("User-Agent", "Winegame-Browser 0.1");
QNetworkReply *reply = manager->get(req);
connect (reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(setRange(qint64,qint64)));
connect (reply, SIGNAL(finished()), &loop, SLOT(quit()));
progress->setModal(true);
progress->setWindowTitle(tr("Downloading Wine..."));
progress->setLabelText(tr("Downloading %1").arg(url));
QPushButton *but = new QPushButton  (progress);
but->setFlat(true);
but->setDisabled(false);
but->setText(tr("Cancel"));
connect (but, SIGNAL(clicked()), this, SLOT (exitApp()));
progress->setCancelButton(but);
progress->show();
loop.exec();
progress->close();
QByteArray buffer = reply->readAll();
QFile file (wineFileName);
if (file.open(QIODevice::WriteOnly))
{
        file.write(buffer);
        file.close();
                    }
else
    qDebug() << "engine: error open file (WINEDISTR):" << file.errorString();
progress->deleteLater();
return wineFileName;
}
/// это наш старый метод, оставим на всякий случай.
//QString engine::downloadWine(QString url){
//   //сначала отделим имя бинаря
//QUrl myurl (url);
//QFileInfo inf (myurl.path());
//QString wineFileName = inf.fileName();
////проверяем, есть ли у нас данный файл
//if (QFile::exists(TMP + QDir::separator() + wineFileName))
//   return wineFileName;
////наш процесс
////QMessageBox::information(0,tr("WineGame"), tr("Downloading of some required components will be start now. It`s near 20-40 Mb. Please establish your internet connection!"));
//
//   QProcess *proc = new QProcess (this);
//   //показываем нотификацию
//   showNotify(tr("Downloading required components"), tr("It`s near 40 MB. Please establish your Internet connection"));
//   //не меняем переменные окружения (ну кроме PWD :D)
//   proc->setWorkingDirectory(TMP);
//   connect (proc, SIGNAL(readyRead()), this, SLOT(showProgress()));
//proc->start(GET, QStringList(url));
//
//proc->waitForFinished(-1);
//qDebug() << proc->readAll();
//delete proc;
//qDebug() << QObject::tr("engine: wine downloading finished, file %1 in directory %2").arg(wineFileName).arg(TMP);
//return wineFileName;
//}



void engine::showNotify (QString header, QString body) //функция НУ СОВСЕМ не доделана.
{
/// знаю что тупизм,но никто не хочет помогать
    if (QProcessEnvironment::systemEnvironment().contains("KDE_FULL_SESSION")) //пока кеды юзают KDialog
        //вся земля юзает notify-send
        //чезез kdialog:
    {
                             QStringList arguments;
                            arguments << "--passivepopup" <<body;
                            arguments << "--title"<<header;
                            QProcess::startDetached("/usr/bin/kdialog",arguments);
                                      }


        //Через notify-send:
    else
    {
                             QStringList arguments;
                            arguments << header << body;
                            QProcess::startDetached("/usr/bin/notify-send",arguments);
                        }

   }

void engine::setRange(qint64 aval, qint64 total)
{
    int kbAval = aval/1024;
    int kbTotal = total/1024;
    progress->setMaximum(kbTotal);
    progress->setValue(kbAval);
}

void engine::exitApp()
{
    QMessageBox::critical(0, tr("Critical error"), tr("Wine distribution not downloaded, so exit application."));
    qApp->exit(-4);
}

void engine::error(QNetworkReply::NetworkError error)
{
    if  (error != QNetworkReply::NoError)
    {
       return;
   }
    else
    {
        QMessageBox::warning(0, tr("Network error"), tr("Network error: %1. Exiting application"));
        qApp->exit(-6);
    }

}
