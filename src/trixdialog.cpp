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


#include "trixdialog.h"
#include "ui_trixdialog.h"
#include "winetricksmodel.h"

TrixDialog::TrixDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrixDialog)
{
    ui->setupUi(this);
	WinetricksModel *model = new WinetricksModel(this, "");
	ui->tblComp->setModel(model);
}

TrixDialog::~TrixDialog()
{
    delete ui;
}

QString TrixDialog::componentName()
{
	return ui->txtComponent->text();
}

void TrixDialog::on_tblComp_clicked(QModelIndex index)
{
	if (index.column() != 0)
		return;
	ui->txtComponent->setText(ui->tblComp->model()->data(index).toString());
}
