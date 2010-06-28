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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "prefixdialog.h"
#include "settingsdialog.h"
#include "shortcutdialog.h"
#include "treemodel.h"

namespace Ui {
    class MainWindow;
}
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
	MainWindow(corelib *lib, QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
private:
    Ui::MainWindow *ui;
	corelib *core;
	QSqlDatabase db;
	PrefixCollection *coll;
	void saveGeom();
	void buildList();
	void launchEngine(const QModelIndex &index);
	TreeModel *model;
	PluginWorker *worker;


private slots:
	void on_treeGames_activated(QModelIndex index);
	void on_treeGames_doubleClicked(QModelIndex index);
	void on_actUpdate_triggered();
	void on_lblNote_linkActivated(QString link);
	void on_action_Make_desktop_icon_triggered();
	void on_action_About_triggered();
	void on_actionAbout_Qt_triggered();
	void on_action_Quit_triggered();
	void on_action_Settings_triggered();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

	//Обработчики событий prefix
	};

#endif // MAINWINDOW_H
