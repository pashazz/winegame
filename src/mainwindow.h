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
	bool checkNodeForPrefix (QTreeWidget *widget);
	void saveGeom();
	void buildList();
	void launchEngine(QString prefixName, bool install);

private slots:
	void on_lblNote_linkActivated(QString link);
	void on_action_Make_desktop_icon_triggered();
	void on_action_About_triggered();
	void on_actionAbout_Qt_triggered();
	void on_action_Quit_triggered();
	void on_action_Settings_triggered();
	void on_lstGames_itemClicked(QTreeWidgetItem* item, int column);
    void on_lstGames_itemDoubleClicked(QTreeWidgetItem* item, int column);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

	//Обработчики событий prefix
	void getPrefixName (QString &prefixName);
	void getFileName (QString &fileName);
	void getPresetName (QString &name);
	void getPresetNote (QString &note);
};

#endif // MAINWINDOW_H
