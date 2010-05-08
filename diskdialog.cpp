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


#include "diskdialog.h"
#include "ui_diskdialog.h"

DiskDialog::DiskDialog(QWidget *parent, corelib *lib, QString cdroot) :
	QDialog(parent),
	ui(new Ui::DiskDialog), core (lib), path(cdroot)
{
    ui->setupUi(this);
	buildList();
}

DiskDialog::~DiskDialog()
{
    delete ui;
}

void DiskDialog::changeEvent(QEvent *e)
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
void DiskDialog::buildList()
{
	QDir dir (core->packageDir());
	qDebug() << dir.path();
	foreach (QFileInfo info, dir.entryInfoList(QDir::Dirs | QDir::Readable))
	{
		//init Prefix object
		Prefix *prefix = new Prefix (this, info.absoluteFilePath());
		if (prefix->isPreset())
		{
		  //add it into this  list
		  QListWidgetItem *item = new QListWidgetItem (ui->lstPresets, 0);
		  item->setToolTip(prefix->note());
		  item->setText(prefix->name());
		  item->setData(Qt::UserRole, info.absoluteFilePath());
		  ui->lstPresets->addItem(item);
	  }
	}
}


void DiskDialog::on_buttonBox_accepted()
{
	if (ui->lstPresets->selectedItems().count() == 0)
		return;
	qDebug() << "ddlg: lauching....";
	engine *eng = new engine(this);
	eng->setDiskpath(path);
	eng->setCdMode(true);
	hide();
	eng->lauch(ui->lstPresets->selectedItems().first()->data(Qt::UserRole).toString(), false);
	close();

}
