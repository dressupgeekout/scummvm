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
#include "rockett/tableau.h"
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
		Tableau *tableau = new Tableau();

		CLU *clu = requestCLUByName("GLOBAL.PRD", "GLOBAL.PRS", "HKEEP.CLU");
		if (clu) {
			useCLU(clu);
			delete clu;
		} else {
			return Common::kPathDoesNotExist;
		}

		XPK *xpk = requestXPKByName("GLOBAL.PRD", "GLOBAL.PRS", "house_01.XPK");
		if (xpk) {
			tableau->addBackgroundImage(xpk);
		} else {
			return Common::kPathDoesNotExist;
		}

		Audio::RewindableAudioStream *aiff = requestAIFByName("GLOBAL.PRD", "GLOBAL.PRS", "House_amb.AIF");
		if (aiff) {
			tableau->addSoundLoop(aiff);
		} else {
			return Common::kPathDoesNotExist;
		}
	}

	//
	// ROCKETT'S TRICKY DECISION: Load the Hidden Hallway -- Play the 'Hall
	// Loop' music and load the starting location graphic
	//
	if (getGameId() == Common::String("rockett_tricky")) {
		Tableau *tableau = new Tableau();

		CLU *clu = requestCLUByName("IDGlobal.PRX", nullptr, "IDPalette.CLU");
		if (clu) {
			useCLU(clu);
			delete clu;
		} else {
			return Common::kPathDoesNotExist;
		}

		Audio::RewindableAudioStream *aiff = requestAIFByName("IDNav.PRX", nullptr, "hallLoop_t7.Aif");
		if (aiff) {
			tableau->addSoundLoop(aiff);
		} else {
			return Common::kPathDoesNotExist;
		}


		XPK *xpk = requestXPKByName("IDNav.PRX", nullptr, "AN.XPK");
		if (xpk) {
			tableau->addBackgroundImage(xpk);
		} else {
			return Common::kPathDoesNotExist;
		}
	}

	//
	// ROCKETT'S SECRET INVITATION: Load the "Housekeeping" main menu and play
	// the background sound
	//
	if (getGameId() == Common::String("rockett_secret")) {
		Tableau *tableau = new Tableau();

		CLU *clu = requestCLUByName("GLOBAL.PRX", nullptr, "Housekeeping.CLU");
		if (clu) {
			useCLU(clu);
			delete clu;
		} else {
			return Common::kPathDoesNotExist;
		}

		XPK *xpk = requestXPKByName("GLOBAL.PRX", nullptr, "house_01.XPK");
		if (xpk) {
			tableau->addBackgroundImage(xpk);
		} else {
			return Common::kPathDoesNotExist;
		}

		Audio::RewindableAudioStream *aiff = requestAIFByName("AMBIENT.PRX", nullptr, "HOUSEKEEPING_AM.Aif");
		if (aiff) {
			tableau->addSoundLoop(aiff);
		} else {
			return Common::kPathDoesNotExist;
		}
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


CLU *RockettEngine::requestCLUByName(const char *archiveName1, const char *archiveName2, const char *cluName) {
	debug(2, "Requesting CLU '%s' from archive '%s'", cluName, archiveName1);
	PresageArchive *archive;

	if (archiveName2) {
		archive = new PresageArchive(archiveName1, archiveName2);
	} else {
		archive = new PresageArchive(archiveName1);
	}
	archive->read();

	if (archive->hasFile(cluName)) {
		CLU *clu = new CLU();
		clu->readFromStream(archive->createReadStreamForMember(cluName));
		delete archive;
		return clu;
	} else {
		warning("Couldn't find expected file %s in archive %s", cluName, archiveName1);
		return nullptr;
	}
}

XPK *RockettEngine::requestXPKByName(const char *archiveName1, const char *archiveName2, const char *xpkName) {
	debug(2, "Requesting XPK '%s' from archive '%s'", xpkName, archiveName1);
	PresageArchive *archive;

	if (archiveName2) {
		archive = new PresageArchive(archiveName1, archiveName2);
	} else {
		archive = new PresageArchive(archiveName1);
	}
	archive->read();

	if (archive->hasFile(xpkName)) {
		XPK *xpk = new XPK();
		xpk->readFromStream(archive->createReadStreamForMember(xpkName));
		delete archive;
		return xpk;
	} else {
		warning("Couldn't find expected file %s in archive %s", xpkName, archiveName1);
		return nullptr;
	}
}

Audio::RewindableAudioStream *RockettEngine::requestAIFByName(const char *archiveName1, const char *archiveName2, const char *aifName) {
	debug(2, "Requesting AIF '%s' from archive '%s'", aifName, archiveName1);
	PresageArchive *archive;

	if (archiveName2) {
		archive = new PresageArchive(archiveName1, archiveName2);
	} else {
		archive = new PresageArchive(archiveName1);
	}
	archive->read();

	if (archive->hasFile(aifName)) {
		Common::SeekableReadStream *stream = archive->createReadStreamForMember(aifName);
		Audio::RewindableAudioStream *aiffStream = Audio::makeAIFFStream(stream, DisposeAfterUse::YES);
		delete archive;
		return aiffStream;
	} else {
		warning("Couldn't find expected file %s in archive %s", aifName, archiveName1);
		return nullptr;
	}
}

void RockettEngine::useCLU(CLU *clu) {
	g_system->getPaletteManager()->setPalette(clu->toPalette(), 0, 256);
}

void RockettEngine::useTableau(Tableau *tableau) {
	_currentTableau = tableau;
}

} // End of namespace Rockett
