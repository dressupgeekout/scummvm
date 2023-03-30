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

#include "rockett/rockett.h"
#include "rockett/detection.h"
#include "rockett/console.h"
#include "common/scummsys.h"
#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/events.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/palette.h"
#include "graphics/surface.h"
#include "audio/decoders/aiff.h"
#include "audio/audiostream.h"
#include "audio/mixer.h"

#include "common/file.h"

#include "rockett/clu.h"
#include "rockett/presage_archive.h"
#include "rockett/xpk.h"

namespace Rockett {

RockettEngine *g_engine;

RockettEngine::RockettEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
	_gameDescription(gameDesc), _randomSource("Rockett") {
	g_engine = this;
}

RockettEngine::~RockettEngine() {
	delete _screen;
}

uint32 RockettEngine::getFeatures() const {
	return _gameDescription->flags;
}

Common::String RockettEngine::getGameId() const {
	return _gameDescription->gameId;
}

Common::Error RockettEngine::run() {
	initGraphics(640, 480);
	_screen = new Graphics::Screen();

	// Set the engine's debugger console
	setDebugger(new Console());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	//
	// ROCKETT'S NEW SCHOOL: Load the "Housekeeping" main menu and play the
	// background sound
	//
	if (getGameId() == Common::String("rockett_newschool")) {
		PresageArchive *globalArchive = new PresageArchive("GLOBAL.PRD", "GLOBAL.PRS");
		globalArchive->read();

		if (globalArchive->hasFile("HKEEP.CLU")) {
			CLU *clu = new CLU();
			clu->readFromStream(globalArchive->createReadStreamForMember("HKEEP.CLU"));
			useCLU(clu);
			delete clu;
		} else {
			warning("couldn't find expected file HKEEP.CLU");
		}

		if (globalArchive->hasFile("house_01.XPK")) {
			XPK *xpk = new XPK();
			xpk->readFromStream(globalArchive->createReadStreamForMember("house_01.XPK"));
			Graphics::Surface *bgSurface = xpk->decodeTiledMode();
			_screen->blitFrom(bgSurface);
			_screen->update();
		} else {
			warning("couldn't find expected file house_01.XPK");
		}

		if (globalArchive->hasFile("House_amb.AIF")) {
			Common::SeekableReadStream *houseAmbient = globalArchive->createReadStreamForMember("House_amb.AIF");
			Audio::RewindableAudioStream *houseAmbientAiff = Audio::makeAIFFStream(houseAmbient, DisposeAfterUse::YES);
			Audio::LoopingAudioStream *loopingStream = new Audio::LoopingAudioStream(houseAmbientAiff, 0, DisposeAfterUse::YES);
			Audio::SoundHandle *handle = new Audio::SoundHandle();
			debug(2, "Playing House_amb.AIF");
			g_system->getMixer()->playStream(Audio::Mixer::kMusicSoundType, handle, loopingStream);
		}

		delete globalArchive;
	}

	//
	// ROCKETT'S TRICKY DECISION: Load the Hidden Hallway -- Play the 'Hall
	// Loop' music and load the starting location graphic
	//
	if (getGameId() == Common::String("rockett_tricky")) {
		PresageArchive *idnav = new PresageArchive("IDNav.PRX");
		idnav->read();

		if (idnav->hasFile("hallLoop_t7.Aif")) {
			Common::SeekableReadStream *hallLoop = idnav->createReadStreamForMember("hallLoop_t7.Aif");
			Audio::RewindableAudioStream *hallLoopAiff = Audio::makeAIFFStream(hallLoop, DisposeAfterUse::YES);
			Audio::LoopingAudioStream *loopingStream = new Audio::LoopingAudioStream(hallLoopAiff, 0, DisposeAfterUse::YES);
			Audio::SoundHandle *handle = new Audio::SoundHandle();
			debug(2, "Playing hallLoop_t7.Aif");
			g_system->getMixer()->playStream(Audio::Mixer::kMusicSoundType, handle, loopingStream);
		}

		PresageArchive *idglobal = new PresageArchive("IDGlobal.PRX");
		idglobal->read();

		CLU *navClu = new CLU;
		if (idglobal->hasFile("IDPalette.CLU")) {
			navClu->readFromStream(idglobal->createReadStreamForMember("IDPalette.CLU"));
			useCLU(navClu);
			delete navClu;
		}

		XPK *xpk = new XPK();
		if (idnav->hasFile("AN.XPK")) {
			debug(2, "reading AN.XPK");
			xpk->readFromStream(idnav->createReadStreamForMember("AN.XPK"));
			Graphics::Surface *surf = xpk->decodeTiledMode();
			_screen->blitFrom(surf);
			_screen->update();
		} else {
			debug(2, "No such file 'AN.XPK'");
		}

		delete idnav;
		delete idglobal;
	}

	//
	// ROCKETT'S SECRET INVITATION: Load the "Housekeeping" main menu and play
	// the background sound
	//
	if (getGameId() == Common::String("rockett_secret")) {
		PresageArchive *globalArchive = new PresageArchive("GLOBAL.PRX");
		globalArchive->read();

		if (globalArchive->hasFile("Housekeeping.CLU")) {
			CLU *clu = new CLU();
			clu->readFromStream(globalArchive->createReadStreamForMember("Housekeeping.CLU"));
			useCLU(clu);
			delete clu;
		} else {
			warning("couldn't find expected file Housekeeping.CLU");
			return Common::kPathDoesNotExist;
		}

		if (globalArchive->hasFile("house_01.XPK")) {
			XPK *xpk = new XPK();
			xpk->readFromStream(globalArchive->createReadStreamForMember("house_01.XPK"));
			Graphics::Surface *bgSurface = xpk->decodeTiledMode();
			_screen->blitFrom(bgSurface);
			_screen->update();
		} else {
			warning("couldn't find expected file house_01.XPK");
			return Common::kPathDoesNotExist;
		}

		delete globalArchive;

		PresageArchive *ambientArchive = new PresageArchive("AMBIENT.PRX");
		ambientArchive->read();

		if (ambientArchive->hasFile("HOUSEKEEPING_AM.Aif")) {
			Common::SeekableReadStream *housekeeping = ambientArchive->createReadStreamForMember("HOUSEKEEPING_AM.Aif");
			Audio::RewindableAudioStream *housekeepingAiff = Audio::makeAIFFStream(housekeeping, DisposeAfterUse::YES);
			Audio::LoopingAudioStream *loopingStream = new Audio::LoopingAudioStream(housekeepingAiff, 0, DisposeAfterUse::YES);
			Audio::SoundHandle *handle = new Audio::SoundHandle();
			debug(2, "Playing HOUSEKEEPING_AM.Aif");
			g_system->getMixer()->playStream(Audio::Mixer::kMusicSoundType, handle, loopingStream);
		} else {
			warning("couldn't find expected file HOUSEKEEPING_AM.Aif");
			return Common::kPathDoesNotExist;
		}

		delete ambientArchive;
	}

	// Simple event handling loop
	Common::Event e;

	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
		}

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		g_system->delayMillis(10);
	}

	return Common::kNoError;
}

Common::Error RockettEngine::syncGame(Common::Serializer &s) {
	// The Serializer has methods isLoading() and isSaving()
	// if you need to specific steps; for example setting
	// an array size after reading it's length, whereas
	// for saving it would write the existing array's length
	int dummy = 0;
	s.syncAsUint32LE(dummy);

	return Common::kNoError;
}

void RockettEngine::useCLU(CLU *clu) {
	g_system->getPaletteManager()->setPalette(clu->toPalette(), 0, 256);
}

} // End of namespace Rockett
