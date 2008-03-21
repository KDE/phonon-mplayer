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
	_vlcMediaObject = NULL;
}

MediaObject::~MediaObject() {
}

void MediaObject::play() {
	qDebug() << "MediaObject::play()";

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
			return;
		}
		break;
	}

	case MediaSource::Stream:
		break;
	}
}

void MediaObject::playInternal(const QString & filename) {
	if (_currentState == Phonon::PlayingState || _currentState == Phonon::PausedState) {
		resume();
	}

	else {
		//Delete previous _vlcMediaObject
		delete _vlcMediaObject;

		_vlcMediaObject = new VLCMediaObject(filename, this);

		connect(_vlcMediaObject, SIGNAL(tick(qint64)),
			SIGNAL(tick(qint64)));
		connect(_vlcMediaObject, SIGNAL(stateChanged(Phonon::State)),
			SLOT(stateChangedInternal(Phonon::State)));
		connect(_vlcMediaObject, SIGNAL(totalTimeChanged(qint64)),
			SIGNAL(totalTimeChanged(qint64)));

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
	if (_vlcMediaObject) {
		return _vlcMediaObject->hasVideo();
	} else {
		return false;
	}
}

bool MediaObject::isSeekable() const {
	if (_vlcMediaObject) {
		return _vlcMediaObject->isSeekable();
	} else {
		return false;
	}
}

qint64 MediaObject::currentTime() const {
	if (_vlcMediaObject) {
		return _vlcMediaObject->currentTime();
	} else {
		return 0;
	}
}

Phonon::State MediaObject::state() const {
	if (_vlcMediaObject) {
		return _vlcMediaObject->state();
	} else {
		return _currentState;
	}
}

QString MediaObject::errorString() const {
	if (_vlcMediaObject) {
		return _vlcMediaObject->errorString();
	} else {
		return "";
	}
}

Phonon::ErrorType MediaObject::errorType() const {
	return Phonon::NormalError;
}

qint64 MediaObject::totalTime() const {
	if (_vlcMediaObject) {
		return _vlcMediaObject->totalTime();
	} else {
		//No media
		return 0;
	}
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

void MediaObject::stateChangedInternal(Phonon::State newState) {
	Phonon::State previousState = _currentState;
	_currentState = newState;
	emit stateChanged(_currentState, previousState);
}

}}	//Namespace Phonon::VLC
