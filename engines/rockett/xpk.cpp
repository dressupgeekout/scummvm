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

#include "common/array.h"
#include "common/debug.h"
#include "common/rect.h"
#include "common/stream.h"
#include "graphics/surface.h"

#include "rockett/xpk_decoder.h"

#include "rockett/xpk.h"

namespace Rockett {

XPKEntry::XPKEntry() {
	_width = 0;
	_height = 0;
	_x = 0;
	_y = 0;
}

XPKEntry::XPKEntry(uint32 width, uint32 height, uint32 x, uint32 y) {
	_width = width;
	_height = height;
	_x = x;
	_y = y;
}

XPKEntry::~XPKEntry() {
	// OK
}

Common::String XPKEntry::toPrettyString() {
  return Common::String::format("#<XPKEntry %dx%d @ %d,%d>",
		_width, _height, _x, _y);
}

Graphics::Surface *XPKEntry::decode(Common::SeekableReadStream *entryFp)
{
	XPKDecoder *decoder = new XPKDecoder();
	Graphics::Surface *surf = decoder->decode(this, entryFp);
	delete decoder;
	return surf;
}

void XPKEntry::setOffset(uint32 offset) {
	_offset = offset;
}

void XPKEntry::setWidth(uint16 width) {
	_width = width;
}

void XPKEntry::setHeight(uint16 height) {
	_height = height;
}

void XPKEntry::setX(uint16 x) {
	_x = x;
}

void XPKEntry::setY(uint16 y) {
	_y = y;
}

uint32 XPKEntry::getOffset() const {
	return _offset;
}

uint16 XPKEntry::getWidth() const {
	return _width;
}

uint16 XPKEntry::getHeight() const {
	return _height;
}

uint16 XPKEntry::getX() const {
	return _x;
}

uint16 XPKEntry::getY() const {
	return _y;
}

// ===============================

XPK::XPK() {
	// OK
}

XPK::~XPK() {
	// OK
}

// Read the XPK file and figure out where all of the individual XPKEntries
// are inside.
bool XPK::readFromStream(Common::SeekableReadStream *stream) {
	_stream = stream;

	// Validate magic
	if (stream->readUint32BE() != 0xa57e7001)
		return false;
	if (stream->readUint16BE() != 0xc002)
		return false;

	_nEntries = stream->readUint16BE();
	debug(2, "XPK has %d entries", _nEntries);

	stream->skip(8);

	// Allocate space for all the entries
	_entries = Common::Array<XPKEntry *>(_nEntries);

	for (int i = 0; i < _nEntries; i++) {
		stream->skip(8); // Ignore first 4 numbers ("x1", "x2", "w1", "w2")

		uint16 x = stream->readUint16BE();
		uint16 y = stream->readUint16BE();
		uint16 width = stream->readUint16BE();
		uint16 height = stream->readUint16BE();

		XPKEntry *entry = new XPKEntry(width, height, x, y);

		if (!entry)
			return false;

		_entries.insert_at(i, entry);
	}

	// The offsets which are listed in the XPK file are not absolute, they are
	// relative to this position right here. But we'll go ahead and store the
	// absolute offsets in the XPK struct we're trying to create.
	//
	// While generating this list, we'll enrich each of the XPKEntries we
	// created earlier with this new info.
	uint32 offsetsStartPos = stream->pos();

	_offsets = Common::Array<uint32>(_nEntries);

	for (int i = 0; i < _nEntries; i++) {
		uint32 offset = stream->readUint32BE();
		uint32 absoluteOffset = offsetsStartPos + offset;
		_offsets[i] = absoluteOffset;
		_entries[i]->setOffset(absoluteOffset);
	}

	return true;
}

// Returns a Surface 640x480 in size. The whole thing is going to be in
// memory (>= 1200 KB). The caller is expected to free the result.
Graphics::Surface *XPK::decodeTiledMode() {
	const uint totalW = 640;
	const uint totalH = 480;
	const uint tileW = 80;
	const uint tileH = 60;
	const uint tileCountX = totalW/tileW; // Number of tiles in the X direction
	const uint tileCountY = totalH/tileH; // Number of tiles in the Y direction
	uint completeCurX = 0;
	uint completeCurY = 0;

	// Create a "transparent canvas" to start with
	Graphics::Surface *surface = new Graphics::Surface();
	surface->create(totalW, totalH, Graphics::PixelFormat::createFormatCLUT8());
	surface->fillRect(Common::Rect(0, 0, totalW, totalH), 0);

	// (1) For each tile...
	for (uint tileIndex = 0; tileIndex < (tileCountX*tileCountY); tileIndex++) {
		Graphics::Surface *tileSurface = this->decodeEntry(tileIndex);

		// The X and Y coordinates of the top-left corner of the current tile,
		// in terms of the complete picture.
		completeCurX = tileW * (tileIndex % tileCountX);
		completeCurY = tileH * (tileIndex / tileCountY);

		// (2) ...splat its contents into the correct spot in the grander image.
		surface->copyRectToSurface(*tileSurface, completeCurX, completeCurY, Common::Rect(0, 0, tileW, tileH));
	}

	return surface;
}

Graphics::Surface *XPK::decodeEntry(int index) {
	XPKEntry *entry = _entries[index];
	return entry->decode(_stream);
}

bool XPK::isTiledModeOK() {
	if (_nEntries != XPK_TILED_TILE_COUNT) {
		return false;
	} else {
		for (int i = 0; i < XPK_TILED_TILE_COUNT; i++) {
			int thisW = _entries[i]->getWidth();
			int thisH = _entries[i]->getHeight();
			if ((thisW != XPK_TILED_TILE_WIDTH) || (thisH != XPK_TILED_TILE_HEIGHT)) {
				return false;
			}
		}
	}

	return true;
}

uint16 XPK::nEntries() const {
	return _nEntries;
}

} // End of namespace Rockett
