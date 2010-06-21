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
	coll = new PrefixCollection (core->database(), core, this);
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
{ /* TODO: сделать модель для списка */
	//сначала вытаскиваем все из reader.
	QTreeWidgetItem *itemReader = new QTreeWidgetItem(0);
	itemReader->setText(0,tr("Not installled applications"));
	foreach (QString prefixName, SourceReader::configurations(core->packageDirs()))
	{
		//init Reader object
		SourceReader reader (prefixName, core, this);
		 //add it into this  list
		QTreeWidgetItem *item = new QTreeWidgetItem(0);
		item->setText(0, reader.realName());
		item->setToolTip(0, reader.realNote());
		item->setIcon(0, QIcon(reader.icon()));
		item->setData(0, 32, prefixName);
		item->setData(0, 33, true); // true - делаем полную установку данного приложения
		itemReader->addChild(item);
	}
	ui->treeApps->addTopLevelItem(itemReader);
	//а теперь установленные.
	QTreeWidgetItem *itemInstalled = new QTreeWidgetItem(0);
	foreach (Prefix *prefix, coll->prefixes())
	{
		//add it into this list
		QTreeWidgetItem *item = new QTreeWidgetItem (0);
		item->setText(0,prefix->name());
		item->setData(0, 32, prefix->ID());
		item->setData(0, 33, false); //false - просто создаем Prefix и запускаем exe без доп. действий.
		item->setToolTip(0,prefix->note());
		item->setIcon(0, QIcon(SourceReader(prefix->ID(), core, this).icon())); // тут иконку достаем из sourcereader
		itemInstalled->addChild(item);
	}
	ui->treeApps->addTopLevelItem(itemInstalled);
	ui->treeApps->sortItems(0, Qt::AscendingOrder);
}


void DiskDialog::on_buttonBox_accepted()
{
	if (ui->treeApps->selectedItems().count() == 0)
		return;
	//проверяем, является ли item - top-level
	for (int i = 0; i < ui->treeApps->topLevelItemCount(); i++)
	{
		if (ui->treeApps->selectedItems().contains(ui->treeApps->topLevelItem(i)))
			return;
	}
	QString prid = ui->treeApps->selectedItems().first()->data(0, 32).toString();
	bool ist = ui->treeApps->selectedItems().first()->data(0, 33).toBool();
	if (ist)
	{
		SourceReader *reader = new SourceReader (prid, core, this);
		coll->install(reader, dvd->exe(), dvd->diskDirectory());
	}
	else
	{
		if (!coll->havePrefix(prid))
		{
			QMessageBox::critical(this, tr("Error"), tr("Program error (unusable instruction"));
			close();
			return;
		}
	Prefix *prefix = coll->getPrefix(prid);
	prefix->runApplication(dvd->exe());
	}
	close();
	return;
}

