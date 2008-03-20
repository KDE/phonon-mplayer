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

#include "vlcevents.h"

#include "vlcloader.h"

#include <QtCore/QLibrary>
#include <QtCore/QtDebug>

namespace Phonon
{
namespace VLC
{

//Hack, global variable
VLCEvents * VLCEvents::_vlcevents = NULL;

VLCEvents::VLCEvents(QObject * parent)
	: QObject(parent) {

	_eventManager = NULL;
}

VLCEvents::~VLCEvents() {
}

VLCEvents * VLCEvents::get() {
	//Lazy initialization
	if (!_vlcevents) {
		_vlcevents = new VLCEvents(NULL);
		
		TODO
mi = libvlc_media_instance_new (vlc, &ex);
catch ();

em = libvlc_media_instance_event_manager (mi, &ex);

		libvlc_event_type_t events[] = {
			libvlc_MediaDescriptorMetaChanged,
			libvlc_MediaDescriptorSubItemAdded,
			libvlc_MediaDescriptorDurationChanged,
			libvlc_MediaDescriptorPreparsedChanged,
			libvlc_MediaDescriptorFreed,
			libvlc_MediaDescriptorStateChanged,

			libvlc_MediaInstancePlayed,
			libvlc_MediaInstancePaused,
			libvlc_MediaInstanceReachedEnd,
			libvlc_MediaInstanceEncounteredError,
			libvlc_MediaInstanceTimeChanged,
			libvlc_MediaInstancePositionChanged,
			libvlc_MediaInstanceSeekableChanged,
			libvlc_MediaInstancePausableChanged,

			libvlc_MediaListItemAdded,
			libvlc_MediaListWillAddItem,
			libvlc_MediaListItemDeleted,
			libvlc_MediaListWillDeleteItem,

			libvlc_MediaListViewItemAdded,
			libvlc_MediaListViewWillAddItem,
			libvlc_MediaListViewItemDeleted,
			libvlc_MediaListViewWillDeleteItem,

			libvlc_MediaListPlayerPlayed,
			libvlc_MediaListPlayerNextItemSet,
			libvlc_MediaListPlayerStopped,

			libvlc_MediaDiscovererStarted,
			libvlc_MediaDiscovererEnded
		};
		int nbEvents = sizeof(events) / sizeof(*events);
		for (int i = 0; i < nbEvents; i++) {
			_vlcevents->libvlc_event_attach(events[i]);
		}
	}

	return _vlcevents;
}

void VLCEvents::libvlc_callback(const libvlc_event_t * event, void * user_data) {
	qDebug() << "VLC event received=" << _vlcevents->libvlc_event_type_name(event->type);

	if (event->type == libvlc_MediaInstanceTimeChanged) {
		emit _vlcevents->timeChanged(event, user_data);
	}

	if (event->type == libvlc_MediaInstancePaused) {
		emit _vlcevents->timeChanged(event, user_data);
	}
}

void VLCEvents::libvlc_event_attach(libvlc_event_type_t event_type) {
	typedef void (*fct) (libvlc_event_manager_t *, libvlc_event_type_t, libvlc_callback_t, void *, libvlc_exception_t *);
	fct function = (fct) VLCLoader::get()->_vlc->resolve("libvlc_event_attach");
	if (function) {
		function(_eventManager, event_type, libvlc_callback, this, &(VLCLoader::get()->_exception));
		VLCLoader::get()->checkException();
	} else {
	}
}

void VLCEvents::libvlc_event_detach(libvlc_event_type_t event_type) {
	typedef void (*fct) (libvlc_event_manager_t *, libvlc_event_type_t, libvlc_callback_t, void *, libvlc_exception_t *);
	fct function = (fct) VLCLoader::get()->_vlc->resolve("libvlc_event_detach");
	if (function) {
		function(_eventManager, event_type, libvlc_callback, this, &(VLCLoader::get()->_exception));
		VLCLoader::get()->checkException();
	} else {
	}
}

const char * VLCEvents::libvlc_event_type_name(libvlc_event_type_t event_type) {
	typedef const char * (*fct) (libvlc_event_type_t);
	fct function = (fct) VLCLoader::get()->_vlc->resolve("libvlc_event_type_name");
	if (function) {
		const char * name = function(event_type);
		return name;
	} else {
		return NULL;
	}
}

}}	//Namespace Phonon::VLC
