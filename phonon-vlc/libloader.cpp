/*
 * VLC backend for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "libloader.h"

#include <QtCore/QLibrary>
#include <QtCore/QtDebug>

LibLoader::LibLoader(const char * libName, const char * functionToTest) {
	_lib = NULL;
	_libName = strdup(libName);
	_functionToTest = strdup(functionToTest);

	if (!load()) {
		qWarning() << "Cannot find:" << _libName << "on your system, error msg:" << _lib->errorString();
	}
}

LibLoader::~LibLoader() {
	delete _lib;
}

bool LibLoader::load() {
	_lib = new QLibrary();

	int majorversions[] = { 3, 2, -1 };
	_lib->unload();
	_lib->setFileName(QLatin1String(_libName));
	for (uint i = 0; i < sizeof(majorversions) / sizeof(majorversions[0]); ++i) {
		_lib->setFileNameAndVersion(_lib->fileName(), majorversions[i]);
		if (_lib->load() && _lib->resolve(_functionToTest)) {
			return true;
		}

		_lib->unload();
	}

	delete _lib;

	return false;
}

void * LibLoader::resolve(const char * name) {
	void * func = _lib->resolve(name);
	if (!func) {
		qWarning() << "Cannot resolve:" << name << "in library:" << _libName;
	}

	return func;
}
