/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
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
 *
 */

#include "common/system.h"
#include "graphics/surface.h"
#include "audio/audiostream.h"
#include "audio/mixer.h"

#include "rockett/rockett.h"
#include "rockett/tableau.h"

namespace Rockett {

Tableau::Tableau() {
	_bgSurface = nullptr;
	_loopingStream = nullptr;
	_loopingHandle = nullptr;
}

Tableau::~Tableau() {
	_bgSurface->free();
}

void Tableau::addBackgroundImage(XPK *xpk) {
	_bgSurface = xpk->decodeTiledMode();
	g_engine->_screen->blitFrom(_bgSurface);
	g_engine->_screen->update();
}

void Tableau::addHighlightable(XPK *normal, XPK *highlighted) {
}

void Tableau::addHighlightableByID(uint32 normal, uint32 highlighted) {
}

void Tableau::addSoundLoop(Audio::RewindableAudioStream *stream) {
	_loopingStream = new Audio::LoopingAudioStream(stream, 0, DisposeAfterUse::YES);
	_loopingHandle = new Audio::SoundHandle();
	g_system->getMixer()->playStream(Audio::Mixer::kMusicSoundType, _loopingHandle, _loopingStream);
}
	
} // End of namespace Rockett
