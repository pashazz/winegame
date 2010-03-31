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

#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile f (QDir::homePath() + "/.config/winegame.geom");
    if (f.open(QIODevice::ReadOnly))
    {
    restoreGeometry(f.readAll());
    f.close();
}
    if (qApp->arguments().empty())
        cdMode = false; //Not to check CD
    else
    {
        if (qApp->arguments().length() > 1) {
        QString cdpath = qApp->arguments().at(1);
        QDir td (QDir::rootPath());
        cdMode = td.exists(cdpath);
        diskpath = cdpath;
    }
        else
            cdMode = false;
    }
    //показываем в статусбаре пть к диску
    QLabel * cdlab = new QLabel (diskpath);
   statusBar()->addWidget(cdlab);
buildList();
engine::showNotify(tr("Hello"), tr("Please connect to Internet!"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_buttonBox_rejected()
{
    saveGeom();
    qApp->exit(2);
}
void MainWindow::buildList()
{
    QDir wdir (gamepath);
    foreach (QString entry, wdir.entryList(QDir::Dirs  | QDir::NoDotAndDotDot))
    {
        QListWidgetItem *it = new QListWidgetItem (ui->lstGames);
        it->setData(Qt::UserRole, gamepath + QDir::separator() + entry);
        it->setText(engine::getName( gamepath + QDir::separator() + entry));
        //загружаем icon как значок игры (если есть)
        it->setIcon(engine::getIcon(gamepath + QDir::separator() + entry));
        }
    }


void MainWindow::lauchEngine(QString pkgpath)
{
    QDir dir (QDir::homePath() + winepath + QDir::separator() + engine::getPrefixName(pkgpath));
if (dir.exists())
{
    QStringList list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (list.contains("drive_c") && list.contains("dosdevices"))
    {
   PrefixDialog *dlg = new PrefixDialog(this,pkgpath);
  dlg->exec();
  return;
    }
}
    engine *eng = new engine (this);
   eng->setDiskpath(diskpath);
   eng->setCdMode(cdMode);
   eng->lauch(pkgpath);

}

void MainWindow::on_buttonBox_accepted()
{
    if (!ui->lstGames->selectedItems().isEmpty()) {
        lauchEngine(ui->lstGames->selectedItems().first()->data(Qt::UserRole).toString());
   saveGeom();
    }

    else
    {
        QMessageBox::warning(this, tr("Warning"), tr("Select item to run"));
    }
}



void MainWindow::on_lstGames_itemClicked(QListWidgetItem* item)
{
    ui->lblNote->setText(engine::getNote(item->data(Qt::UserRole).toString()));
}

void MainWindow::saveGeom()
{
    QFile f (QDir::homePath() + "/.config/winegame.geom");
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
   f.write(saveGeometry());
   f.close();
}

void MainWindow::on_lstGames_itemDoubleClicked(QListWidgetItem* item)
{
    //Запуск приложения по EXEPATH
    QString workdir = item->data(Qt::UserRole).toString();
    QString exe = engine::getStandardExe(workdir + "/control");
    if (!exe.isEmpty())
    {

    QString wineprefix = engine::prefixPath(workdir);
    QString wine = engine::getWine(workdir);
    //QtConcurrent
    qDebug() << "MainWindow: starting EXE:" << wine << exe;
    QProcess p (this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("WINEPREFIX", wineprefix);
    env.insert("WINEDEBUG", "-all");
    p.setProcessEnvironment(env);
    p.start(wine, QStringList(exe));
    p.waitForFinished(-1);
}
}
