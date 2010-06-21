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


#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent, corelib *lib) :
	QDialog(parent), core (lib),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
	ui->sbMemory->setMinimum(1);
	ui->sbMemory->setMaximum(4096);
	ui->txtWineDir->setText(core->wineDir());
	ui->sbMemory->setValue(core->videoMemory().toInt());
	ui->txtMount->setText(core->mountDir());
	ui->txtDisc->setText(core->discDir());
	if (lib->whichBin("fuseiso").isEmpty())
	{
		ui->cbForceFuse->setVisible(false);
		ui->lblFuse->setVisible(false);
	}
	else
		ui->cbForceFuse->setChecked(core->forceFuseiso());
	ui->lstPackageDirs->addItems(core->packageDirs());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SettingsDialog::on_cmdBrowseWine_clicked()
{
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select Winegame Windows storage directory"), core->wineDir());
	if (!newdir.isEmpty())
		ui->txtWineDir->setText(newdir);
}

void SettingsDialog::on_buttonBox_accepted()
{
	if (!ui->txtWineDir->text().isEmpty())
		core->setWineDir(ui->txtWineDir->text());
	core->setVideoMemory(ui->sbMemory->value());
	if (!ui->txtMount->text().isEmpty())
		core->setMountDir(ui->txtMount->text());
	if (!ui->txtDisc->text().isEmpty())
		core->setDiscDir(ui->txtDisc->text());
	core->setForceFuseiso(ui->cbForceFuse->isChecked());
	core->setPackageDirs(generateTexts());
	core->syncSettings();
}

void SettingsDialog::on_cmdBrowseMount_clicked()
{
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select Winegame directory for mounting ISOs"), core->mountDir());
	if (!newdir.isEmpty())
		ui->txtWineDir->setText(newdir);
}

void SettingsDialog::on_cmdBrowseDisc_clicked()
{
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select Winegame directory for copying files from multiple CDs"), core->discDir());
	if (!newdir.isEmpty())
		ui->txtWineDir->setText(newdir);
}

void SettingsDialog::on_cmdAdd_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select directory for Winegame packages"), QDir::homePath());
	if (!dir.isEmpty())
		ui->lstPackageDirs->addItem(dir);
}

void SettingsDialog::on_cmdDelete_clicked()
{
	foreach (QListWidgetItem *item, ui->lstPackageDirs->selectedItems())
	{
		ui->lstPackageDirs->takeItem(ui->lstPackageDirs->row(item));
		delete item;
	}
}


void SettingsDialog::on_lstPackageDirs_itemSelectionChanged()
{
	if (ui->lstPackageDirs->selectedItems().isEmpty())
	{
		ui->cmdDelete->setEnabled(false);
		ui->cmdEdit->setEnabled(false);
	}
	else
	{
		ui->cmdDelete->setEnabled(true);
		ui->cmdEdit->setEnabled(true);
	}
}

void SettingsDialog::on_cmdEdit_clicked()
{
	QString oldDir = ui->lstPackageDirs->selectedItems().first()->text();
	if (!QDir(oldDir).exists())
		oldDir = QDir::homePath();
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select directory for Winegame packages"), oldDir);
	if (!dir.isEmpty())
		ui->lstPackageDirs->selectedItems().first()->setText(dir);
}

QStringList SettingsDialog::generateTexts()
{
	QStringList list;
	for (int i = 0; i <= ui->lstPackageDirs->count() -1; i++)
	{
		list.append(ui->lstPackageDirs->item(i)->text());
	}
	return list;
}
