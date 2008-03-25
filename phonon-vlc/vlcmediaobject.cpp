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
#include "vlc_symbols.h"

#include <QtCore/QtDebug>

namespace Phonon
{
namespace VLC
{

//VLC returns a strange position... have to multiply by VLC_POSITION_RESOLUTION
static const int VLC_POSITION_RESOLUTION = 10000;

VLCMediaObject::VLCMediaObject(QObject * parent)
	: QObject(parent) {

	_mediaInstance = NULL;
	_mediaInstanceEventManager = NULL;
	_mediaDescriptor = NULL;
	_mediaDescriptorEventManager = NULL;
	_mediaList = NULL;
	_mediaListEventManager = NULL;
}

VLCMediaObject::~VLCMediaObject() {
}

void VLCMediaObject::loadMedia(const QString & filename) {
	qDebug() << "loadMedia" << (int) this << filename;

	//Create a new item
	_mediaDescriptor = p_libvlc_media_descriptor_new(_instance, filename.toAscii(), _exception);

	//Create a media instance playing environement
	_mediaInstance = p_libvlc_media_instance_new_from_media_descriptor(_mediaDescriptor, _exception);

	//No need to keep the media descriptor now
	//p_libvlc_media_descriptor_release(_mediaDescriptor);

	//connectToAllVLCEvents() needs _mediaInstance
	connectToAllVLCEvents();

	qDebug() << "duration:" << p_libvlc_media_descriptor_get_duration(_mediaDescriptor, _exception);

	updateMetaData();
	emit stateChanged(Phonon::StoppedState);
}

void VLCMediaObject::play() {
	//Hook into a window
	//libvlc_media_instance_set_drawable(_mediaInstance, drawable, _exception);

	p_libvlc_media_instance_play(_mediaInstance, _exception);
	checkException();
}

void VLCMediaObject::pause() {
	p_libvlc_media_instance_pause(_mediaInstance, _exception);
	checkException();
}

void VLCMediaObject::stop() {
	p_libvlc_media_instance_stop(_mediaInstance, _exception);
	checkException();

	//VLC does not send a StoppedState when running libvlc_media_instance_stop() :/
	emit stateChanged(Phonon::StoppedState);
}

void VLCMediaObject::seek(qint64 milliseconds) {
	p_libvlc_media_instance_set_time(_mediaInstance, milliseconds * VLC_POSITION_RESOLUTION, _exception);
	checkException();
}

Phonon::State VLCMediaObject::state() const {
	libvlc_state_t st = p_libvlc_media_instance_get_state(_mediaInstance, _exception);
	checkException();

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
	return p_libvlc_exception_get_message(_exception);
}

bool VLCMediaObject::hasVideo() const {
	bool hasVideo = p_libvlc_media_instance_has_vout(_mediaInstance, _exception);
	checkException();

	return hasVideo;
}

bool VLCMediaObject::isSeekable() const {
	bool isSeekable = p_libvlc_media_instance_is_seekable(_mediaInstance, _exception);
	checkException();

	return isSeekable;
}

void VLCMediaObject::connectToAllVLCEvents() {
	_mediaInstanceEventManager = p_libvlc_media_instance_event_manager(_mediaInstance, _exception);
	_mediaDescriptorEventManager = p_libvlc_media_descriptor_event_manager(_mediaDescriptor, _exception);
	//_mediaListEventManager = p_libvlc_media_list_event_manager(_mediaList, _exception);

	libvlc_event_type_t eventsMediaInstance[] = {
		libvlc_MediaInstancePlayed,
		libvlc_MediaInstancePaused,
		libvlc_MediaInstanceReachedEnd,
		libvlc_MediaInstanceEncounteredError,
		libvlc_MediaInstanceTimeChanged,
		libvlc_MediaInstancePositionChanged,
		libvlc_MediaInstanceSeekableChanged,
		libvlc_MediaInstancePausableChanged,

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
		p_libvlc_event_attach(_mediaInstanceEventManager, eventsMediaInstance[i], libvlc_callback, this, _exception);
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
		p_libvlc_event_attach(_mediaDescriptorEventManager, eventsMediaDescriptor[i], libvlc_callback, this, _exception);
		checkException();
	}
/*
	libvlc_event_type_t eventsMediaList[] = {
		libvlc_MediaListItemAdded,
		libvlc_MediaListWillAddItem,
		libvlc_MediaListItemDeleted,
		libvlc_MediaListWillDeleteItem,
	};
	nbEvents = sizeof(eventsMediaList) / sizeof(*eventsMediaList);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(_mediaListEventManager, eventsMediaList[i], libvlc_callback, this, _exception);
	}
*/
}

void VLCMediaObject::libvlc_callback(const libvlc_event_t * event, void * user_data) {
	VLCMediaObject * vlcMediaObject = (VLCMediaObject *) user_data;

	qDebug() << "event=" << (int) vlcMediaObject << p_libvlc_event_type_name(event->type);

	if (event->type == libvlc_MediaInstanceTimeChanged) {
		//new_time / VLC_POSITION_RESOLUTION since VLC adds * VLC_POSITION_RESOLUTION, don't know why...
		qDebug() << "new_time=" << event->u.media_instance_time_changed.new_time;
		emit vlcMediaObject->tick(event->u.media_instance_time_changed.new_time / VLC_POSITION_RESOLUTION);
		////////BUG POSITION//////////
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
		//new_duration / VLC_POSITION_RESOLUTION since VLC adds * VLC_POSITION_RESOLUTION, don't know why...
		emit vlcMediaObject->totalTimeChanged(event->u.media_descriptor_duration_changed.new_duration / VLC_POSITION_RESOLUTION);
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

	metaDataMap.insert(QLatin1String("ARTIST"), QString::fromUtf8(p_libvlc_media_descriptor_get_meta(_mediaDescriptor, libvlc_meta_Artist, _exception)));
	metaDataMap.insert(QLatin1String("ALBUM"), QString::fromUtf8(p_libvlc_media_descriptor_get_meta(_mediaDescriptor, libvlc_meta_Album, _exception)));
	metaDataMap.insert(QLatin1String("TITLE"), QString::fromUtf8(p_libvlc_media_descriptor_get_meta(_mediaDescriptor, libvlc_meta_Title, _exception)));
	metaDataMap.insert(QLatin1String("DATE"), QString::fromUtf8(p_libvlc_media_descriptor_get_meta(_mediaDescriptor, libvlc_meta_Date, _exception)));
	metaDataMap.insert(QLatin1String("GENRE"), QString::fromUtf8(p_libvlc_media_descriptor_get_meta(_mediaDescriptor, libvlc_meta_Genre, _exception)));
	metaDataMap.insert(QLatin1String("TRACKNUMBER"), QString::fromUtf8(p_libvlc_media_descriptor_get_meta(_mediaDescriptor, libvlc_meta_TrackNumber, _exception)));
	metaDataMap.insert(QLatin1String("DESCRIPTION"), QString::fromUtf8(p_libvlc_media_descriptor_get_meta(_mediaDescriptor, libvlc_meta_Description, _exception)));

	metaDataMap.insert(QLatin1String("COPYRIGHT"), QString::fromUtf8(p_libvlc_media_descriptor_get_meta(_mediaDescriptor, libvlc_meta_TrackNumber, _exception)));
	metaDataMap.insert(QLatin1String("URL"), QString::fromUtf8(p_libvlc_media_descriptor_get_meta(_mediaDescriptor, libvlc_meta_URL, _exception)));
	metaDataMap.insert(QLatin1String("ENCODEDBY"), QString::fromUtf8(p_libvlc_media_descriptor_get_meta(_mediaDescriptor, libvlc_meta_EncodedBy, _exception)));

	emit metaDataChanged(metaDataMap);
}

qint64 VLCMediaObject::totalTime() const {
	libvlc_time_t t = p_libvlc_media_instance_get_length(_mediaInstance, _exception);
	checkException();

	return t;
}

qint64 VLCMediaObject::currentTime() const {
	libvlc_time_t t = p_libvlc_media_instance_get_time(_mediaInstance, _exception);
	checkException();

	return t;
}

}}	//Namespace Phonon::VLC
