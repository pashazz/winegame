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

#include "winegameui.h"
#include "gamedialog.h"
#include "ui_gamedialog.h"

GameDialog::GameDialog(QWidget *parent, QString path, corelib *lib) :
    QDialog(parent),
    ui(new Ui::GameDialog),
    _path (path)
{
	Prefix *prefix = new Prefix (this, path, lib);
    ui->setupUi(this);
    //setting the UI
    if (qApp->arguments().length() > 1)
		  ui->lblIcon->setPixmap(QPixmap(path + "/icon"));
	else
	   ui->lblIcon->setPixmap(getIcoFromDisc());

    ui->lblIcon->setText("");
    ui->lblName->setText(tr("A Microsoft Windows(r) application is found on this disc. <br><br><b>%1</b><br><br> Would you like to install it? ").arg(prefix->name()));
    ui->lblDesc->setText(prefix->note());

}

GameDialog::~GameDialog()
{
    delete ui;
}
QPixmap GameDialog::getIcoFromDisc()
{
if (qApp->arguments().length() > 1)
    {
	QString droot = qApp->arguments().at(1);
	if (QFile::exists (corelib::autorun(droot)))
    {
		QSettings stg (corelib::autorun(droot),  QSettings::IniFormat, this);
    stg.beginGroup("autorun");
    QString icon = qApp->arguments().at(1) + QDir::separator() + stg.value("Icon").toString();
    qDebug() << "engine: ico file detected" << icon;
    return QPixmap(icon);
}
    else
        return QPixmap(_path + "/icon");
}
return QPixmap(_path + "/icon");
}