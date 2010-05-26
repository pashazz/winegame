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

#ifndef PREFIXDIALOG_H
#define PREFIXDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include "prefix.h"

namespace Ui {
    class PrefixDialog;
}

class PrefixDialog : public QDialog {
    Q_OBJECT
public:
	PrefixDialog(QWidget *parent, QString path, corelib *core);
    ~PrefixDialog();

private:
    Ui::PrefixDialog *ui;
    Prefix *pr;

private slots:
    void on_cmdControl_clicked();
    void on_cmdRemove_clicked();
    void on_cmdBoot_clicked();
    void on_cmdUninst_clicked();
    void on_cmdNotepad_clicked();
    void on_cmdTerm_clicked();
    void on_cmdWP_clicked();
    void on_cmdEXE_clicked();
    void on_cmdTask_clicked();
    void on_cmdFM_clicked();
    void on_cmdReg_clicked();
    void on_cmdWinecfg_clicked();
};

#endif // PREFIXDIALOG_H
