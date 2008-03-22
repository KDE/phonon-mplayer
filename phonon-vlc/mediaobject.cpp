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

#include "vlcmediaobject.h"

#include <QtCore/QUrl>

namespace Phonon
{
namespace VLC
{

MediaObject::MediaObject(QObject * parent)
	: QObject(parent) {

	_currentState = Phonon::LoadingState;

	_vlcMediaObject = new VLCMediaObject(this);

	connect(_vlcMediaObject, SIGNAL(tick(qint64)),
		SIGNAL(tick(qint64)));
	connect(_vlcMediaObject, SIGNAL(stateChanged(Phonon::State)),
		SLOT(stateChangedInternal(Phonon::State)));
	connect(_vlcMediaObject, SIGNAL(totalTimeChanged(qint64)),
		SIGNAL(totalTimeChanged(qint64)));
	connect(_vlcMediaObject, SIGNAL(metaDataChanged(const QMultiMap<QString, QString> &)),
		SIGNAL(metaDataChanged(const QMultiMap<QString, QString> &)));
}

MediaObject::~MediaObject() {
	delete _vlcMediaObject;
}

void MediaObject::play() {
	switch (_mediaSource.type()) {

	case MediaSource::Invalid:
		break;

	case MediaSource::LocalFile:
		playInternal(_mediaSource.fileName());
		break;

	case MediaSource::Url:
		playInternal(_mediaSource.url().toString());
		break;

	case MediaSource::Disc: {
		switch (_mediaSource.discType()) {
		case Phonon::NoDisc:
			//kFatal(610) << "I should never get to see a MediaSource that is a disc but doesn't specify which one";
			return;
		case Phonon::Cd:
			playInternal(_mediaSource.deviceName());
			break;
		case Phonon::Dvd:
			playInternal(_mediaSource.deviceName());
			break;
		case Phonon::Vcd:
			playInternal(_mediaSource.deviceName());
			break;
		default:
			qCritical() << __FUNCTION__ << "error: unsupported MediaSource::Disc";
		}
		break;
	}

	case MediaSource::Stream:
		break;

	default:
		qCritical() << __FUNCTION__ << "error: unsupported MediaSource";
		break;

	}
}

void MediaObject::loadMediaInternal(const QString & filename) {
	//Loads the media_instance
	_vlcMediaObject->loadMedia(filename);
}

void MediaObject::playInternal(const QString & filename) {
	loadMediaInternal(filename);

	if (_currentState == Phonon::PlayingState || _currentState == Phonon::PausedState) {
		resume();
	}

	else {
		//Play the media_instance
		_vlcMediaObject->play();
	}
}

void MediaObject::resume() {
	pause();
}

void MediaObject::pause() {
	_vlcMediaObject->pause();
}

void MediaObject::stop() {
	_vlcMediaObject->stop();
}

void MediaObject::seek(qint64 milliseconds) {
	_vlcMediaObject->seek(milliseconds);
}

qint32 MediaObject::tickInterval() const {
	return 1000;
}

void MediaObject::setTickInterval(qint32 interval) {
}

bool MediaObject::hasVideo() const {
	return _vlcMediaObject->hasVideo();
}

bool MediaObject::isSeekable() const {
	return _vlcMediaObject->isSeekable();
}

qint64 MediaObject::currentTime() const {
	return _vlcMediaObject->currentTime();
}

Phonon::State MediaObject::state() const {
	return _vlcMediaObject->state();
}

QString MediaObject::errorString() const {
	return _vlcMediaObject->errorString();
}

Phonon::ErrorType MediaObject::errorType() const {
	return Phonon::NormalError;
}

qint64 MediaObject::totalTime() const {
	return _vlcMediaObject->totalTime();
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
		loadMediaInternal(_mediaSource.fileName());
		break;
	case MediaSource::Url:
		loadMediaInternal(_mediaSource.url().toString());
		break;
	case MediaSource::Disc: {
		switch (source.discType()) {
		case Phonon::NoDisc:
			//kFatal(610) << "I should never get to see a MediaSource that is a disc but doesn't specify which one";
			return;
		case Phonon::Cd:
			loadMediaInternal(_mediaSource.deviceName());
			break;
		case Phonon::Dvd:
			loadMediaInternal(_mediaSource.deviceName());
			break;
		case Phonon::Vcd:
			loadMediaInternal(_mediaSource.deviceName());
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

void MediaObject::stateChangedInternal(Phonon::State newState) {
	Phonon::State previousState = _currentState;
	_currentState = newState;
	emit stateChanged(_currentState, previousState);
}

}}	//Namespace Phonon::VLC
