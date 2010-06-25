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


#ifndef PACKAGE_H
#define PACKAGE_H
#include "prefix.h"

class Package
{
public:
	Package(Prefix *prefix, Package *parent = 0); //For package item
	Package (const QString &category, Package *parent = 0); //for category item
	virtual ~Package() {}
	void addPackage (Package *package);
	Package *child (int row);
	int childCount() const;
	int columnCount() const {return 1;}
	 QVariant data(int column) const;
	 int row() const;
	 Package *parent() {return parentItem;}

	 //Access func
	 Prefix*  prefix () {return _prefix;}
	 QString category () {return _category;}
	 bool havePrefix () {return _prefix != 0;}
	 void setParent (Package *parent) {parentItem = parent;}
 private:
	 QList<Package*> childItems;
	 Package *parentItem;
	 Prefix *_prefix;
	 QString _category;
};

#endif // PACKAGE_H
