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

#ifndef PHONON_VLC_VLCMEDIAOBJECT_H
#define PHONON_VLC_VLCMEDIAOBJECT_H

#include <vlc/libvlc.h>

#include <QtCore/QObject>
#include <QtCore/QString>

class QLibrary;

namespace Phonon
{
namespace VLC
{

/**
 * Libvlc callbacks.
 *
 * @see libvlc.h
 * @author Tanguy Krotoff
 */
class VLCMediaObject : public QObject {
	Q_OBJECT
public:

	VLCMediaObject(libvlc_media_instance_t * mediaInstance, QObject * parent);
	~VLCMediaObject();

signals:

	void timeChanged(qint64 time);

private:

	void connectToAllVLCEvents();

	libvlc_event_manager_t * libvlc_media_instance_event_manager();

	void libvlc_event_attach(libvlc_event_type_t event_type);

	void libvlc_event_detach(libvlc_event_type_t event_type);

	const char * libvlc_event_type_name(libvlc_event_type_t event_type);

	static void libvlc_callback(const libvlc_event_t * event, void * user_data);

	libvlc_event_manager_t * _eventManager;

	libvlc_media_instance_t * _mediaInstance;
};

}}	//Namespace Phonon::VLC

#endif	//PHONON_VLC_VLCMEDIAOBJECT_H
