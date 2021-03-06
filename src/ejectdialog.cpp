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


#include "ejectdialog.h"
#include "ui_ejectdialog.h"

EjectDialog::EjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EjectDialog)
{
    ui->setupUi(this);
	connect(ui->cmdSwitch, SIGNAL(clicked()),this, SLOT(switchDisc()));
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::Tool);
}

EjectDialog::~EjectDialog()
{
    delete ui;
}

void EjectDialog::switchDisc()
{
	hide();
	bool result;
	emit ejectRequested(result);
	if (!result)
	{
		QMessageBox::warning(this, tr("Error"), tr("Failed to switch disc. Maybe device is busy?"));
	}
	show();
}
