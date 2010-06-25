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


#include "package.h"

Package::Package(const QString &category, Package *parent)
	:_category(category), parentItem(parent)
{}

Package::Package(Prefix *prefix, Package *parent)
	:_prefix(prefix), parentItem (parent)
{}

void Package::addPackage(Package *package)
{
	if (package->parent() != this)
		package->setParent(this);
	childItems.append(package);
}

Package* Package::child(int row)
{
	return childItems.value(row);
}

int Package::childCount() const
{
	return childItems.count();
}

int Package::row() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<Package*>(this));

	return 0;
}
