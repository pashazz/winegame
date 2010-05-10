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

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent), core (new corelib(this)),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
	ui->sbMemory->setMinimum(1);
	ui->sbMemory->setMaximum(4096);
	ui->txtPkg->setText(core->packageDir());
	ui->txtWineDir->setText(core->wineDir());
  ui->sbMemory->setValue(core->videoMemory().toInt());
	ui->txtMount->setText(core->mountDir());
	ui->cbForceFuse->setChecked(core->forceFuseiso());
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

void SettingsDialog::on_cmdBrowseWG_clicked()
{
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select Winegame Packages directory"), core->packageDir());
	if (!newdir.isEmpty())
		ui->txtPkg->setText(newdir);
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
	if (!ui->txtPkg->text().isEmpty())
		core->setPackageDir(ui->txtPkg->text());
	if (!ui->txtMount->text().isEmpty())
		core->setMountDir(ui->txtMount->text());
	core->setForceFuseiso(ui->cbForceFuse->isChecked());

	core->syncSettings();
}

void SettingsDialog::on_cmdBrowseMount_clicked()
{
	QString newdir = QFileDialog::getExistingDirectory(this, tr("Select Winegame directory for mounting ISOs"), core->mountDir());
	if (!newdir.isEmpty())
		ui->txtWineDir->setText(newdir);
}
