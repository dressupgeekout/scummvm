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

#include "common/file.h"
#include "common/rect.h"
#include "graphics/surface.h"

#include "rockett/xpk_decoder.h"
#include "rockett/xpk.h"

namespace Rockett {

XPKRepeatMarker::XPKRepeatMarker() {
	this->newline = false;
	this->reps = 0;
	this->loc = 0L;
}

XPKRepeatMarker::~XPKRepeatMarker() {
	// OK
}

XPKDecoder::XPKDecoder() {
	_nReps = 0;
	_directCounter = 0;
	_curX = 0;
	_curY = 0;
	_repeatMarkerTop = -1;

	for (int i = 0; i < XPK_MAX_REPEAT_MARKERS; i++) {
		_repeatMarkers[i] = nullptr;
	}
}

XPKDecoder::~XPKDecoder() {
	// OK
}

#define NEWLINE()        \
	do {                   \
		_curX = 0; _curY++;  \
	} while (0)

// You need to free the result
Graphics::Surface *XPKDecoder::decode(XPKEntry *entry, Common::SeekableReadStream *entryFp) {
	byte _byte = 0;
	byte instruction = 0;
	byte argument = 0;
	byte color = 0;

	// Create a "transparent canvas" to start with
	Graphics::Surface *surface = new Graphics::Surface();
	surface->create(entry->getWidth(), entry->getHeight(), Graphics::PixelFormat::createFormatCLUT8());
	surface->fillRect(Common::Rect(0, 0, entry->getWidth(), entry->getHeight()), 0);

	entryFp->seek(entry->getOffset(), SEEK_SET);

	while (!(_byte == XPKINST_EOF && _nReps)) {
		_byte = entryFp->readByte();

#ifdef LUNAPURPURA_XPK_HACK
			// XXX HACK!!
			if (_curY >= entry->getHeight()) {
				_curY = entry->getHeight() - 1;
			}
#endif

		if (_directCounter > 0) { // DIRECT MODE ENABLED
			color = _byte;
			surface->setPixel(_curX, _curY, color);
			_curX++;
			_directCounter--;
		} else { // DIRECT MODE OFF OR EXPIRED
			if (_nReps > 0) { // RLE ENGAGED
				color = _byte;
				for (; _nReps > 0; _nReps--) {
					surface->setPixel(_curX, _curY, color);
					_curX++;
				}
			} else { // NEED A NEW INSTRUCTION
				instruction = _byte >> 4; // First 4 bits
				argument = _byte & 0x0F; // Last 4 bits

				switch (instruction) {
				case XPKINST_REPEAT:
					{
						switch (_byte) {
						case XPKINST_RLENEXT:
							_byte = entryFp->readByte();
							_nReps = _byte;
							break;
						case XPKINST_REPEAT_END:
							{
								XPKRepeatMarker *marker = _currentRepeatMarker();
								if (marker->reps > 0) {
									marker->reps--;
								}
								if (marker->reps > 0) {
									entryFp->seek(marker->loc);
								} else {
									_popRepeatMarker();
								}
							}
							break;
						default:
							_pushRepeatMarker(false, argument, entryFp->pos());
						}
					}
					break;
				case XPKINST_BIGREPEAT:
					_pushRepeatMarker(false, 16+argument, entryFp->pos());
					break;
				case XPKINST_XSKIP:
					if (argument) {
						_curX += argument;
					} else {
						_curX += entryFp->readUint16BE();
					}
					break;
				case XPKINST_BIGXSKIP:
					_curX += (16 + argument);
					break;
				case XPKINST_RLE:
					// 0x40 should be safe to ignore
					_nReps = argument;
					break;
				case XPKINST_BIGRLE:
					_nReps = (16 + argument);
					break;
				case XPKINST_DIRECT:
					{
						if (argument) {
							_directCounter = argument;
						} else {
							_directCounter = entryFp->readUint16BE();
						}
					}
					break;
				case XPKINST_BIGDIRECT:
					_directCounter = (16 + argument);
					break;
				case XPKINST_LINEREPEAT:
					{
						switch (_byte) {
						case XPKINST_LINEREPEAT_END:
							{
								XPKRepeatMarker *marker = _currentRepeatMarker();
								if (marker->reps > 0) {
									marker->reps--;
								}
								if (marker->reps > 0) {
									entryFp->seek(marker->loc);
								} else {
									_popRepeatMarker();
								}
								NEWLINE();
								break;
							}
						default:
							_pushRepeatMarker(true, argument, entryFp->pos());
							NEWLINE();
						}
					}
					break;
				case XPKINST_BIGLINEREPEAT:
					{
						switch (_byte) {
						case XPKINST_LINEREPEAT_END:
							{
								XPKRepeatMarker *marker = _currentRepeatMarker();
								if (marker->reps > 0) {
									marker->reps--;
								}
								if (marker->reps > 0) {
									entryFp->seek(marker->loc);
								} else {
									_popRepeatMarker();
								}
								NEWLINE();
								break;
							}
						default:
							_pushRepeatMarker(true, 16+argument, entryFp->pos());
							NEWLINE();
						}
					}
					break;
				case XPKINST_SETXNL:
					if (argument) {
						_curX = argument;
					} else {
						_curX = entryFp->readUint16BE();
					}
					_curY++;
					break;
				case XPKINST_BIGSETXNL:
					_curX = (16 + argument);
					_curY++;
					break;
				case XPKINST_RLENL:
					_nReps = argument;
					NEWLINE();
					break;
				case XPKINST_BIGRLENL:
					_nReps = (16 + argument);
					NEWLINE();
					break;
				case XPKINST_DIRECTNL:
					if (argument) {
						_directCounter = argument;
					} else {
						_directCounter = entryFp->readUint16BE();
					}
					NEWLINE();
					break;
				case XPKINST_BIGDIRECTNL:
					_directCounter = (16 + argument);
					NEWLINE();
					break;
				default:
					warning("unhandled XPK instruction: 0x%X, ftell=%ld", _byte, entryFp->pos());
				}
			}
		}
	}

	return surface;
}

#undef NEWLINE

void XPKDecoder::_pushRepeatMarker(bool withNewline, uint reps, uint32 loc) {
	_repeatMarkerTop++;
	if (_repeatMarkerTop >= XPK_MAX_REPEAT_MARKERS) {
		warning("%s: STACK OVERFLOW!!", __func__);
		_repeatMarkerTop--;
	} else {
		_repeatMarkers[_repeatMarkerTop] = new XPKRepeatMarker();
		_repeatMarkers[_repeatMarkerTop]->newline = withNewline;
		_repeatMarkers[_repeatMarkerTop]->reps = reps;
		_repeatMarkers[_repeatMarkerTop]->loc = loc;
	}
}

void XPKDecoder::_popRepeatMarker() {
	free(_repeatMarkers[_repeatMarkerTop]);
	_repeatMarkerTop--;
}

XPKRepeatMarker *XPKDecoder::_currentRepeatMarker() {
	return _repeatMarkers[_repeatMarkerTop];
}

} // End of namespace Rockett
