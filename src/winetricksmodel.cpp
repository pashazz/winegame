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


#include "winetricksmodel.h"

WinetricksModel::WinetricksModel(QObject *parent, QString winetricks) :
	QAbstractTableModel(parent), wtricks(winetricks)
{
	makeData();
}

void WinetricksModel::makeData()
{
	//run winetricks
	QProcess p;
	if (wtricks.isEmpty() || (!QFileInfo(wtricks).isFile()))
		wtricks = "winetricks";
	p.start(wtricks, QStringList("help"));
	p.waitForFinished();
	QStringList rawStr = QString (p.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);
	//Remove non-splittable entries from the array
	while (rawStr.first() != "Packages:")
	{
		rawStr.removeFirst();
	}

	// parse list step by step
	foreach (QString str, rawStr)
	{
		if (str.endsWith(":")) //packages: pseudopackages: etc
		{
			rawStr.removeOne(str);
			continue;
		}
	list.append(getTrix (str));
	}
}

int WinetricksModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED (parent);
	return list.count();
}

QVariant WinetricksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (section >= columnCount(QModelIndex()) || orientation != Qt::Horizontal)
		return QVariant();
	if (role != Qt::DisplayRole)
		return QVariant();
	switch (section)
	{
	case 0:
		return tr("Name");
	case 1:
		return tr("Description");
	default:
		return QVariant();
	}
}

QVariant WinetricksModel::data(const QModelIndex &index, int role) const
{

	switch (index.column())
	{
	case 0:
		switch (role)
		{
				case Qt::DisplayRole:
			return list.at(index.row()).first;
				case Qt::FontRole:
			QFont f("Times");
			f.setBold(true);
			return f;
		}

	case 1:
		if (role == Qt::DisplayRole)
			return list.at(index.row()).second; //иначе проваливаюсь на default
	default:
		return QVariant();
	}
}

Trix WinetricksModel::getTrix(const QString &str)
{
	QStringList rawData = str.trimmed().split(" ", QString::SkipEmptyParts);
	Trix trix;
	if (rawData.count() < 2)
		return trix;
	trix.first = rawData.takeFirst();
	trix.second = rawData.join(" ");
	return trix;
}

Qt::ItemFlags WinetricksModel::flags(const QModelIndex &index) const
{
	if (index.column() == 0)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	else
		return Qt::ItemIsEnabled;
}
