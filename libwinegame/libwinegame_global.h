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


#ifndef LIBWINEGAME_GLOBAL_H
#define LIBWINEGAME_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WINESTUFFLIBRARY)
#  define WINESTUFFSHARED_EXPORT Q_DECL_EXPORT
#else
#  define  WINESTUFFSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBWINEGAME_GLOBAL_H
