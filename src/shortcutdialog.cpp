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


#include "shortcutdialog.h"
#include "ui_shortcutdialog.h"

ShortCutDialog::ShortCutDialog(QWidget *parent, QString name, QString prefix) :
	QDialog(parent),
	ui(new Ui::ShortCutDialog), _prefix (prefix)
{
	ui->setupUi(this);
	ui->txtName->setText(name);
}

ShortCutDialog::~ShortCutDialog()
{
    delete ui;
}

void ShortCutDialog::changeEvent(QEvent *e)
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

QString ShortCutDialog::path()
{
	return ui->txtPath->text();
}
QString ShortCutDialog::name()
{
	return ui->txtName->text();
}

void ShortCutDialog::on_cmdBrowse_clicked()
{
	ui->txtPath->setText(QFileDialog::getOpenFileName(this, tr("Select EXE file"), _prefix + "/dosdevices/c:", tr("Windows executables (*.exe)")));
}

