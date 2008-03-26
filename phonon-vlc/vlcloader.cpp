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

libvlc_instance_t * _vlcInstance = NULL;
libvlc_exception_t * _vlcException = new libvlc_exception_t();
libvlc_media_instance_t * _vlcMediaInstance = NULL;
libvlc_drawable_t _vlcMediaInstanceWidgetId = 0;

namespace Phonon
{
namespace VLC
{

void initLibVLC() {
	//Global variables
	_vlcInstance = NULL;
	_vlcException = new libvlc_exception_t();

	QString vlcPath(QCoreApplication::applicationDirPath());
	QString vlcPluginsPath(vlcPath + "/plugins");
	const char * vlcArgc[] = { vlcPath.toAscii().constData(), "--plugin-path=", vlcPluginsPath.toAscii().constData() };

	p_libvlc_exception_init(_vlcException);

	//Init VLC modules, should be done only once
	_vlcInstance = p_libvlc_new(sizeof(vlcArgc) / sizeof(*vlcArgc), vlcArgc, _vlcException);
	checkException();
}

void releaseLibVLC() {
	p_libvlc_release(_vlcInstance);
	unloadLibVLC();
}

void checkException() {
	if (p_libvlc_exception_raised(_vlcException)) {
		qDebug() << "libvlc exception:" << p_libvlc_exception_get_message(_vlcException);
	}
}

const char * libvlc_version() {
	static const char * version = NULL;

	if (!version) {
		//Uses libvlc old API, there no libvlc_get_version() is the new API
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
