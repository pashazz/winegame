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


#ifndef DISKDIALOG_H
#define DISKDIALOG_H

#include <QtGui>
#include "prefixcollection.h"
#include "dvdrunner.h"
#include "feedbackdialog.h"
#include "treemodel.h"
#include "ejectdialog.h"
#include "appsettings.h"

namespace Ui {
    class DiskDialog;
}

class DiskDialog : public QDialog {
    Q_OBJECT
public:
	DiskDialog(QWidget *parent, DVDRunner *runner, corelib *lib, PluginWorker *wrk);
    ~DiskDialog();

protected:
    void changeEvent(QEvent *e);
	QStringList dirList (QDir dir);
private:
    Ui::DiskDialog *ui;
	void buildList();
	corelib *core;
	DVDRunner *dvd;
	PrefixCollection *coll;
	PluginWorker *worker;
private slots:
 void on_buttonBox_accepted();
};

#endif // DISKDIALOG_H
