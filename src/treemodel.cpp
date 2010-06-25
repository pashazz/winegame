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

/*! Роли (кастомные) нашего дерева
  32 - ID префикса
  34 - note (описание)
  35 - путь к иконке

  */
#include "treemodel.h"

TreeModel::TreeModel(QObject *parent, PrefixCollection *pcoll) :
	QAbstractItemModel(parent), collection(pcoll), core(pcoll->library())
{
	updateDatas();
}


Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	//не хочу, чтобы заголовки были видны.
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant();
}

Package * TreeModel::getPackage(const QModelIndex &index) const
{ //Спионерено с примеров Qt
	if (index.isValid())
		return static_cast<Package*>(index.internalPointer());
	else
		return rootItem;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{ //спионерено с примеров Qt
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	Package *parentPkg;
	if (!parent.isValid())
		parentPkg = rootItem;
	else
		parentPkg = static_cast<Package*>(parent.internalPointer());

	Package *childItem = parentPkg->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	Package *package = getPackage(index);
	Package *parentPackage = package->parent();
	if ((!parentPackage) || (parentPackage == rootItem))
		return QModelIndex();
	else
		return createIndex(parentPackage->row(), 0, parentPackage);

}

int TreeModel::rowCount(const QModelIndex &parent) const
{
	if (parent.column() > 0)
		return 0;
	Package *parentItem;
	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<Package*>(parent.internalPointer());

	return parentItem->childCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	Package *package = getPackage(index);
	if (!package)
	{
		qDebug() << "BAD: OLOLOLO CANT GET PACKAGE";
		return QVariant();
	}
	switch (role)
	{
	case Qt::DisplayRole: case Qt::EditRole /* что за шит */:
		{
			if (!package->category().isEmpty())
				return package->category();
			if (package->prefix())
				return package->prefix()->name();
		}
	case Qt::DecorationRole:
		{
			if (!package->category().isEmpty())
				return QIcon(":/desktop/winegame.png");
			if (package->havePrefix())
				return QIcon(SourceReader(package->prefix()->ID(), core, qApp).icon());
			else
				return QIcon(":/desktop/winegame.png");
		}
	case 32:
		if (!package->category().isEmpty())
			return QVariant();
		if (package->havePrefix())
			return package->prefix()->ID();
		break;
		case 34:
		if (!package->category().isEmpty())
			return QVariant();
		if (package->havePrefix())
			return package->prefix()->note();
		else
			return QVariant();
		break;
		case 35:
		if (!package->category().isEmpty())
			return QVariant();
		if (package->havePrefix())
			return SourceReader(package->prefix()->ID(), core, qApp).icon();
		else
			return QVariant();
		break;
		default:
		return QVariant();
		//return QAbstractItemModel::data(index, role);

}
}

void TreeModel::updateDatas()
{
	available = new Package(tr("Available applications"));
	installed = new Package (tr("Installed applications"));
	presets = new Package (tr("Templates/Presets"));
	foreach (Prefix *prefix, collection->prefixes())
	{
		//add it into this list
		Package *package = new Package (prefix, installed);
		installed->addPackage(package);
	}
	foreach (QString conf, SourceReader::configurations(core->packageDirs()))
	{
		SourceReader *reader = new SourceReader(conf, core, qApp);
		//construct prefix
		Prefix *prefix = new Prefix(reader->ID(), reader->realName(), reader->realNote(),
									reader->prefixPath(), reader->wine(), qApp, core);
		Package *package = new Package (prefix);
	if (reader->preset())
		presets->addPackage(package);
	else
		available->addPackage(package);
	}
	rootItem = new Package (tr("Applications"));
	if (installed->childCount() > 0)
		rootItem->addPackage(installed);
	if (available->childCount() > 0)
		rootItem->addPackage(available);
}


bool TreeModel::hasChildren(const QModelIndex &parent) const
{
	Package *package = getPackage(parent);
	if (!package)
		return false;
	else
		return (package->childCount() > 0);
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
	return 1; //I know about 1 column
}

void TreeModel::resetDatas()
{
	beginResetModel();
	delete installed;
	delete available;
	delete presets;
	delete rootItem;
	endResetModel();
	updateDatas();
}

TreeModel::~TreeModel()
{
	if (available)
		delete available;
	if (presets)
		delete presets;
	if (installed)
		delete installed;
	if (rootItem)
		delete rootItem;
}
