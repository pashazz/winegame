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


DiskDialog::DiskDialog(QWidget *parent, DVDRunner *runner, corelib *lib, PluginWorker *wrk) :
	QDialog(parent),
	ui(new Ui::DiskDialog), core (lib), dvd (runner),worker(wrk)
{
    ui->setupUi(this);
	/* Get a plugins list */
	PluginList list = worker->plugins();
	if (!dvd->isMounted())
	{
		close();
		return;
	}
	coll = new PrefixCollection (core->database(), core, wrk, this);
	connect (this, SIGNAL(rejected()), dvd, SLOT(cancel()));
	TreeModel *model = new TreeModel(this, coll, list, true);
	ui->treeApps->setModel(model);
	ui->treeApps->expandAll();
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
	//About creating report
	/*Т.к. сейчас мало игр поддерживается, спрашиваем feedback.
	  (хотя и вне зависимости от того, используется ли Native или нет)
	*/
	AppSettings s (this);
	if (s.feedBack ())
	{
		if (QMessageBox::question(this, tr("Feedback"), tr("Do you want to provide a small feedback report? It will help us improving WineGame"), QMessageBox::Yes, QMessageBox::No)
			== QMessageBox::Yes)
			{
			QDir dir (dvd->diskDirectory());
			QStringList list = dirList(dir);
			FeedbackDialog *dlg = new FeedbackDialog(this, list, dvd->prefix()->name());
			dlg->exec();
		}
	}
	EjectDialog *dlg = new EjectDialog (this);
	connect(dlg, SIGNAL(ejectRequested(bool&)), dvd, SLOT(eject(bool&)));
	dlg->show();
	dlg->move((qApp->desktop()->width() - dlg->width()) / 2,
				  (qApp->desktop()->height() - dlg->height()) / 2 );
	if (!coll->havePrefix(prid))
	{
		TreeModel *model = static_cast<TreeModel*>(ui->treeApps->model());
		SourceReader *reader = model->readerFor(ui->treeApps->currentIndex());
		if (!reader)
		{
			QMessageBox::warning(this, tr("Cannot retreive SourceReader for this item"), tr("Hehehe, one of us is looser..."));
			dlg->close();
			return;
		}
		dvd->setPrefix(reader->prefix());
		QStringList obj = QStringList() << dvd->diskDirectory() << dvd->device();
		coll->install(reader, dvd->exe(), obj, false);
}
	else
	{
		Prefix *prefix = coll->getPrefix(prid);
		dvd->setPrefix(prefix);
		prefix->setDiscAttributes(dvd->diskDirectory(), dvd->device());
		QString exe = dvd->exe();
		if (exe.isEmpty())
			prefix->runApplication(QFileDialog::getOpenFileName(this, tr("Select EXE file"), dvd->diskDirectory(), tr("Windows executables (*.exe)")));
		else
			prefix->runApplication(exe);
	}
	dlg->close();
	close();
	return;
}

QStringList DiskDialog::dirList(QDir dir)
{
	static QDir mainPath = dir; //присваивается только 1 раз.
	QStringList myList = QStringList();
	foreach (QFileInfo directory, dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name))
	{
		myList.append(mainPath.relativeFilePath(directory.absoluteFilePath()));
		if (directory.isDir())
			myList.append(dirList(QDir(directory.absoluteFilePath())));
	}
	return myList;
}
