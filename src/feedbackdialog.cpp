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


#include "feedbackdialog.h"
#include "ui_feedbackdialog.h"

FeedbackDialog::FeedbackDialog(QWidget *parent, const QStringList &fileList, const QString &confName) :
	QDialog(parent), cname(confName), list(fileList),
    ui(new Ui::FeedbackDialog)
{
    ui->setupUi(this);
	ui->txtSave->setText(QString("%1/report-%2.txt").arg(QDir::homePath(), confName));
}

FeedbackDialog::~FeedbackDialog()
{
    delete ui;
}

void FeedbackDialog::on_cmdBrowse_clicked()
{
	QString text = QFileDialog::getSaveFileName(this, tr("Where to save report file?"), QDir::currentPath());
	if (!text.isEmpty())
		ui->txtComment->setText(text);
}

void FeedbackDialog::on_buttonBox_accepted()
{
	if (ui->txtSave->text().isEmpty())
	{
		QMessageBox::critical(this,tr("Report Creator"), tr("Report file name is empty. Exit."));
		return;
	}
	QFile file (ui->txtSave->text());
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(this, tr("Report Creator"), tr("Unable to open report file..."));
		return;
	}
	QTextStream stream(&file);
	stream << QString("Winegame install report: %1\n").arg(QDateTime::currentDateTime().toString(Qt::SystemLocaleDate));
	stream << "\n\n";
	stream << QString("Disc file list:\n %1").arg(list.join("\n"));
	stream << QString("Configuration name: %1").arg(cname);
	stream << "\n";
	stream << QString ("Comment:%1\n").arg(ui->txtComment->toPlainText());

	file.close();
}
