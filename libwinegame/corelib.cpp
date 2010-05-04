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


#include "corelib.h"
//The core of WineGame. Commonly used func.
corelib::corelib(QObject *parent)
    :QObject(parent)
{
}

QString corelib::whichBin(QString bin) {
    QProcess p (0);
    p.start("which", QStringList (bin));
    p.waitForFinished(-1);
    QString path = QString(p.readAll()).trimmed();
    path.remove('\n');
    return path;
}
void corelib::init()
{
    if (!QFile::exists(QDir::homePath() + config))
    {
		if (!QFile::exists(whichBin("wine")))
		{
			qDebug() << "FATAL!!!! Wine not found, I WILL QUIT!";
			qApp->exit(-4);
		}
        int mem = 0;
         mem= QInputDialog::getInt(0, QObject::tr("WineGame"), QObject::tr("Enter memory size of your video card (in megabytes). If you click Cancel, then default will be used"), 128, 1, 4096);
            if (mem == 0)
                mem = 128;
            QSettings stg (QDir::homePath() + config, QSettings::IniFormat, 0);
            stg.setValue("VideoMemory", mem);
            stg.sync();
    }

}

void corelib::unpackWine (QString distr, QString destination)
{
     QDir dir (destination);
     if (!dir.exists())
         dir.mkdir(dir.path());
/*     else
     {
         int result = QMessageBox::question(0, QObject::tr("Question"), QObject::tr("I see, that Wine did installed previously for this application (Directory %1 exists).<br>Would you like to install wine now?").arg(destination), QMessageBox::Yes, QMessageBox::No);
         if (result == QMessageBox::No)
             return;

     } */
 QProcess *proc = new QProcess (0); //не забываем удалять
 proc->setWorkingDirectory(destination);
 QString unpackLine =  QObject::tr ("tar xvpf %1 -C %2").arg(distr).arg(destination);
 proc->start(unpackLine);
  proc->waitForFinished(-1);
 qDebug() << QObject::tr("engine: Wine distribution %1 unpacked to %2").arg(distr).arg(destination);
}

QString corelib::downloadWine(QString url) //TODO: проверка на ошибки.
{
    QUrl myurl = QUrl(url);
    QFileInfo inf (myurl.path());
    QString wineFileName =TMP + QDir::separator() +  inf.fileName();
    //проверяем, есть ли у нас данный файл
    if (QFile::exists(wineFileName))
        return wineFileName;
     showNotify(tr("Don`t worry!"), tr("Now WineGame will download some files, that will need for get your applicaton running"));
  progress = new QProgressDialog(0);
     QEventLoop loop;
QNetworkAccessManager *manager = new QNetworkAccessManager (this);
QNetworkRequest req; //request для Url
this->progress = progress;
req.setUrl(QUrl(url));
req.setRawHeader("User-Agent", "Winegame-Browser 0.1");
QNetworkReply *reply = manager->get(req);
connect (reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(setRange(qint64,qint64)));
connect (reply, SIGNAL(finished()), &loop, SLOT(quit()));
connect (reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT (error(QNetworkReply::NetworkError)));
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


void corelib::error(QNetworkReply::NetworkError error)
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
void corelib::showNotify (QString header, QString body) //функция НУ СОВСЕМ не доделана.
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

void corelib::setRange(qint64 aval, qint64 total)
{
    int kbAval = aval;
    int kbTotal = total;
    progress->setMaximum(kbTotal);
    progress->setValue(kbAval);
}

void corelib::exitApp()
{
    QMessageBox::critical(0, tr("Critical error"), tr("Wine distribution not downloaded, so exit application."));
    qApp->exit(-4);
}

bool corelib::checkPrefixName(QString prefix)
{
    //пока что тут у нас проверяется на пробелы.
    if (prefix.contains(' '))
        return false;
 if (prefix == "wines") //зарезервировано для вайнов
     return false;
 return true;
}
