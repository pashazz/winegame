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


#include "plugindialog.h"
#include "ui_plugindialog.h"

PluginDialog::PluginDialog(QWidget *parent, PluginWorker *worker) :
    QDialog(parent),
    ui(new Ui::PluginDialog)
{
    ui->setupUi(this);
	ui->tableWidget->setRowCount(worker->plugins().count());
	int i = 0;
	foreach (FormatInterface *plugin, worker->plugins())
	{
		QTableWidgetItem *file = new QTableWidgetItem (QIcon::fromTheme("application-x-executable"), worker->files().at(worker->plugins().indexOf(plugin)));
		file->setFlags(Qt::ItemIsEnabled);
		ui->tableWidget->setItem(i, 0, file);
		QTableWidgetItem *desc = new QTableWidgetItem(plugin->title());
		desc->setFlags(Qt::ItemIsEnabled);
		ui->tableWidget->setItem(i, 1, desc);
		QTableWidgetItem *author = new QTableWidgetItem(plugin->author());
		author->setFlags(Qt::ItemIsEnabled);
		ui->tableWidget->setItem(i, 2, author);
		i++;
	}
	ui->tableWidget->sortByColumn(0);
}

PluginDialog::~PluginDialog()
{
    delete ui;
}
