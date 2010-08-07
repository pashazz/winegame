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
#include "plugindialog.h"

MainWindow::MainWindow(corelib *lib, QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::MainWindow),
		core (lib), db(lib->database()), worker(new PluginWorker(this, lib)), coll(new PrefixCollection(lib->database(), lib, worker, this)),
		install (false)
{
	ui->setupUi(this);
	if (core->autoSync())
	{
		foreach (FormatInterface *plugin, worker->plugins())
		{
			plugin->updateAllWines(coll);
		}
	}

	model = new TreeModel(this, coll, worker->plugins(), false);
	ui->treeGames->setModel(model);
	ui->treeGames->expandAll();
	setSettings (1);
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
    qApp->exit(2);
}

void MainWindow::buildList()
{
	TreeModel *model = static_cast<TreeModel*>(ui->treeGames->model());
	model->resetDatas();
	ui->treeGames->expandAll();
}

void MainWindow::launchEngine(const QModelIndex &index)
{
	QString prefixName = index.data(32).toString();
	if (prefixName.isEmpty())
		return;
	if (!coll->havePrefix(prefixName))
	{
		SourceReader *reader = model->readerFor(index);
		QString fileName;
		if (reader->needFile())
		{
		QStringList filters;
		filters.push_back(tr("Windows Executables (*.exe)"));
		filters.push_back(tr("Microsoft Installers (*.msi)"));
		filters.push_back(tr("Windows CMD Scripts (*.bat)"));
		QString filter;
		fileName =  QFileDialog::getOpenFileName(this, tr("Select EXE/MSI file"), QDir::currentPath(), filters.join(";;"), &filter);
		if (fileName.isEmpty())
		{
			statusBar()->showMessage(tr("No file selected, aborting"));
			return;
		}
	}
		install = true;
		Prefix *prefix = coll->install(reader, fileName);
		if (!prefix)
			statusBar()->showMessage(tr("Installation error"), 3000);
		else if (QMessageBox::question(this, tr("Application installed successfully"), tr("Do you want to configure parameters for this application?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		{
			PrefixDialog *dlg = new PrefixDialog (this, prefix, coll);
			dlg->exec();
		}
		install = false;
	}
	else
	{
		install = true;
		Prefix *p =  coll->getPrefix(prefixName);
		PrefixDialog *dlg = new PrefixDialog(this, p, coll);
		dlg->exec();
		install = false;
	}
	buildList();
}

void MainWindow::on_buttonBox_accepted()
{
if (!ui->treeGames->currentIndex().isValid())
	qApp->quit();
else
{
	QString id = ui->treeGames->currentIndex().data(32).toString();
	if (id.isEmpty())
		qApp->quit();
	launchEngine(ui->treeGames->currentIndex());
}
}


void MainWindow::on_action_Settings_triggered()
{
	SettingsDialog *dlg = new SettingsDialog (this, core);
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
	QString id = ui->treeGames->currentIndex().data(32).toString();
	if (id.isEmpty())
		return;
	if (!coll->havePrefix(id))
	{
		statusBar()->showMessage(tr("Application isn`t installed"), 3000);
		return;
	}
	Prefix *prefix = coll->getPrefix(id);
	ShortCutDialog *dlg = new ShortCutDialog(this, prefix->name(), prefix->path());
	dlg->exec();
	prefix->makeDesktopIcon(dlg->name(), dlg->path(), ui->treeGames->currentIndex().data(35).toString());
}

void MainWindow::on_lblNote_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actUpdate_triggered()
{
	foreach (FormatInterface *plugin, worker->plugins())
		plugin->updateAllWines(coll);
}

void MainWindow::on_treeGames_doubleClicked(QModelIndex index)
{
	launchEngine(index);
}

void MainWindow::on_treeGames_activated(QModelIndex index)
{
	QString note = index.data(34).toString();
	if (!note.isEmpty())
		ui->lblNote->setText(note);
}

void MainWindow::on_actAboutPlugins_triggered()
{
	PluginDialog *dlg = new PluginDialog(this, worker);
	dlg->exec();
}

void MainWindow::closeEvent (QCloseEvent *e)
{
    if (install)
    {
	core->client ()->showNotify (tr( "Unable to quit"), tr("Please wait while installation ends"));
	e->ignore ();
    }
    else
    {
	setSettings (2);
	e->accept ();
    }
}

void MainWindow::setSettings (int role)
{
    static AppSettings s (this);
    switch (role)
    {
    case 1:
	{
	    //restore state
	    restoreState (s.readState ("MainWindow"));
	    restoreGeometry (s.readGeometry ("MainWindow"));
	    break;
	}
    case 2:
	{ //save state
	   s.writeState (saveState (), "MainWindow");
	   s.writeGeometry (saveGeometry (), "MainWindow");
	}
    }
}
