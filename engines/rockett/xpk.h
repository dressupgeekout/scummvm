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

#ifndef ROCKETT_XPK_H
#define ROCKETT_XPK_H

#include "common/array.h"
#include "common/stream.h"
#include "graphics/surface.h"

#define XPK_TILED_TILE_COUNT 64
#define XPK_TILED_TILE_WIDTH 80
#define XPK_TILED_TILE_HEIGHT 60

namespace Rockett {

class XPKEntry {
public:
	XPKEntry();
	XPKEntry(uint32 width, uint32 height, uint32 x, uint32 y);
	~XPKEntry();

	Common::String toPrettyString();
	Graphics::Surface *decode(Common::SeekableReadStream *entryFp);

	void setOffset(uint32 offset);
	void setWidth(uint16 width);
	void setHeight(uint16 height);
	void setX(uint16 x);
	void setY(uint16 y);

	uint32 getOffset() const;
	uint16 getWidth() const;
	uint16 getHeight() const;
	uint16 getX() const;
	uint16 getY() const;

private:
	uint32 _offset; // Absolute start location within the XPK file
	uint16 _width;
	uint16 _height;
	uint16 _x;
	uint16 _y;
};

typedef Common::SharedPtr<XPKEntry> XPKEntryPtr;

class XPK {
public:
	XPK();
	~XPK();

	bool readFromStream(Common::SeekableReadStream *stream);

	Graphics::Surface *decodeTiledMode();
	Graphics::Surface *decodeEntry(int index);
	bool isTiledModeOK();

	uint16 nEntries() const;
	Common::Array<XPKEntry *> _entries;

private:
	Common::SeekableReadStream *_stream;
	uint16 _nEntries;
	Common::Array<uint32> _offsets;
};

} // End of namespace Rockett

#endif // ROCKETT_XPK_H
