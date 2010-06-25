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
		core (lib), db(lib->database()), coll(new PrefixCollection(lib->database(), lib, this))
{
	ui->setupUi(this);
	QFile f (QDir::homePath() + "/.config/winegame.geom");
    if (f.open(QIODevice::ReadOnly))
    {
		restoreGeometry(f.readAll());
		f.close();
	}
	TreeModel *model = new TreeModel(this, coll);
	ui->treeGames->setModel(model);
	ui->treeGames->expandAll();
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
	TreeModel *model = static_cast<TreeModel*>(ui->treeGames->model());
	model->resetDatas();
	ui->treeGames->expandAll();
}


void MainWindow::launchEngine(QString prefixName)
{
	if ((!coll->havePrefix(prefixName)))
	{
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
		SourceReader *reader = new SourceReader(prefixName, core, this);
		connect (reader, SIGNAL(presetPrefixNeed(QString&)), this, SLOT (getPrefixName(QString&)));
		connect(reader, SIGNAL(presetNameNeed(QString&)), this, SLOT(getPresetName(QString&)));
		connect (reader, SIGNAL(presetNoteNeed(QString&)), this, SLOT(getPresetNote(QString&)));
		Prefix *prefix = coll->install(reader,fileName);
		buildList();
		if (!prefix)
			statusBar()->showMessage(tr("Installation error"), 3000);
		else if (QMessageBox::question(this, tr("Application installed successfully"), tr("Do you want to configure parameters for this application?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		{
			PrefixDialog *dlg = new PrefixDialog (this, prefix, coll);
			dlg->exec();
		}
	}
	else
	{
		Prefix *p =  coll->getPrefix(prefixName);
		PrefixDialog *dlg = new PrefixDialog(this, p, coll);
		dlg->exec();
	}
	buildList();
}

void MainWindow::getFileName(QString &fileName)
{
	fileName = QFileDialog::getOpenFileName(0,  tr("Выберите EXE файл"), QDir::homePath(), tr("Windows executables (*.exe)"));
}

void MainWindow::getPresetName(QString &name)
{
	QString myPrefix =QInputDialog::getText(0, tr("Give a readable for your application"), tr ("Give a short readable name for your application, for example 'CoolGame v3'"));
	if (myPrefix.isEmpty())
	{
		QMessageBox::critical(0, tr("No name specified"), tr ("Abort. Reason: No application name specified"));
		name = "";
		return;
	}
	else
		name = myPrefix;
}
void MainWindow::getPresetNote(QString &note)
{
	note =QInputDialog::getText(0, tr("Give a readable note about your application"), tr ("Give a short note about your application, for example 'CoolGame v3'"));
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
	if (dir.exists())
	{
		QMessageBox::warning(0, tr("Application with this name is already installed."), tr("To force installation process, remove directory %1.").arg(core->wineDir() +QDir::separator() + myPrefix));
		//я знаю, это плохо
		goto dialog;
	}
	if (!corelib::checkPrefixName(myPrefix))
		goto dialog;
	prefixName = myPrefix;
	return;
}

void MainWindow::on_buttonBox_accepted()
{
if (ui->treeGames->currentIndex() == QModelIndex())
	qApp->quit();
else
{
	QString id = ui->treeGames->currentIndex().data(32).toString();
	if (id.isEmpty())
		qApp->quit();
	launchEngine(id);
}
}

void MainWindow::saveGeom()
{
	QFile f (QDir::homePath() + "/.config/winegame.geom");
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
	f.write(saveGeometry());
	f.close();
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
	prefix->makeDesktopIcon(dlg->path(), dlg->name(), SourceReader(prefix->ID(),core,this).icon());
}

void MainWindow::on_lblNote_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actUpdate_triggered()
{
	core->syncPackages();
}

void MainWindow::on_treeGames_doubleClicked(QModelIndex index)
{
	QString id = index.data(32).toString();
	launchEngine(id);
}


void MainWindow::on_treeGames_activated(QModelIndex index)
{
	QString note = index.data(34).toString();
	if (!note.isEmpty())
		ui->lblNote->setText(note);
}
