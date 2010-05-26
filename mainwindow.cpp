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
#include "about.h"
MainWindow::MainWindow(corelib *lib, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	core (lib)
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
    //показываем в статусбаре путь к диску
    QLabel * cdlab = new QLabel (diskpath);
   statusBar()->addWidget(cdlab);
buildList();
core->showNotify(tr("Hello"), tr("Please connect to Internet!"));
db = QSqlDatabase::database();
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
	QDir wdir (core->packageDir());
	ui->lstGames->clear();

    //а тут мы создаем родительский node
    QTreeWidgetItem *par = new QTreeWidgetItem (ui->lstGames);
    par->setIcon(0, QIcon(":/desktop/winegame.png"));
    par->setText(0, tr("Applications"));
	QTreeWidgetItem  *installed = new QTreeWidgetItem(par);
	installed->setText(0, tr("Installed applications"));
	installed->setIcon(0, QIcon(":/desktop/winegame.png"));
	QTreeWidgetItem *presetpar = new QTreeWidgetItem (ui->lstGames);
      presetpar->setIcon(0, QIcon(":/desktop/winegame.png"));
       presetpar->setText(0, tr("Pre-Sets (Templates)"));
    foreach (QString entry, wdir.entryList(QDir::Dirs  | QDir::NoDotAndDotDot))
    {
        Prefix myPrefix (this, core->packageDir() + QDir::separator() + entry);
        QTreeWidgetItem *it = new QTreeWidgetItem (0);
                it->setData(0, Qt::UserRole, core->packageDir() + QDir::separator() + entry);
        it->setText(0,  myPrefix.name());
			   //загружаем icon как значок игры (если есть)
        it->setIcon(0, myPrefix.icon());
		//Force adding to installed, if so.
        if (myPrefix.isPreset())
            presetpar->addChild(it);
		else if (myPrefix.hasDBEntry())
		{
			myPrefix.checkWineDistr();
			installed->addChild(it);
		}
		else
            par->addChild(it);
        myPrefix.deleteLater();
        }
	 if (installed->childCount() <= 0)
	{
		par->removeChild(installed);
	}
	ui->lstGames->expandAll();
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
//т.к. запуск идет из MainWindow, мы заранее знаем, что ни о каких исошках не может быть и  реч
QStringList filters;
filters.push_back(tr("Windows Executables (*.exe)"));
filters.push_back(tr("Microsoft Installers (*.msi)"));
QString filter;
QString fileName =  QFileDialog::getOpenFileName(this, tr("Select EXE/MSI file"), QDir::homePath(), filters.join(";;"), &filter);
if (fileName.isEmpty())
{
	statusBar()->showMessage(tr("No file selected, aborting"));
	return;
}
if (filter == filters.at(1)) //MSI

{
	//prepend "msiexec"
	fileName.prepend("msiexec ");
}
connect (prefix, SIGNAL(fileNeed(QString&)),this, SLOT(getFileName(QString&)));
connect (prefix, SIGNAL(prefixNameNeed(QString&)), this, SLOT (getPrefixName(QString&)));
connect (prefix, SIGNAL(error(QString)), this, SLOT(showError(QString)));
bool res  = prefix->runApplication(fileName, "", "");
if (!res)
showError(tr("Installation error"));

}

void MainWindow::getFileName(QString &fileName)
{
	fileName = QFileDialog::getOpenFileName(0,  tr("Выберите EXE файл"), QDir::homePath(), tr("Windows executables (*.exe)"));
}

void MainWindow::getPrefixName(QString &prefixName)
{
	dialog:
	QString myPrefix =QInputDialog::getText(0, tr("Give a name for your application"), tr ("Give a short latin name for your application. <br> It will be installed in %1/Windows/APPLICATION NAME/drive_c").arg(QDir::homePath()));
	if (myPrefix.isEmpty())
	{
		QMessageBox::critical(0, tr("No name specified"), tr ("Abort. Reason: No application name specified"));
		prefixName = "";
		return;
	}
	//проверим, есть ли такой префикс уже.
	QDir dir;
	dir.setPath(core->wineDir() + QDir::separator() + myPrefix);
	if (dir.exists()){

		QMessageBox::warning(0, tr("Application with this name is already installed."), tr("To force installation process, remove directory %1.").arg(core->wineDir() +QDir::separator() + myPrefix));
		//я знаю, это плохо
		goto dialog;
	}
	if (!corelib::checkPrefixName(myPrefix))
		goto dialog;
	prefixName = myPrefix;
	return;
}

void MainWindow::showError(QString error)
{
	QMessageBox::critical(this, tr("Engine execution error"), error);
}

void MainWindow::on_buttonBox_accepted()
{
   if (!ui->lstGames->selectedItems().isEmpty()) {
	   {
		   if (ui->lstGames->selectedItems().first()->data(0, Qt::UserRole).toString().isEmpty())
			   return;
	   lauchEngine(ui->lstGames->selectedItems().first()->data(0, Qt::UserRole).toString());
		buildList();
	}
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
	Prefix *prefix = new Prefix (this, item->data(column, Qt::UserRole).toString());
	if (!prefix->hasDBEntry())
		return; //нету установленных приложений здесь.
	QString exe = QFileDialog::getOpenFileName(0,  tr("Выберите EXE файл"), QDir::homePath(), tr("Windows executables (*.exe)"));
	 prefix->runProgram(exe);
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


void MainWindow::on_action_Settings_triggered()
{
	SettingsDialog *dlg = new SettingsDialog (this);
	dlg->exec();
}

void MainWindow::on_action_Quit_triggered()
{
	qApp->quit();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
	qApp->aboutQt();
}

void MainWindow::on_action_About_triggered()
{
	About *a = new About (this);
	a->exec();
}

void MainWindow::on_action_Make_desktop_icon_triggered()
{
	if (ui->lstGames->selectedItems().count() <= 0)
	{
		statusBar()->showMessage(tr("No item selected"));
		return;
	}
	Prefix *prefix = new Prefix (this, ui->lstGames->selectedItems().first()->data(0, Qt::UserRole).toString());
	ShortCutDialog *dlg = new ShortCutDialog (this, prefix->name(), prefix->prefixPath());
	if (dlg->exec() == QDialog::Accepted)
	{
		if (dlg->name().isEmpty() || dlg->path().isEmpty())
		{
			QMessageBox::warning(this, tr("warning"), tr("Append all fields"));
			return;
		}
		prefix->makeDesktopIcon(dlg->path(), dlg->name());
	}
}
