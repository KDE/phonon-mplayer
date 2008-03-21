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

#include "mediaobject.h"

#include "vlcloader.h"
#include "vlcevents.h"

namespace Phonon
{
namespace VLC
{

MediaObject::MediaObject(QObject * parent)
	: QObject(parent) {

	_mediaInstance = NULL;
}

MediaObject::~MediaObject() {
}

void MediaObject::play() {
	qDebug() << "MediaObject::play()";

	switch (_mediaSource.type()) {
	case MediaSource::Invalid:
		break;
	case MediaSource::LocalFile: {
		//Create a new item
		libvlc_media_descriptor_t * md = VLCLoader::get()->libvlc_media_descriptor_new(_mediaSource.fileName());

		//Create a media instance playing environement
		_mediaInstance = VLCLoader::get()->libvlc_media_instance_new_from_media_descriptor(md);


		VLCMediaObject * vlcMediaObject = new VLCMediaObject(_mediaInstance, this);

		connect(vlcMediaObject, SIGNAL(timeChanged(qint64)),
			SIGNAL(tick(qint64)));


		//Hook into a window
		VLCLoader::get()->libvlc_media_instance_set_drawable(_mediaInstance, VLCLoader::get()->getDrawableWidget());

		//No need to keep the media descriptor now
		VLCLoader::get()->libvlc_media_descriptor_release(md);

		//Play the media_instance
		VLCLoader::get()->libvlc_media_instance_play(_mediaInstance);

		break;
	}
	case MediaSource::Url:
		break;
	case MediaSource::Disc: {
		switch (_mediaSource.discType()) {
		case Phonon::NoDisc:
			//kFatal(610) << "I should never get to see a MediaSource that is a disc but doesn't specify which one";
			return;
		case Phonon::Cd:
			break;
		case Phonon::Dvd:
			break;
		case Phonon::Vcd:
			break;
		default:
			return;
		}
		break;
	}
	case MediaSource::Stream:
		break;
	}
}

void MediaObject::pause() {
	VLCLoader::get()->libvlc_media_instance_pause(_mediaInstance);
}

void MediaObject::stop() {
	VLCLoader::get()->libvlc_media_instance_stop(_mediaInstance);

	//Free the media_instance
	VLCLoader::get()->libvlc_media_instance_release(_mediaInstance);
	_mediaInstance = NULL;
}

void MediaObject::seek(qint64 milliseconds) {
}

qint32 MediaObject::tickInterval() const {
	return 0;
}

void MediaObject::setTickInterval(qint32 interval) {
}

bool MediaObject::hasVideo() const {
	return true;
}

bool MediaObject::isSeekable() const {
	return true;
}

qint64 MediaObject::currentTime() const {
	return VLCLoader::get()->libvlc_media_instance_get_time(_mediaInstance);
}

Phonon::State MediaObject::state() const {
	return Phonon::StoppedState;
}

QString MediaObject::errorString() const {
	return "";
}

Phonon::ErrorType MediaObject::errorType() const {
	return Phonon::NormalError;
}

qint64 MediaObject::totalTime() const {
	return 0;
}

MediaSource MediaObject::source() const {
	return _mediaSource;
}

void MediaObject::setSource(const MediaSource & source) {
	_mediaSource = source;

	switch (source.type()) {
	case MediaSource::Invalid:
		break;
	case MediaSource::LocalFile:
		break;
	case MediaSource::Url:
		break;
	case MediaSource::Disc: {
		switch (source.discType()) {
		case Phonon::NoDisc:
			//kFatal(610) << "I should never get to see a MediaSource that is a disc but doesn't specify which one";
			return;
		case Phonon::Cd:
			break;
		case Phonon::Dvd:
			break;
		case Phonon::Vcd:
			break;
		default:
			return;
		}
		}
		break;
	case MediaSource::Stream:
		break;
	}
}

void MediaObject::setNextSource(const MediaSource & source) {
	_mediaSource = source;
}

qint32 MediaObject::prefinishMark() const {
	return 0;
}

void MediaObject::setPrefinishMark(qint32) {
}

qint32 MediaObject::transitionTime() const {
	return 0;
}

void MediaObject::setTransitionTime(qint32) {
}

bool MediaObject::hasInterface(Interface iface) const {
	return true;
}

QVariant MediaObject::interfaceCall(Interface iface, int command, const QList<QVariant> & arguments) {
	return new QVariant();
}

}}	//Namespace Phonon::VLC
