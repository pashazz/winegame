/*
    Winegame - small utility to prepare Wine and install win32 apps in it.
    Copyright (C) 2010 Pavel Zinin <pzinin@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "engine.h"
#include "enginefunc.h"
#include <QtDebug>
#include <QPushButton>
#include <QInputDialog>
using namespace QtConcurrent;
engine::engine(QObject *parent) : //сейчас мы не делаем ничего
    QObject(parent)
{

}
void engine::lauch(QString workdir, bool msg)
{
    QString winebin;
    QSettings s (workdir + CTRL,  QSettings::IniFormat, this);
 prefix = QDir::homePath() + winepath  + QDir::separator() + s.value("application/prefix").toString();
//ищем контейнер префикса
if (!s.value("application/container").toString().isEmpty())
{
    QString container = downloadWine(s.value("application/container").toString());
    unpackWine(TMP + QDir::separator() + container, prefix);
}

//найдем переменную WINEDISTR для развертывания Wine (если есть)
if (!s.value("wine/distr").toString().isEmpty())
{
    QString distr = s.value("wine/distr").toString();
//здесь запускаем процесс закачки и распаковки данного дистрибутива Wine
    QString destination = QDir::homePath() + winepath + "/wines/" + getPrefixName(workdir);
    QDir dir (QDir::homePath() + winepath + "/wines");
    if (!dir.exists())
        dir.mkdir(dir.path());
//а может быть Wine уже распакован, м
//теперь устанавливаем переменную winebin
winebin = destination + "/usr/bin/wine"; //дададада!
qDebug() << "engine: setting wine binary to " << winebin;
if (!QFile::exists(winebin))
{
    QString distrname =     downloadWine(distr);
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
wineBinary = winebin; //для doDesktop;
name = getName(workdir);
note = getNote(workdir);
program = getStandardExe(workdir);
//что делать теперь? Хм
//запускаем preinst с набором variables
QProcessEnvironment myEnv = QProcessEnvironment::systemEnvironment();
qDebug() << tr("engine: setting Prefix: %1").arg(prefix);
myEnv.insert("FILESDIR", workdir + "/files");
myEnv.insert("WINEPREFIX", prefix);
myEnv.insert("WINE", winebin);
myEnv.insert("CDROOT", this->diskpath);
myEnv.insert("WINEDEBUG", "-all");

/// Устанавливаем компоненты Microsoft здесь. Вы можете скачать их в ~/.winetrickscache, чтобы программа не загружала их сама
if (!s.value("wine/components").toString().isEmpty())
    doPkgs(s.value("wine/components").toString(),myEnv);
//запускаем скрипт Preinst
QProcess *proc = new QProcess (this);
proc->setProcessEnvironment(myEnv);
proc->setWorkingDirectory(workdir);
if (QFile::exists(workdir + "/preinst"))
{
    proc->start ("\"" +workdir + "/preinst\"");
proc->waitForFinished(-1);
}
//теперь ищем упоминание о запускаемом файле в control
QString exefile = s.value("application/setup").toString();
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
       p.start(tr("rm -rf %1").arg(prefix));
       p.waitForFinished(-1);
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
makefix(prefix);

}
/// http://bugs.winehq.org/show_bug.cgi?id=22069 (wine bug - working with desktop files)
if (s.value("wine/memory", false).toBool())
{
    //получаем видеопамять.
    QSettings stg (QDir::homePath() + config, QSettings::IniFormat, this);
   //для совместимости, берем значение в int (а вдруг там нам не int подсунули, или <= 0
    QString mem = stg.value("VideoMemory", -1).toString();
    if (mem.toInt() > 0)
    {
        qDebug() << "engine: [memory] setting video memory to " << mem;
        setMemory(mem);
    }
}

program = s.value("application/exepath").toString();
if (!program.isEmpty())
{
    //получаем иконку
    QString icon;
    if (cdMode)
    {
    QSettings stg (diskpath+ "/autorun.inf", QSettings::IniFormat, this);
    stg.beginGroup("autorun");
     icon = diskpath + QDir::separator() + stg.value("Icon").toString();
    qDebug() << "engine: ico file detected" << icon;
}
    else
    {
        if (QFile::exists(workdir+"/icon"))
        icon = workdir+"/icon";
    }
    this->iconPath = icon;
    doDesktop(s.value("application/prefix").toString());
}
if (msg) {
int result = QMessageBox::question(0, tr("Question"), tr("Would you like to install a new game?"), QMessageBox::Yes, QMessageBox::No);
if (result == QMessageBox::No)
    qApp->quit();
}
              }

void engine::lauchPreset(QString preset, bool msg)
{
    QString workdir = gamepath + "/presets/" + preset;
 //Get PREFIX variable
    dialog:
    QString myPrefix =QInputDialog::getText(0, tr("Give a name for your application"), tr ("Give a short latin name for your application. <br> It will be installed in %1/Windows/APPLICATION NAME/drive_c").arg(QDir::homePath()));
    if (myPrefix.isEmpty())
    {
        QMessageBox::critical(0, tr("No name specified"), tr ("Abort. Reason: No application name specified"));
        return;
    }
    //проверим, есть ли такой префикс уже.
    QDir dir;
    dir.setPath(QDir::homePath() + winepath + QDir::separator() + myPrefix);
    if (dir.exists()){

        QMessageBox::warning(0, tr("Application with this name is already installed."), tr(" To force installation process, remove directory %1.").arg(QDir::homePath() + winepath +QDir::separator() + myPrefix));
        //я знаю, это плохо
        goto dialog;
    }
    else if (dir.exists(gamepath + QDir::separator() + myPrefix))
    {
        QMessageBox::warning(0, tr("Warning"), tr("I know about this game. <br>You can install it by clicking  it`s entry in 'Applications' list"));
        return;
    }
    QSettings s(workdir + CTRL, QSettings::IniFormat, this);
//which wine
    {
    QProcess *proc = new QProcess (this);
    proc->start ("which wine");
    proc->waitForFinished();
     wineBinary = proc->readAll();
     qDebug() << "engine: setting wine binary to " << wineBinary; // да я знаю, что можно было обойтись одной строкой
 }
 //ставим компоненты
     QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
     //Для вычисления префикса нам не подходят стандартные методы Engine. Поэтому, воспользуемся своим
     prefix = QDir::homePath() + winepath + QDir::separator() + myPrefix;
     qDebug() << "engine: setting Prefix" <<prefix;
     env.insert("WINEDEBUG", "-all");
     env.insert("WINEPREFIX", prefix);
     /*!
       Напоминаю, обязательное условие пресета - присутствие components в секции wine! ОБЯЗАТЕЛЬНОЕ!
       */
     doPkgs(s.value("wine/components").toString(), env);
   //теперь проверяем memory
     if (s.value("wine/memory").toBool())
     {
         QSettings stg (QDir::homePath() + config, QSettings::IniFormat, this);
        //для совместимости, берем значение в int (а вдруг там нам не int подсунули, или <= 0
         QString mem = stg.value("VideoMemory", -1).toString();
         if (mem.toInt() > 0)
         {
             qDebug() << "engine: [memory] setting video memory to " << mem;
             setMemory(mem);
         }
     }
     //запускаем EXE
     //теперь ищем упоминание о запускаемом файле в control
     QString exefile = s.value("application/setup").toString();
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
            p.start(tr("rm -rf %1").arg(prefix));
            p.waitForFinished(-1);
            return;
        }
     }
     qDebug() << tr("engine: starting Windows program %1 with wine binary %2").arg(exe).arg(wineBinary);
     QProcess *proc = new QProcess (this);
     proc->setProcessEnvironment(env);;
     proc->start(wineBinary+ " \"" + exe  +"\"" );
     proc->waitForFinished(-1);

     if (msg) {
     int result = QMessageBox::question(0, tr("Question"), tr("Would you like to install a new game?"), QMessageBox::Yes, QMessageBox::No);
     if (result == QMessageBox::No)
         qApp->quit();
     }
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
    QProcess p (this);
  p.setProcessEnvironment(env);
  QStringList plist = pkgs.split(" ", QString::SkipEmptyParts);
    p.start("/usr/bin/winetricks", plist);
    p.waitForFinished(-1);
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
    QSettings stg(path + CTRL, QSettings::IniFormat, 0 );
