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

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QLibrary>
#include <QtCore/QtDebug>

#include <QtGui/QWidget>

#include "QtCore/qt_windows.h"

namespace Phonon
{
namespace VLC
{

//Hack, global variable
VLCLoader * VLCLoader::_vlcloader = NULL;

VLCLoader::VLCLoader(QObject * parent) {
	_vlc = new QLibrary(parent);
	_instance = NULL;
}

VLCLoader::~VLCLoader() {
	_vlc->unload();
	delete _vlc;
}

VLCLoader * VLCLoader::get() {
	//Lazy initialization
	if (!_vlcloader) {
		QDir vlcPath(QCoreApplication::applicationDirPath());
		qDebug() << "VLC path=" << vlcPath.exists() << vlcPath.path();
		QDir vlcPluginsPath(vlcPath.path() + "/plugins");
		qDebug() << "VLC plugins path=" << vlcPluginsPath.exists() << vlcPluginsPath.path();

		QFile vlcDll(vlcPath.path() + "/libvlc-control");
		qDebug() << "VLC .dll path=" << vlcDll.exists() << vlcDll.fileName();
		_vlcloader = new VLCLoader(NULL);
		_vlcloader->load(vlcDll.fileName());

		//HINSTANCE pHnd = LoadLibraryA((QDir::toNativeSeparators(QString("\"") + vlcDll.fileName() + QString("\""))).toAscii().data());

		_vlcloader->libvlc_exception_init();

		char * vlcArgc[] = { vlcPath.path().toAscii().data(), "--plugin-path=", vlcPluginsPath.path().toAscii().data() };

		//Init VLC modules, should be done only once
		_vlcloader->libvlc_new(sizeof(vlcArgc) / sizeof(vlcArgc[0]), vlcArgc);
	}

	return _vlcloader;
}

void VLCLoader::checkException() {
	if (libvlc_exception_raised()) {
		qCritical() << "error:" << libvlc_exception_get_message();
	}
}

bool VLCLoader::load(const QString & libname) {
	qDebug() << "VLCLoader=" << libname;
	_vlc->setFileName(libname);
	_vlc->load();
	if (!_vlc->isLoaded()) {
		qDebug() << "libvlc couldn't be loaded:" << _vlc->errorString();
		return false;
	} else {
		return true;
	}
}

void VLCLoader::libvlc_new(int argc, const char * const * argv) {
	typedef libvlc_instance_t * (*fct) (int, const char * const *, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_new");
	if (function) {
		_instance = function(argc, argv, &_exception);
		checkException();
	} else {
	}
}

void VLCLoader::libvlc_exception_init() {
	typedef void (*fct) (libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_exception_init");
	if (function) {
		function(&_exception);
	} else {
	}
}

const char * VLCLoader::libvlc_exception_get_message() {
	typedef const char * (*fct) (const libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_exception_get_message");
	if (function) {
		return function(&_exception);
	} else {
		return NULL;
	}
}

libvlc_media_descriptor_t * VLCLoader::libvlc_media_descriptor_new(const QString & mediaDescriptor) {
	typedef libvlc_media_descriptor_t * (*fct) (libvlc_instance_t *, const char *, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_media_descriptor_new");
	if (function) {
		return function(_instance, mediaDescriptor.toAscii(), &_exception);
	} else {
		return NULL;
	}
}

libvlc_media_instance_t * VLCLoader::libvlc_media_instance_new_from_media_descriptor(libvlc_media_descriptor_t * md) {
	typedef libvlc_media_instance_t * (*fct) (libvlc_media_descriptor_t *, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_media_instance_new_from_media_descriptor");
	if (function) {
		return function(md, &_exception);
		checkException();
	} else {
		return NULL;
	}
}

void VLCLoader::libvlc_media_descriptor_release(libvlc_media_descriptor_t * md) {
	typedef void (*fct) (libvlc_media_descriptor_t *);
	fct function = (fct) _vlc->resolve("libvlc_media_descriptor_release");
	if (function) {
		function(md);
	} else {
	}
}

void VLCLoader::libvlc_media_instance_play(libvlc_media_instance_t * mi) {
	typedef void (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_media_instance_play");
	if (function) {
		function(mi, &_exception);
	} else {
	}
}

void VLCLoader::libvlc_media_instance_pause(libvlc_media_instance_t * mi) {
	typedef void (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_media_instance_pause");
	if (function) {
		function(mi, &_exception);
		checkException();
	} else {
	}
}

void VLCLoader::libvlc_media_instance_stop(libvlc_media_instance_t * mi) {
	typedef void (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_media_instance_stop");
	if (function) {
		function(mi, &_exception);
	} else {
	}
}

void VLCLoader::setDrawableWidget(const QWidget * widget) {
	_drawableWidget = (int) widget->winId();
}

int VLCLoader::getDrawableWidget() const {
	return _drawableWidget;
}

void VLCLoader::libvlc_media_instance_set_drawable(libvlc_media_instance_t * mi, libvlc_drawable_t drawable) {
	typedef void (*fct) (libvlc_media_instance_t *, libvlc_drawable_t, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_media_instance_set_drawable");
	if (function) {
		function(mi, drawable, &_exception);
		checkException();
	} else {
	}
}

void VLCLoader::libvlc_video_set_parent(libvlc_drawable_t drawable) {
	typedef void (*fct) (libvlc_instance_t *, libvlc_drawable_t, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_video_set_parent");
	if (function) {
		function(_instance, drawable, &_exception);
		checkException();
	} else {
	}
}

libvlc_time_t VLCLoader::libvlc_media_instance_get_time(libvlc_media_instance_t * mi) {
	typedef libvlc_time_t (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_media_instance_get_time");
	if (function) {
		libvlc_time_t t = function(mi, &_exception);
		checkException();
		return t;
	} else {
		return 0;
	}
}

void VLCLoader::libvlc_media_instance_release(libvlc_media_instance_t * mi) {
	typedef void (*fct) (libvlc_media_instance_t *);
	fct function = (fct) _vlc->resolve("libvlc_media_instance_release");
	if (function) {
		function(mi);
	} else {
	}
}

void VLCLoader::libvlc_release() {
	typedef void (*fct) (libvlc_instance_t *);
	fct function = (fct) _vlc->resolve("libvlc_release");
	if (function) {
		function(_instance);
		checkException();
	} else {
	}
}

int VLCLoader::libvlc_exception_raised() {
	typedef int (*fct) (const libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_exception_raised");
	if (function) {
		return function(&_exception);
	} else {
		return 0;
	}
}

}}	//Namespace Phonon::VLC
