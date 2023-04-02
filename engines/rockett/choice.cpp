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

#include "common/rect.h"
#include "common/system.h"

#include "rockett/rockett.h"
#include "rockett/choice.h"

namespace Rockett {

Choice::Choice() {
	_archive = nullptr;
}

Choice::~Choice() {
	if (_archive)
		delete _archive;
}

void Choice::onMouseUp(const Common::Point &pos) {
	debug(2, "choice onmouseup");

	if (g_engine->getGameId() == Common::String("rockett_tricky")) {
		Audio::RewindableAudioStream *aiff = g_engine->requestAIFByName("IDL/Whit.PRX", nullptr, "WHI_noteInCan_t.Aif");
		Audio::SoundHandle *handle = new Audio::SoundHandle();
		g_system->getMixer()->playStream(Audio::Mixer::kSFXSoundType, handle, aiff);
	}
}

void Choice::draw() {
	_drawBackgroundImage();
	_drawBalloons();
}

void Choice::_drawBackgroundImage() {
}

void Choice::_drawBalloons() {
}

} // End of namespace Rockett