stg.beginGroup("application");
return stg.value("prefix").toString();
}
QString engine::prefixPath(QString dir)
{
    if (engine::getPrefixName(dir).isEmpty())
        return QString();

    QString prefix = QDir::homePath() + winepath + QDir::separator() + engine::getPrefixName(dir);
        return prefix;
}

QString engine::getWine(QString path)
{
    QSettings stg(path + CTRL, QSettings::IniFormat, 0 );
 stg.beginGroup("wine");
 QString distr = stg.value("distr").toString();
 if (distr.isEmpty())
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
    QUrl myurl = QUrl(url);
    QFileInfo inf (myurl.path());
    QString wineFileName =TMP + QDir::separator() +  inf.fileName();
    //проверяем, есть ли у нас данный файл
    if (QFile::exists(wineFileName))
        return wineFileName;
     showNotify(tr("Don`t worry!"), tr("Now WineGame will download some files, that will need for get your applicaton running"));
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
    int kbAval = aval;
    int kbTotal = total;
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

void engine::doDesktop(QString workname)
     {
    /// эта функция - надеюсь, временная. Используется с переменной EXEPATH из control. Все из-за бага Wine: http://bugs.winehq.org/show_bug.cgi?id=17055
    // проголосуйте за исправление этого бага, пожалуйста. Тогда я удалю эту ф-цию к черту.
    QString desktopFile =QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + QDir::separator()+ workname + ".desktop"; //создаем ярлык на десктопе
    qDebug() << "debug: writing to" << desktopFile;

   QFile file (desktopFile);
   file.open(QIODevice::WriteOnly | QIODevice::Truncate);
   QTextStream stream (&file);
   stream <<  "[Desktop Entry]\n";
    QString exec = QObject::tr("env WINEPREFIX='%1' '%2'  '%3'").arg(prefix).arg(wineBinary).arg(program);
    stream << tr("Exec=%1\n").arg(exec);
    stream << tr("Name=%1\n").arg(name);
    stream << tr("Comment=%1\n").arg(note);
    stream << "Type=Application\n";
    stream << "Terminal=false\n";
       QFile iconFile (iconPath);
    QFileInfo info (iconFile);
    if (iconFile.exists()) {
        qDebug() << "engine: copy ico to " << QDir::homePath() + "/.local/share/icons/" + info.fileName();
    iconFile.copy(QDir::homePath() + "/.local/share/icons/" + info.fileName());
    stream << tr("Icon=%1").arg(QDir::homePath() + "/.local/share/icons/" + info.fileName());
}
 file.close();
}
void engine::setMemory(QString mem)
{
    const QString reg = "/tmp/win.reg";
    QStringList args;
    args << "regedit";
    args << reg;
    QFile f (reg);
    QTextStream stream (&f);
    f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    stream << "\n";
    stream << "REGEDIT4\n";
    stream << "[HKEY_CURRENT_USER\\Software\\Wine\\Direct3D]";
    stream << "\n";
    stream << "\"VideoMemorySize\"=";
    stream << tr("\"%1\"").arg(mem);
    stream << "\n";
    f.close();
QProcess p (this);
QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
env.insert("WINEPREFIX", prefix);
p.setProcessEnvironment(env);
p.start(wineBinary, args);
p.waitForFinished(-1);
}
QString engine::getStandardExe(QString controlFile)
{
    QSettings stg(controlFile, QSettings::IniFormat, 0 );
    stg.beginGroup("application");
    return stg.value("exepath").toString();
}

void engine::makefix(QString prefix)
{
    QFile file (prefix + "/system.reg");
    QTextStream stream (&file);
   //сначала откроем реестр для чтения
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString registry = stream.readAll();
    file.close();
    registry.replace ("winebrowser.exe -nohome", "winebrowser.exe -nohome %1");
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    stream << registry;
    file.close();
}
