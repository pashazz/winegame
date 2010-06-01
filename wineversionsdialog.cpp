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


#include "wineversionsdialog.h"
#include "ui_wineversionsdialog.h"

WineVersionsDialog::WineVersionsDialog(QWidget *parent, const QStringList &wineVersions, const QString &currentVersion) :
    QDialog(parent),
	ui(new Ui::WineVersionsDialog), fb(false)
{
    ui->setupUi(this);
	//Заполняем список wine
	ui->wineBox->addItems(wineVersions);
	if (currentVersion.isEmpty())
		ui->lblVersion->setText(tr("Using Default wine version"));
	else
		ui->lblVersion->setText(tr("Using %1 wine version").arg(currentVersion));
}

WineVersionsDialog::~WineVersionsDialog()
{
    delete ui;
}

void WineVersionsDialog::changeEvent(QEvent *e)
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

void WineVersionsDialog::on_buttonBox_accepted()
{
	//фиксируем
	version  = ui->wineBox->currentText();
}

void WineVersionsDialog::on_buttonBox_clicked(QAbstractButton* button)
{
	if (ui->buttonBox->standardButton(button) == QDialogButtonBox::RestoreDefaults)
	{
		emit fallback();
		ui->lblVersion->setText(tr("Using Default wine version"));
		fb = true;
	}
}
