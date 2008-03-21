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

#ifndef PHONON_VLC_VLCLOADER_H
#define PHONON_VLC_VLCLOADER_H

#include <vlc/libvlc.h>

#include <QtCore/QObject>
#include <QtCore/QString>

#include "vlcevents.h"

class QLibrary;

namespace Phonon
{
namespace VLC
{

/**
 *
 *
 * @see libvlc.h
 * @author Tanguy Krotoff
 */
class VLCLoader : public QObject {
	Q_OBJECT
	friend class VLCMediaObject;
public:

	/**
	 * Singleton.
	 * FIXME Ugly hack to get VLCLoader accessible from everywhere.
	 *
	 * Global variable.
	 */
	static VLCLoader * get();

	bool load(const QString & libname);

	void libvlc_new(int argc, const char * const * argv);
	void libvlc_release();

	void libvlc_exception_init();

	libvlc_media_descriptor_t * libvlc_media_descriptor_new(const QString & mediaDescriptor);

	libvlc_media_instance_t * libvlc_media_instance_new_from_media_descriptor(libvlc_media_descriptor_t * md);

	void libvlc_media_descriptor_release(libvlc_media_descriptor_t * md);

	void libvlc_media_instance_play(libvlc_media_instance_t * mi);
	void libvlc_media_instance_pause(libvlc_media_instance_t * mi);
	void libvlc_media_instance_stop(libvlc_media_instance_t * mi);

	void setDrawableWidget(const QWidget * widget);
	int getDrawableWidget() const;

	/** FIXME does not work inside VLC!!! */
	void libvlc_media_instance_set_drawable(libvlc_media_instance_t * mi, libvlc_drawable_t drawable);

	void libvlc_video_set_parent(libvlc_drawable_t drawable);

	libvlc_time_t libvlc_media_instance_get_time(libvlc_media_instance_t * mi);

	void libvlc_media_instance_release(libvlc_media_instance_t * mi);

private:

	VLCLoader(QObject * parent);
	~VLCLoader();

	void checkException();

	int libvlc_exception_raised();

	/** Hack, global variable. */
	static VLCLoader * _vlcLoader;

	const char * libvlc_exception_get_message();

	QLibrary * _vlc;

	libvlc_instance_t * _instance;

	libvlc_exception_t _exception;

	int _drawableWidget;
};

}}	//Namespace Phonon::VLC

#endif	//PHONON_VLC_VLCLOADER_H
