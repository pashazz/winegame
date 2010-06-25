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

DiskDialog::DiskDialog(QWidget *parent, DVDRunner *runner, corelib *lib) :
	QDialog(parent),
	ui(new Ui::DiskDialog), core (lib), dvd (runner)
{
    ui->setupUi(this);
	if (!dvd->isMounted())
	{
		close();
		return;
	}
	coll = new PrefixCollection (core->database(), core, this);
	connect (this, SIGNAL(rejected()), dvd, SLOT(cancel()));
	TreeModel *model = new TreeModel(this, coll);
	ui->treeApps->setModel(model);
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

void DiskDialog::on_buttonBox_accepted()
{
	if (ui->treeApps->currentIndex() == QModelIndex())
		qApp->quit();
	QString prid = ui->treeApps->currentIndex().data(32).toString();
	if (!coll->havePrefix(prid))
	{
		SourceReader reader (prid, core, this);
		MessageHandler *handler = new MessageHandler(this, core);
		connect (&reader, SIGNAL(presetNameNeed(QString&)), handler, SLOT(prefixName(QString&)));
		connect (&reader, SIGNAL(presetNoteNeed(QString&)), handler, SLOT(prefixNote(QString&)));
		connect(&reader, SIGNAL(presetPrefixNeed(QString&)), handler, SLOT(prefixID(QString&)));
		dvd->setReader(&reader);
		if (!dvd->exe().isEmpty())
			coll->install(&reader, dvd->exe(), dvd->diskDirectory());
	}
	else
	{
		Prefix *prefix = coll->getPrefix(prid);
		prefix->runApplication(dvd->exe());
	}
	close();
	return;
}


