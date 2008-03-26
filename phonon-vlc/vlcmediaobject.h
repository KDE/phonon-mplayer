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

#include <phonon/mediaobjectinterface.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMultiMap>

namespace Phonon
{
namespace VLC
{

/**
 * Libvlc MediaObject.
 *
 * Encapsulates vlc MediaObject specific code.
 * Take care of libvlc events via libvlc_callback()
 *
 * @see MediaObject
 * @author Tanguy Krotoff
 */
class VLCMediaObject : public QObject {
	Q_OBJECT
public:

	VLCMediaObject(QObject * parent);
	~VLCMediaObject();

	void loadMedia(const QString & filename);
	void play();
	void pause();
	void stop();
	void seek(qint64 milliseconds);

	bool hasVideo() const;
	bool isSeekable() const;

	Phonon::State state() const;

	qint64 currentTime() const;
	qint64 totalTime() const;

	QString errorString() const;

signals:

	//void aboutToFinish()
	//void bufferStatus(int percentFilled);
	//void currentSourceChanged(const Phonon::MediaSource & newSource);
	//void finished()
	//void hasVideoChanged(bool hasVideo);
	void metaDataChanged(const QMultiMap<QString, QString> & metaData);
	//void prefinishMarkReached(qint32 msecToEnd);
	//void seekableChanged(bool isSeekable);
	void stateChanged(Phonon::State newState);
	void tick(qint64 time);
	void totalTimeChanged(qint64 newTotalTime);

private:

	/**
	 * Connects libvlc_callback() to all vlc events.
	 *
	 * @see libvlc_callback()
	 */
	void connectToAllVLCEvents();

	/**
	 * Retrieves (i.e ARTIST, TITLE, ALBUM...) meta data of a file.
	 */
	void updateMetaData();

	/**
	 * Libvlc callback.
	 *
	 * Receives all vlc events.
	 *
	 * @see connectToAllVLCEvents()
	 * @see libvlc_event_attach()
	 */
	static void libvlc_callback(const libvlc_event_t * event, void * user_data);

	//MediaInstance
	libvlc_event_manager_t * _vlcMediaInstanceEventManager;

	//MediaDescriptor
	libvlc_media_descriptor_t * _vlcMediaDescriptor;
	libvlc_event_manager_t * _vlcMediaDescriptorEventManager;

	//MediaList
	libvlc_media_list_t * _vlcMediaList;
	libvlc_event_manager_t * _vlcMediaListEventManager;

	//MediaListView
	libvlc_media_list_view_t * _vlcMediaListView;
	libvlc_event_manager_t * _vlcMediaListViewEventManager;

	//MediaListPlayer
	libvlc_media_list_player_t * _vlcMediaListPlayer;
	libvlc_event_manager_t * _vlcMediaListPlayerEventManager;

	//MediaDiscoverer
	libvlc_media_discoverer_t * _vlcMediaDiscoverer;
	libvlc_event_manager_t * _vlcMediaDiscovererEventManager;
};

}}	//Namespace Phonon::VLC

#endif	//PHONON_VLC_VLCMEDIAOBJECT_H
