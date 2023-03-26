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

#include "common/file.h"

#include "rockett/clu.h"
#include "rockett/presage_archive.h"

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
	// Initialize 320x200 paletted graphics mode
	initGraphics(320, 200);
	_screen = new Graphics::Screen();

	// Set the engine's debugger console
	setDebugger(new Console());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	// ====================

	// CHARLOTTE VALIDATION
	if (Common::String(_gameDescription->gameId) == Common::String("rockett_tricky")) {
		Common::File *fp = new Common::File;
		if (fp->open("600-!Title.CLU")) {
			CLU *clu = new CLU;
			clu->readFromStream(fp);
			clu->dump();
			delete clu;
		}
		fp->close();

		PresageArchive *prx = new PresageArchive("Title.PRX");
		prx->read();
		debug(2, "Title.PRX has %d members", prx->nEntries());
		delete prx;

		PresageArchive *prx2 = new PresageArchive("IDL/Whit.PRX");
		prx2->read();
		debug(2, "IDL/Whit.PRX has %d members", prx2->nEntries());

		Common::ArchiveMemberList list;
		prx2->listMembers(list);

		for (Common::ArchiveMemberList::const_iterator i = list.begin(); i != list.end(); i++) {
			PresageArchiveMember *member = (PresageArchiveMember*)i->get();
			debug(2, "ARCHIVE MEMBER %s", member->getName().c_str());
		}

		delete prx2;

		// Confirm we can instantiate a CLU from _within_ a PRX
		PresageArchive *prx3 = new PresageArchive("Title.PRX");
		prx3->read();

		CLU *titleClu = new CLU;
		if (prx3->hasFile("!Title.CLU")) {
			titleClu->readFromStream(prx3->createReadStreamForMember("!Title.CLU"));
			delete prx3;
			titleClu->dump();
			delete titleClu;
		} else {
			debug(2, "This PRX has no such !Title.CLU");
		}
	}

	if (Common::String(_gameDescription->gameId) == Common::String("rockett_newschool")) {
		PresageArchive *archive1 = new PresageArchive("TITLE.PRD", "TITLE.PRS");
		archive1->read();

		Common::ArchiveMemberList list1;
		archive1->listMembers(list1);
		debug(2, "(PRDPRS) TITLE.PR{D,S} has %d members", archive1->nEntries());

		for (Common::ArchiveMemberList::const_iterator i = list1.begin(); i != list1.end(); i++) {
			PresageArchiveMember *member = (PresageArchiveMember*)i->get();
			debug(2, "(PRDPRDS) ARCHIVE MEMBER %s", member->getName().c_str());
		}

		delete archive1;

		PresageArchive *archive2 = new PresageArchive("IDL/Arro.PRD", "IDL/Arro.PRS");
		archive2->read();

		Common::ArchiveMemberList list2;
		archive2->listMembers(list2);
		debug(2, "(PRDPRS) IDL/Arro.PR{D,S} has %d members", archive2->nEntries());

		for (Common::ArchiveMemberList::const_iterator i = list2.begin(); i != list2.end(); i++) {
			PresageArchiveMember *member = (PresageArchiveMember*)i->get();
			debug(2, "(PRDPRDS) ARCHIVE MEMBER %s", member->getName().c_str());
		}

		delete archive2;
	}

	// ====================

	// Draw a series of boxes on screen as a sample
	for (int i = 0; i < 100; ++i)
		_screen->frameRect(Common::Rect(i, i, 320 - i, 200 - i), i);
	_screen->update();

	// Simple event handling loop
	byte pal[256 * 3] = { 0 };
	Common::Event e;
	int offset = 0;

	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
		}

		// Cycle through a simple palette
		++offset;
		for (int i = 0; i < 256; ++i)
			pal[i * 3 + 1] = (i + offset) % 256;
		g_system->getPaletteManager()->setPalette(pal, 0, 256);
		_screen->update();

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

} // End of namespace Rockett
