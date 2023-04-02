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
#include "audio/mixer.h"
#include "common/rect.h"
#include "graphics/surface.h"

#include "rockett/xpk.h"

namespace Rockett {

enum TableauKind {
	kRockettTableauVanilla,		// default options
	kRockettTableauEmpty,			// nothing clickable, full-screen video only
	kRockettTableauMainMenu,	// lots of little clickables in "Housekeeping"
	kRockettTableauNormal,		// watching the story in ordinary gameplay
	kRockettTableauChoice,		// Rockett must make a decision, mood-balloons
	kRockettTableauPlayback,	// TV mode
};

// A Tableau is a single 'screen' and all of its clickable elements. We
// don't call it a "Scene" on purpose because that name is already reserved
// for the main storyline.
class Tableau {
public:
	Tableau();
	virtual ~Tableau();

	void addBackgroundImage(XPK *xpk);

	// A pair of XPKs, one xor the other is shown depending on the mouse
	// position.
	//
	// XXX need to come up with a callback somehow
	void addHighlightable(XPK *normal, XPK *highlighted);
	void addHighlightableByID(uint32 normal, uint32 highlighted);

	void addSoundLoop(Audio::RewindableAudioStream *stream);

	// Event handlers
	virtual void onMouseUp(const Common::Point &pos);
	virtual void onMouseMove(const Common::Point &curPos, const Common::Point &relPos);

private:
	Graphics::Surface *_bgSurface;
	Audio::LoopingAudioStream *_loopingStream;
	Audio::SoundHandle *_loopingHandle;
};

} // End of namespace Rockett

#endif // ROCKETT_TABLEAU_H
