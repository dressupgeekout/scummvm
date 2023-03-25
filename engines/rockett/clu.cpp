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

#include "common/debug.h"
#include "common/stream.h"

#include "rockett/clu.h"

namespace Rockett {

CLU::CLU() {
}

CLU::~CLU() {
}

// The stream is expected to have already been advanced to the appropriate
// position.
bool CLU::readFromStream(Common::SeekableReadStream *stream) {
	byte *cluptr = nullptr;

	// Validate magic
	if (stream->readUint32BE() != 0x00000000)
		return false;
	if (stream->readUint32BE() != 0x800000FF)
		return false;

	for (int i = 0; i < kCluMaxElements; i++) {
		memset(_array[i], 0, sizeof(_array[i]));
		cluptr = _array[i];

		stream->skip(2); // Skip marker
		*cluptr = stream->readByte(); // R
		cluptr++; 

		stream->skip(1); 
		*cluptr = stream->readByte(); // G
		cluptr++;

		stream->skip(1);
		*cluptr = stream->readByte(); // B

		stream->skip(1); // Go to next marker
	}

	return true;
}

byte *CLU::colorAtIndex(byte index) {
	return _array[index];
}

void CLU::dump() {
	for (int i = 0; i < kCluMaxElements; i++) {
		debug(2, "<%d,%d,%d>", _array[i][0], _array[i][1], _array[i][2]);
	}
}

} // End of namespace Rockett
