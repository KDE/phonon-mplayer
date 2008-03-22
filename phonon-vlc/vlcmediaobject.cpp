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

#include "vlcmediaobject.h"

#include "vlcloader.h"

#include <QtCore/QLibrary>
#include <QtCore/QtDebug>

namespace Phonon
{
namespace VLC
{

VLCMediaObject::VLCMediaObject(QObject * parent)
	: QObject(parent) {

	_vlc = VLCLoader::getVLCLib();
	_exception = VLCLoader::getVLCException();

	_mediaInstance = NULL;
	_mediaInstanceEventManager = NULL;
	_mediaDescriptor = NULL;
	_mediaDescriptorEventManager = NULL;
}

VLCMediaObject::~VLCMediaObject() {
}

void VLCMediaObject::loadMedia(const QString & filename) {
	//Create a new item
	_mediaDescriptor = VLCLoader::get().libvlc_media_descriptor_new(filename);

	//Create a media instance playing environement
	_mediaInstance = libvlc_media_instance_new_from_media_descriptor();

	//Hook into a window
	libvlc_media_instance_set_drawable(VLCLoader::get().getDrawableWidget());

	//No need to keep the media descriptor now
	//libvlc_media_descriptor_release();

	//connectToAllVLCEvents() needs _mediaInstance
	connectToAllVLCEvents();
}

void VLCMediaObject::play() {
	typedef void (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_media_instance_play");
	if (function) {
		function(_mediaInstance, _exception);
		checkException();
	} else {
	}
}

void VLCMediaObject::pause() {
	if (_mediaInstance) {
		typedef void (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_pause");
		if (function) {
			function(_mediaInstance, _exception);
			checkException();
		} else {
		}
	}
}

void VLCMediaObject::stop() {
	if (_mediaInstance) {
		typedef void (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_stop");
		if (function) {
			function(_mediaInstance, _exception);
			checkException();
		}
		//VLC does not send a StoppedState when running libvlc_media_instance_stop() :/
		emit stateChanged(Phonon::StoppedState);
	}
}

void VLCMediaObject::seek(qint64 milliseconds) {
	if (_mediaInstance) {
		typedef libvlc_event_manager_t * (*fct) (libvlc_media_instance_t *, libvlc_time_t, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_set_time");
		if (function) {
			function(_mediaInstance, milliseconds, _exception);
			checkException();
		} else {
		}
	}
}

Phonon::State VLCMediaObject::state() const {
	libvlc_state_t st = libvlc_NothingSpecial;

	if (_mediaInstance) {
		typedef libvlc_state_t (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_get_state");
		if (function) {
			st = function(_mediaInstance, _exception);
			checkException();
		}
	}

	Phonon::State state;

	switch (st) {
	case libvlc_NothingSpecial:
		state = Phonon::LoadingState;
		break;
	case libvlc_Stopped:
		state = Phonon::StoppedState;
		break;
	case libvlc_Opening:
		state = Phonon::LoadingState;
		break;
	case libvlc_Buffering:
		state = Phonon::BufferingState;
		break;
	case libvlc_Ended:
		state = Phonon::StoppedState;
		break;
	case libvlc_Error:
		state = Phonon::ErrorState;
		break;
	case libvlc_Playing:
		state = Phonon::PlayingState;
		break;
	case libvlc_Paused:
		state = Phonon::PausedState;
		break;
	default:
		qCritical() << "error: unknown VLC state=" << st;
		break;
	}

	return state;
}

QString VLCMediaObject::errorString() const {
	return VLCLoader::get().libvlc_exception_get_message();
}

bool VLCMediaObject::hasVideo() const {
	bool hasVideo = false;

	if (_mediaInstance) {
		typedef bool (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_has_vout");

		if (function) {
			hasVideo = function(_mediaInstance, _exception);
			checkException();
		}
	}

	return hasVideo;
}

bool VLCMediaObject::isSeekable() const {
	bool isSeekable = false;

	if (_mediaInstance) {
		typedef bool (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_is_seekable");
		if (function) {
			isSeekable = function(_mediaInstance, _exception);
			checkException();
		}
	}

	return isSeekable;
}

libvlc_media_instance_t * VLCMediaObject::libvlc_media_instance_new_from_media_descriptor() {
	libvlc_media_instance_t * mi = NULL;

	if (_mediaDescriptor) {
		typedef libvlc_media_instance_t * (*fct) (libvlc_media_descriptor_t *, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_new_from_media_descriptor");
		if (function) {
			mi = function(_mediaDescriptor, _exception);
			checkException();
		}
	}

	return mi;
}

void VLCMediaObject::libvlc_media_descriptor_release() {
	if (_mediaDescriptor) {
		typedef void (*fct) (libvlc_media_descriptor_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_descriptor_release");
		if (function) {
			function(_mediaDescriptor);
		} else {
		}
	}
}

void VLCMediaObject::connectToAllVLCEvents() {
	_mediaInstanceEventManager = libvlc_media_instance_event_manager();
	_mediaDescriptorEventManager = libvlc_media_descriptor_event_manager();

	libvlc_event_type_t eventsMediaInstance[] = {
		libvlc_MediaInstancePlayed,
		libvlc_MediaInstancePaused,
		libvlc_MediaInstanceReachedEnd,
		libvlc_MediaInstanceEncounteredError,
		libvlc_MediaInstanceTimeChanged,
		libvlc_MediaInstancePositionChanged,
		libvlc_MediaInstanceSeekableChanged,
		libvlc_MediaInstancePausableChanged,

		//~ libvlc_MediaListItemAdded,
		//~ libvlc_MediaListWillAddItem,
		//~ libvlc_MediaListItemDeleted,
		//~ libvlc_MediaListWillDeleteItem,

		//~ libvlc_MediaListViewItemAdded,
		//~ libvlc_MediaListViewWillAddItem,
		//~ libvlc_MediaListViewItemDeleted,
		//~ libvlc_MediaListViewWillDeleteItem,

		//~ libvlc_MediaListPlayerPlayed,
		//~ libvlc_MediaListPlayerNextItemSet,
		//~ libvlc_MediaListPlayerStopped,

		//~ libvlc_MediaDiscovererStarted,
		//~ libvlc_MediaDiscovererEnded
	};
	int nbEvents = sizeof(eventsMediaInstance) / sizeof(*eventsMediaInstance);
	for (int i = 0; i < nbEvents; i++) {
		libvlc_event_attach(_mediaInstanceEventManager, eventsMediaInstance[i]);
	}

	libvlc_event_type_t eventsMediaDescriptor[] = {
		libvlc_MediaDescriptorMetaChanged,
		libvlc_MediaDescriptorSubItemAdded,
		libvlc_MediaDescriptorDurationChanged,
		//libvlc_MediaDescriptorPreparsedChanged,
		libvlc_MediaDescriptorFreed,
		libvlc_MediaDescriptorStateChanged,
	};
	nbEvents = sizeof(eventsMediaDescriptor) / sizeof(*eventsMediaDescriptor);
	for (int i = 0; i < nbEvents; i++) {
		libvlc_event_attach(_mediaDescriptorEventManager, eventsMediaDescriptor[i]);
	}
}

libvlc_event_manager_t * VLCMediaObject::libvlc_media_instance_event_manager() {
	libvlc_event_manager_t * evt = NULL;

	if (_mediaInstance) {
		typedef libvlc_event_manager_t * (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_event_manager");
		if (function) {
			evt = function(_mediaInstance, _exception);
		}
	}

	return evt;
}

libvlc_event_manager_t * VLCMediaObject::libvlc_media_descriptor_event_manager() {
	libvlc_event_manager_t * evt = NULL;

	if (_mediaDescriptor) {
		typedef libvlc_event_manager_t * (*fct) (libvlc_media_descriptor_t *, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_descriptor_event_manager");
		if (function) {
			evt = function(_mediaDescriptor, _exception);
		}
	}

	return evt;
}

void VLCMediaObject::libvlc_callback(const libvlc_event_t * event, void * user_data) {
	VLCMediaObject * vlcMediaObject = (VLCMediaObject *) user_data;

	qDebug() << "VLC event=" << vlcMediaObject->libvlc_event_type_name(event->type);

	if (event->type == libvlc_MediaInstanceTimeChanged) {
		//new_time / 1000 since VLC adds * 1000, don't know why...
		qDebug() << "new_time=" << event->u.media_instance_time_changed.new_time;
		emit vlcMediaObject->tick(event->u.media_instance_time_changed.new_time / 1000);
	}

	//void stateChanged(Phonon::State newState, Phonon::State oldState);
	//~ Phonon::LoadingState
	//~ Phonon::StoppedState
	//~ Phonon::PlayingState
	//~ Phonon::BufferingState
	//~ Phonon::PausedState
	//~ Phonon::ErrorState

	//~ libvlc_MediaInstancePlayed,
	//~ libvlc_MediaInstancePaused,
	//~ libvlc_MediaInstanceReachedEnd,
	//~ libvlc_MediaInstanceEncounteredError,
	//~ libvlc_MediaInstanceTimeChanged,
	//~ libvlc_MediaInstancePositionChanged,
	//~ libvlc_MediaInstanceSeekableChanged,
	//~ libvlc_MediaInstancePausableChanged,

	//Media instance event

	if (event->type == libvlc_MediaInstancePlayed) {
		emit vlcMediaObject->stateChanged(Phonon::PlayingState);
	}

	if (event->type == libvlc_MediaInstancePaused) {
		emit vlcMediaObject->stateChanged(Phonon::PausedState);
	}

	if (event->type == libvlc_MediaInstanceReachedEnd) {
		emit vlcMediaObject->stateChanged(Phonon::StoppedState);
	}

	//Meta descriptor event

	if (event->type == libvlc_MediaDescriptorDurationChanged) {
		//new_duration / 1000 since VLC adds * 1000, don't know why...
		emit vlcMediaObject->totalTimeChanged(event->u.media_descriptor_duration_changed.new_duration / 1000);
	}

	if (event->type == libvlc_MediaDescriptorMetaChanged) {
		vlcMediaObject->updateMetaData();
	}
}

void VLCMediaObject::updateMetaData() {
	//~ libvlc_meta_Title,
	//~ libvlc_meta_Artist,
	//~ libvlc_meta_Genre,
	//~ libvlc_meta_Copyright,
	//~ libvlc_meta_Album,
	//~ libvlc_meta_TrackNumber,
	//~ libvlc_meta_Description,
	//~ libvlc_meta_Rating,
	//~ libvlc_meta_Date,
	//~ libvlc_meta_Setting,
	//~ libvlc_meta_URL,
	//~ libvlc_meta_Language,
	//~ libvlc_meta_NowPlaying,
	//~ libvlc_meta_Publisher,
	//~ libvlc_meta_EncodedBy,
	//~ libvlc_meta_ArtworkURL,
	//~ libvlc_meta_TrackID

	//~ Phonon::ArtistMetaData
	//~ Phonon::AlbumMetaData
	//~ Phonon::TitleMetaData
	//~ Phonon::DateMetaData
	//~ Phonon::GenreMetaData
	//~ Phonon::TracknumberMetaData
	//~ Phonon::DescriptionMetaData

	QMultiMap<QString, QString> metaDataMap;

	metaDataMap.insert(QLatin1String("ARTIST"), QString::fromUtf8(libvlc_media_descriptor_get_meta(libvlc_meta_Artist)));
	metaDataMap.insert(QLatin1String("ALBUM"), QString::fromUtf8(libvlc_media_descriptor_get_meta(libvlc_meta_Album)));
	metaDataMap.insert(QLatin1String("TITLE"), QString::fromUtf8(libvlc_media_descriptor_get_meta(libvlc_meta_Title)));
	metaDataMap.insert(QLatin1String("DATE"), QString::fromUtf8(libvlc_media_descriptor_get_meta(libvlc_meta_Date)));
	metaDataMap.insert(QLatin1String("GENRE"), QString::fromUtf8(libvlc_media_descriptor_get_meta(libvlc_meta_Genre)));
	metaDataMap.insert(QLatin1String("TRACKNUMBER"), QString::fromUtf8(libvlc_media_descriptor_get_meta(libvlc_meta_TrackNumber)));
	metaDataMap.insert(QLatin1String("DESCRIPTION"), QString::fromUtf8(libvlc_media_descriptor_get_meta(libvlc_meta_Description)));

	metaDataMap.insert(QLatin1String("COPYRIGHT"), QString::fromUtf8(libvlc_media_descriptor_get_meta(libvlc_meta_TrackNumber)));
	metaDataMap.insert(QLatin1String("URL"), QString::fromUtf8(libvlc_media_descriptor_get_meta(libvlc_meta_URL)));
	metaDataMap.insert(QLatin1String("ENCODEDBY"), QString::fromUtf8(libvlc_media_descriptor_get_meta(libvlc_meta_EncodedBy)));

	emit metaDataChanged(metaDataMap);
}

char * VLCMediaObject::libvlc_media_descriptor_get_meta(libvlc_meta_t meta) {
	char * name = NULL;

	if (_mediaDescriptor) {
		typedef char * (*fct) (libvlc_media_descriptor_t *, libvlc_meta_t, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_descriptor_get_meta");
		if (function) {
			name = function(_mediaDescriptor, meta, _exception);
			checkException();
		}
	}

	return name;
}

void VLCMediaObject::libvlc_event_attach(libvlc_event_manager_t * event_manager, libvlc_event_type_t event_type) {
	typedef void (*fct) (libvlc_event_manager_t *, libvlc_event_type_t, libvlc_callback_t, void *, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_event_attach");
	if (function) {
		function(event_manager, event_type, libvlc_callback, this, _exception);
		checkException();
	} else {
	}
}

void VLCMediaObject::libvlc_event_detach(libvlc_event_manager_t * event_manager, libvlc_event_type_t event_type) {
	typedef void (*fct) (libvlc_event_manager_t *, libvlc_event_type_t, libvlc_callback_t, void *, libvlc_exception_t *);
	fct function = (fct) _vlc->resolve("libvlc_event_detach");
	if (function) {
		function(event_manager, event_type, libvlc_callback, this, _exception);
		checkException();
	} else {
	}
}

const char * VLCMediaObject::libvlc_event_type_name(libvlc_event_type_t event_type) {
	typedef const char * (*fct) (libvlc_event_type_t);
	fct function = (fct) _vlc->resolve("libvlc_event_type_name");
	const char * name = NULL;
	if (function) {
		name = function(event_type);
	}
	return name;
}

qint64 VLCMediaObject::totalTime() const {
	libvlc_time_t t = 0;

	if (_mediaInstance) {
		typedef libvlc_time_t (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_get_length");
		if (function) {
			t = function(_mediaInstance, _exception);
			checkException();
		}
	}

	return t;
}

qint64 VLCMediaObject::currentTime() const {
	libvlc_time_t t = 0;

	if (_mediaInstance) {
		typedef libvlc_time_t (*fct) (libvlc_media_instance_t *, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_get_time");
		libvlc_time_t t = 0;
		if (function) {
			t = function(_mediaInstance, _exception);
			checkException();
		}
	}

	return t;
}

void VLCMediaObject::libvlc_media_instance_release() {
	if (_mediaInstance) {
		typedef void (*fct) (libvlc_media_instance_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_release");
		if (function) {
			function(_mediaInstance);
			checkException();
		} else {
		}
	}
}

void VLCMediaObject::checkException() const {
	VLCLoader::get().checkException();
}

void VLCMediaObject::libvlc_media_instance_set_drawable(libvlc_drawable_t drawable) {
	if (_mediaInstance) {
		typedef void (*fct) (libvlc_media_instance_t *, libvlc_drawable_t, libvlc_exception_t *);
		fct function = (fct) _vlc->resolve("libvlc_media_instance_set_drawable");
		if (function) {
			function(_mediaInstance, drawable, _exception);
			checkException();
		} else {
		}
	}
}

}}	//Namespace Phonon::VLC
