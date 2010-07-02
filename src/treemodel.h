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


#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "prefixcollection.h"
#include "package.h"
#include "plugincore.h"
#include <QIcon>

typedef QList <FormatInterface *> PluginList;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
	explicit TreeModel(QObject *parent, PrefixCollection *pcoll,  PluginList list, bool includeDvdPackages);
	~TreeModel();
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	bool hasChildren(const QModelIndex &parent) const;
	void resetDatas();
	SourceReader * readerFor (const QModelIndex &index);
	void setCollection (PrefixCollection *coll) {delete collection; collection = coll; updateDatas();}

private:
	Package *rootItem;
	//Installed, Pre-Sets & Available
	Package *installed;
	Package *available;
	PrefixCollection *collection;
	corelib *core;
	PluginList plugins;
	bool includeDvd;
	QString iconById (const QString &id) const;
protected:
	Package* getPackage (const QModelIndex &index) const;
	void updateDatas();

	QMap <int, SourceReader *> readers;
};

#endif // TREEMODEL_H
