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


#ifndef WINETRICKSMODEL_H
#define WINETRICKSMODEL_H

#include <QtGui>

typedef QPair <QString, QString> Trix;
typedef QList<Trix> QTrixList;
class WinetricksModel : public QAbstractTableModel
{
    Q_OBJECT
public:
	explicit WinetricksModel(QObject *parent, QString winetricks);
	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	int columnCount(const QModelIndex &parent) const {Q_UNUSED(parent); return 2;}
	Qt::ItemFlags flags(const QModelIndex &index) const;
protected:
	QString wtricks;
	void makeData();
	Trix getTrix (const QString &str);
private:
	QTrixList list;
};

#endif // WINETRICKSMODEL_H
