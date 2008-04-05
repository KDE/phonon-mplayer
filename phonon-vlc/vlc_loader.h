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

#ifndef PHONON_VLC_VLCLOADER_H
#define PHONON_VLC_VLCLOADER_H

#include <vlc/libvlc.h>

#ifndef KDE4_FOUND
	#include <QtCore/QFuture>
#endif	//!KDE4_FOUND

/** Libvlc instance global variable. */
extern libvlc_instance_t * _vlcInstance;

/** Libvlc exception handling global variable. */
extern libvlc_exception_t * _vlcException;

/** Libvlc media player global variable. */
extern libvlc_media_player_t * _vlcCurrentMediaPlayer;

/** Libvlc widget id global variable, where vlc will show images/movies. */
extern libvlc_drawable_t _vlcMediaPlayerWidgetId;

#ifndef KDE4_FOUND
/** QFuture permits to run initLibVLC() in another thread. Global variable. */
extern QFuture<void> _initLibVLCFuture;
#endif	//!KDE4_FOUND

namespace Phonon
{
namespace VLC
{

/**
 * Checks for a libvlc exception.
 *
 * If a libvlc exception has been raised, shows an error message.
 */
void checkException();

/**
 * Launch and initialize libvlc.
 *
 * _instance and _exception global variables are initialized.
 */
void initLibVLC();

/**
 * Stops libvlc.
 */
void releaseLibVLC();

}}	//Namespace Phonon::VLC

#endif	//PHONON_VLC_VLCLOADER_H