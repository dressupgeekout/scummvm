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

#ifndef ROCKETT_XPK_DECODER_H
#define ROCKETT_XPK_DECODER_H

#include "common/util.h"
#include "graphics/surface.h"

#include "rockett/clu.h"

namespace Rockett {

// Forward decl
class XPKEntry;

enum XPKDecoderInstruction {
	XPKINST_REPEAT = 0x0, // Opcode 0 instead of argument 0 ...
	XPKINST_RLENEXT = 0x00, // ... BUT, opcode 0 AND argument 0 is a special case
	XPKINST_REPEAT_END = 0x01,
	XPKINST_BIGREPEAT = 0x1,
	XPKINST_XSKIP = 0x2,
	XPKINST_BIGXSKIP = 0x3,
	XPKINST_RLE = 0x4,
	XPKINST_EOF = 0x41,
	XPKINST_BIGRLE = 0x5,
	XPKINST_DIRECT = 0x6,
	XPKINST_BIGDIRECT = 0x7,
	XPKINST_LINEREPEAT = 0x8,
	XPKINST_LINEREPEAT_END = 0x81,
	XPKINST_BIGLINEREPEAT = 0x9,
	XPKINST_SETXNL = 0xA,
	XPKINST_BIGSETXNL = 0xB,
	XPKINST_RLENL = 0xC,
	XPKINST_BIGRLENL = 0xD,
	XPKINST_DIRECTNL = 0xE,
	XPKINST_BIGDIRECTNL = 0xF,
};
typedef enum XPKDecoderInstruction XPKDecoderInstruction;

class XPKRepeatMarker {
public:
	XPKRepeatMarker();
	~XPKRepeatMarker();

	bool newline;
	uint32 reps;
	uint32 loc;
};

#define XPK_MAX_REPEAT_MARKERS 4

class XPKDecoder {
public:
	XPKDecoder();
	~XPKDecoder();

	Graphics::Surface *decode(XPKEntry *entry, Common::SeekableReadStream *entryFp);

private:
	void _pushRepeatMarker(bool withNewline, uint reps, uint32 loc);
	void _popRepeatMarker();
	XPKRepeatMarker *_currentRepeatMarker();

	uint32 _nReps;
	uint32 _directCounter;
	uint32 _curX;
	uint32 _curY;
	XPKRepeatMarker *_repeatMarkers[XPK_MAX_REPEAT_MARKERS];
	int _repeatMarkerTop;
};

} // End of namespace Rockett

#endif // ROCKETT_XPK_DECODER_H
