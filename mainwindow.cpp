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
corelib *core  = new corelib(this);
core->showNotify(tr("Hello"), tr("Please connect to Internet!"));
ui->lstGames->expandAll();

//Обновляем вайн
core->updateWines();
core->deleteLater();
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
    QStringList taboo;
    //поддержка пресетов
    taboo << "presets";

    //а тут мы создаем родительский node
    QTreeWidgetItem *par = new QTreeWidgetItem (ui->lstGames);
    par->setIcon(0, QIcon(":/desktop/winegame.png"));
    par->setText(0, tr("Applications"));
    QTreeWidgetItem *presetpar = new QTreeWidgetItem (ui->lstGames);
      presetpar->setIcon(0, QIcon(":/desktop/winegame.png"));
       presetpar->setText(0, tr("Pre-Sets (Templates)"));
    foreach (QString entry, wdir.entryList(QDir::Dirs  | QDir::NoDotAndDotDot))
    {
        if (taboo.contains(entry))
            continue;
        Prefix myPrefix (this, gamepath + QDir::separator() + entry);
        QTreeWidgetItem *it = new QTreeWidgetItem (0);
                it->setData(0, Qt::UserRole, gamepath + QDir::separator() + entry);
        if (AutoPackage::isAutoPackage(gamepath + QDir::separator() + entry))
            it->setData(0, 63, true); //ролью 63 мы определяем, что это автопакет.
        it->setText(0,  myPrefix.name());
        //загружаем icon как значок игры (если есть)
        it->setIcon(0, myPrefix.icon());
        if (myPrefix.isPreset())
            presetpar->addChild(it);
        else
            par->addChild(it);
        myPrefix.deleteLater();
        }
    }


void MainWindow::lauchEngine(QString pkgpath)
{
    Prefix *prefix = new Prefix (this, pkgpath);
    QDir dir (prefix->prefixPath());
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
if  (AutoPackage::isAutoPackage(pkgpath))
{
    //это автопакет, значит он предназначен для установки с CD/DVD
    QMessageBox::warning(this, tr("Warning"), tr("It`s necessary that the application was installed from CD/DVD.<br>To do this , insert DVD witrg run WineGame with your cd/dvd (for example, winegame /media/cdrom from console."));
    return;
}
engine *eng = new engine (this);
   eng->setDiskpath(diskpath);
   eng->setCdMode(cdMode);
   eng->lauch(pkgpath);

}

void MainWindow::on_buttonBox_accepted()
{
    if (ui->lstGames->selectedItems().first()->data(0, Qt::UserRole).toString().isEmpty())
        return;
   if (!ui->lstGames->selectedItems().isEmpty()) {
         lauchEngine(ui->lstGames->selectedItems().first()->data(0, Qt::UserRole).toString());
    }

    else
    {
        QMessageBox::warning(this, tr("Warning"), tr("Select item to run"));
    }
}

void MainWindow::saveGeom()
{
    QFile f (QDir::homePath() + "/.config/winegame.geom");
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
   f.write(saveGeometry());
   f.close();
}

void MainWindow::on_lstGames_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (item->data(0, 64).toBool())
        return;
    //Запуск приложения по EXEPATH
    if (item->data(column, Qt::UserRole).toString().isEmpty())
        return;

    QString workdir = item->data(column, Qt::UserRole).toString();
    Prefix  *prefix = new Prefix (this, workdir);
    QString exe = prefix->standardExe();
    if (!exe.isEmpty())
    {
    QString wineprefix = prefix->prefixPath();
     QString wine = prefix->wine();
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
prefix->deleteLater();
}

void MainWindow::on_lstGames_itemClicked(QTreeWidgetItem* item, int column)
{
    if (item->data(column, Qt::UserRole).toString().isEmpty())
    {
        ui->lblNote->setText("");
        return;
    }
    Prefix *prefix = new Prefix (this, item->data(column, Qt::UserRole).toString());
    ui->lblNote->setText(prefix->note());
    prefix->deleteLater();
}

