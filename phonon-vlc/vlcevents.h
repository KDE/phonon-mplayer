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

#ifndef PHONON_VLC_VLCEVENTS_H
#define PHONON_VLC_VLCEVENTS_H

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
class VLCEvents : public QObject {
	Q_OBJECT
public:

	/**
	 * Singleton.
	 * FIXME Ugly hack to get VLCEvents accessible from everywhere.
	 *
	 * Global variable.
	 */
	static VLCEvents * get();

	void libvlc_event_attach(libvlc_event_type_t event_type);

	void libvlc_event_detach(libvlc_event_type_t event_type);

	const char * libvlc_event_type_name(libvlc_event_type_t event_type);

signals:

	void timeChanged(const libvlc_event_t * event, void * user_data);

private:

	VLCEvents(QObject * parent);
	~VLCEvents();

	static void libvlc_callback(const libvlc_event_t * event, void * user_data);

	/** Hack, global variable. */
	static VLCEvents * _vlcevents;

	libvlc_event_manager_t * _eventManager;
};

}}	//Namespace Phonon::VLC

#endif	//PHONON_VLC_VLCEVENTS_H
