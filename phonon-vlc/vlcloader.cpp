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

#include "vlcloader.h"

#include "vlc_symbols.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QLibrary>
#include <QtCore/QtDebug>

#include <QtGui/QWidget>

namespace Phonon
{
namespace VLC
{

//Global variables
libvlc_instance_t * _instance = NULL;
libvlc_exception_t * _exception = new libvlc_exception_t();

void initLibVLC() {
	QString vlcPath(QCoreApplication::applicationDirPath());
	QString vlcPluginsPath(vlcPath + "/plugins");
	const char * vlcArgc[] = { vlcPath.toAscii().constData(), "--plugin-path=", vlcPluginsPath.toAscii().constData() };

	p_libvlc_exception_init(_exception);

	//Init VLC modules, should be done only once
	_instance = p_libvlc_new(sizeof(vlcArgc) / sizeof(*vlcArgc), vlcArgc, _exception);
	checkException();
}

void checkException() {
	if (p_libvlc_exception_raised(_exception)) {
		qCritical() << "libvlc error:" << p_libvlc_exception_get_message(_exception);
	}
}

const char * libvlc_version() {
	static const char * version = NULL;

	if (!version) {
		QLibrary vlcOldAPI;
		vlcOldAPI.setFileName(QCoreApplication::applicationDirPath() + "/libvlc");
		vlcOldAPI.load();

		typedef char const * (*fct) (void);
		fct function = (fct) vlcOldAPI.resolve("VLC_Version");

		if (function) {
			version = function();
		}

		vlcOldAPI.unload();
	}

	return version;
}

}}	//Namespace Phonon::VLC
