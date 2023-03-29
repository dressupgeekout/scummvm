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

#ifndef ROCKETT_TABLEAU_H
#define ROCKETT_TABLEAU_H

#include "audio/audiostream.h"
#include "graphics/surface.h"

#include "rockett/xpk.h"

namespace Rockett {

// A Tableau is a single 'screen' and all of its clickable elements. We
// don't call it a "Scene" on purpose because that name is already reserved
// for the main storyline.
class Tableau {
public:
	Tableau();
	~Tableau();

	void addBackgroundImage(const Common::String &filename);

	// A pair of XPKs, one xor the other is shown depending on the mouse
	// position.
	//
	// XXX need to come up with a callback somehow
	void addHighlightable(const Common::String &normal, const Common::String &highlighted);
	void addHighlightableByID(uint32 normal, uint32 highlighted);

	void addSoundLoop(const Common::String &filename);

private:
	Graphics::Surface *_bgSurface;
	Audio::LoopingAudioStream *_loopingStream;
};

} // End of namespace Rockett

#endif // ROCKETT_TABLEAU_H
