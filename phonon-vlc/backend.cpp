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

#include "backend.h"

#include "mediaobject.h"
#include "videowidget.h"
#include "audiooutput.h"

#include "vlcloader.h"
#include "vlc_symbols.h"

#include <QtCore/QByteArray>
#include <QtCore/QSet>
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>

Q_EXPORT_PLUGIN2(phonon_vlc, Phonon::VLC::Backend);

namespace Phonon
{
namespace VLC
{

Backend::Backend(QObject * parent, const QVariantList &)
	: QObject(parent) {

	setProperty("identifier", QLatin1String("phonon_vlc"));
	setProperty("backendName", QLatin1String("VLC"));
	setProperty("backendComment", QLatin1String("VLC plugin for Phonon"));
	setProperty("backendVersion", QLatin1String("0.1"));
	setProperty("backendWebsite", QLatin1String("http://multimedia.kde.org/"));

	//Before everything else
	initLibVLC();

	qDebug() << "Using VLC version:" << libvlc_version();
}

Backend::~Backend() {
	releaseLibVLC();
}

QObject * Backend::createObject(BackendInterface::Class c, QObject * parent, const QList<QVariant> & args) {
	switch (c) {
	case MediaObjectClass:
		return new MediaObject(parent);
	/*case VolumeFaderEffectClass:
		return new VolumeFaderEffect(parent);
	*/
	case AudioOutputClass:
		return new AudioOutput(parent);
	/*case AudioDataOutputClass:
		return new AudioDataOutput(parent);
	case VisualizationClass:
		return new Visualization(parent);
	case VideoDataOutputClass:
		return new VideoDataOutput(parent);
	case EffectClass: {
		Q_ASSERT(args.size() == 1);
		qDebug() << "creating Effect(" << args[0];
		Effect * effect = new Effect(args[0].toInt(), parent);
		if (effect->isValid()) {
			return effect;
		}
		delete effect;
		return NULL;
	}*/
	case VideoWidgetClass: {
		VideoWidget * videoWidget = new VideoWidget(qobject_cast<QWidget *>(parent));
		return videoWidget;
	}
	}
	return NULL;
}

bool Backend::supportsVideo() const {
	return true;
}

bool Backend::supportsOSD() const {
	return true;
}

bool Backend::supportsFourcc(quint32 fourcc) const {
	switch(fourcc) {
	case 0x00000000:
		return true;
	default:
		return false;
	}
}

bool Backend::supportsSubtitles() const {
	return true;
}

QStringList Backend::availableMimeTypes() const {
	if (m_supportedMimeTypes.isEmpty()) {
		//Audio mime types
		m_supportedMimeTypes
			<< "audio/mp3"
			<< "audio/x-mp3"
			<< "audio/wav"

			<< "audio/mpeg"
			<< "audio/x-ms-wma"
			<< "audio/vnd.rn-realaudio"
			<< "audio/x-wav";

		//Video mime types
		m_supportedMimeTypes
			<< "video/mpg"
			<< "video/avi"

			<< "video/mpeg"
			<< "video/mp4"
			<< "video/quicktime"
			<< "video/x-ms-wmv";
	}

	return m_supportedMimeTypes;
}

QList<int> Backend::objectDescriptionIndexes(ObjectDescriptionType type) const {
	QList<int> list;

	/*switch(type) {
	case Phonon::AudioOutputDeviceType:
		break;
	case Phonon::AudioCaptureDeviceType:
		break;
	case Phonon::VideoOutputDeviceType:
		break;
	case Phonon::VideoCaptureDeviceType:
		break;
	case Phonon::VisualizationType:
		break;
	case Phonon::AudioCodecType:
		break;
	case Phonon::VideoCodecType:
		break;
	case Phonon::ContainerFormatType:
		break;
	case Phonon::EffectType:
		break;
	}*/

	return list;
}

QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType type, int index) const {
	QHash<QByteArray, QVariant> ret;

	/*switch (type) {
	case Phonon::AudioOutputDeviceType:
		break;
	case Phonon::AudioCaptureDeviceType:
		break;
	case Phonon::VideoOutputDeviceType:
		break;
	case Phonon::VideoCaptureDeviceType:
		break;
	case Phonon::VisualizationType:
		break;
	case Phonon::AudioCodecType:
		break;
	case Phonon::VideoCodecType:
		break;
	case Phonon::ContainerFormatType:
		break;
	case Phonon::EffectType:
		break;
	}*/

	return ret;
}

bool Backend::startConnectionChange(QSet<QObject *> nodes) {
	Q_UNUSED(nodes);
	// there's nothing we can do but hope the connection changes won't take too long so that buffers
	// would underrun. But we should be pretty safe the way xine works by not doing anything here.
	return true;
}

bool Backend::connectNodes(QObject * _source, QObject * _sink) {
	return true;
}

bool Backend::disconnectNodes(QObject * _source, QObject * _sink) {
	return true;
}

bool Backend::endConnectionChange(QSet<QObject *> nodes) {
	return true;
}

void Backend::freeSoundcardDevices() {
}

QString Backend::toString() const {
	return "VLC Phonon Backend by Tanguy Krotoff <tkrotoff@gmail.com>,"
		"libvlc version=" + QString(libvlc_version());
}

}}	//Namespace Phonon::VLC
