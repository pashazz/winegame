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

#include "prefixdialog.h"
#include "shortcutdialog.h"
#include "ui_prefixdialog.h"

PrefixDialog::PrefixDialog(QWidget *parent, Prefix *prefix, PrefixCollection *coll) :
    QDialog(parent),
    ui(new Ui::PrefixDialog),
	pr (prefix), collection(coll)
{
    ui->setupUi(this);
	ui->tabWidget->setCurrentIndex(0);//это если вдруг я забыл переместить страницу в designer
    setWindowTitle((pr->name()));
	connect (ui->cmdC, SIGNAL(clicked()), pr, SLOT (launch_c()));
}

PrefixDialog::~PrefixDialog()
{
    delete ui;
}

void PrefixDialog::on_cmdWinecfg_clicked()
{
	pr->runApplication("winecfg");
}

void PrefixDialog::on_cmdReg_clicked()
{
	pr->runApplication("regedit");
}

void PrefixDialog::on_cmdFM_clicked()
{
	pr->runApplication("winefile");
}

void PrefixDialog::on_cmdTask_clicked()
{
	pr->runApplication("taskmgr");
}

void PrefixDialog::on_cmdEXE_clicked()
{
	QFileDialog *dlg = new QFileDialog (this, tr("Select Windows program to run"), pr->path() + "/drive_c", tr("Windows executables (*.exe)"));
    dlg->setFileMode(QFileDialog::ExistingFile);
    if (dlg->exec() == QDialog::Accepted)
	{
		if (QMessageBox::question(this, tr("Desktop icon"), tr("Do you want to create desktop icon for this application?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		{
			//open shortcut dialog
			ShortCutDialog *scut = new ShortCutDialog(this, pr->name(), pr->path(), dlg->selectedFiles().at(0));
			scut->exec();
			pr->makeDesktopIcon(scut->name(), scut->path(), "");
		}
		pr->runApplication(dlg->selectedFiles().at(0));
	}
}

void PrefixDialog::on_cmdWP_clicked()
{
	pr->runApplication("wordpad", QDir::homePath());
}

void PrefixDialog::on_cmdTerm_clicked()
{
	pr->runApplication("\"wineconsole cmd\"", QDir::homePath());
}

void PrefixDialog::on_cmdNotepad_clicked()
{
	pr->runApplication("notepad");
}

void PrefixDialog::on_cmdUninst_clicked()
{
	pr->runApplication("uninstaller");
}

void PrefixDialog::on_cmdBoot_clicked()
{
	pr->runApplication("wineboot");
}

void PrefixDialog::on_cmdRemove_clicked()
{
	collection->remove(pr->ID());
	close();
}

void PrefixDialog::on_cmdControl_clicked()
{
	 pr->runApplication("control");
}

void PrefixDialog::on_cmdTest_clicked()
{
   PolDownloader *pol = new PolDownloader(collection, pr->ID(), pr->lib());
   WineVersionsDialog *dlg = new WineVersionsDialog(this, pol->versionList(), pol->detectCurrentVersion());
   connect (dlg, SIGNAL(fallback()), pol, SLOT(fallback())); //function to restore defaults
   if (dlg->exec() == QDialog::Accepted)
   {
	   if (!dlg->fallbackRequested())
		  if (!pol->setWineVersion(dlg->wineVersion()))
			  QMessageBox::warning(this, tr("Error"), tr("Unable to set WINE version"));
	   //update Prefix object.
	   QString id = pr->ID();
	   delete pr;
	  pr = collection->getPrefix(id);
   }
}
