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
public:

	/**
	 * Singleton.
	 * Makes VLCLoader accessible from everywhere.
	 *
	 * Global variable.
	 */
	static VLCLoader & get();
	static QLibrary * getVLCLib();
	static libvlc_exception_t * getVLCException();

	bool load(const QString & libname);

	//Initialization
	void libvlc_new(int argc, const char * const * argv);
	void libvlc_release();

	const char * libvlc_version();

	libvlc_media_descriptor_t * libvlc_media_descriptor_new(const QString & filename);

	//Audio
	int libvlc_audio_get_volume();
	void libvlc_audio_set_volume(int volume);

	//Widget
	void libvlc_video_set_parent(libvlc_drawable_t drawable);
	void setDrawableWidget(const QWidget * widget);
	int getDrawableWidget() const;

	//Exception
	void libvlc_exception_init();
	void checkException();
	const char * libvlc_exception_get_message();

private:

	VLCLoader(QObject * parent);
	~VLCLoader();

	int libvlc_exception_raised();

	/** Hack, global variable. */
	static VLCLoader * _vlcLoader;

	QLibrary * _vlc;

	libvlc_instance_t * _instance;

	libvlc_exception_t * _exception;

	int _drawableWidget;
};

}}	//Namespace Phonon::VLC

#endif	//PHONON_VLC_VLCLOADER_H
