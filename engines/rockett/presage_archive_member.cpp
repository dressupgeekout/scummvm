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

#include "common/archive.h"
#include "common/stream.h"

#include "rockett/presage_archive_member.h"
#include "rockett/presage_archive.h"

namespace Rockett {

PresageArchiveMember::PresageArchiveMember() {
	// OK
}

PresageArchiveMember::~PresageArchiveMember() {
	// OK
}

void PresageArchiveMember::setInternalId(int16 internal_id) {
	_internal_id = internal_id;
}

void PresageArchiveMember::setOffset(uint32 offset) {
	_offset = offset;
}

void PresageArchiveMember::setFiletype(const Common::String &filetype) {
	_filetype = filetype;
}

void PresageArchiveMember::setResourceId(uint32 rid) {
	_rid = rid;
}

void PresageArchiveMember::setSize(uint32 size) {
	_size = size;
}

void PresageArchiveMember::setName(const Common::String &name) {
	_name = name;
}

uint16 PresageArchiveMember::getInternalId() const {
	return _internal_id;
}

uint32 PresageArchiveMember::getOffset() const {
	return _offset;
}

Common::String PresageArchiveMember::getFiletype() const {
	return _filetype;
}

uint32 PresageArchiveMember::getResourceId() const {
	return _rid;
}

uint32 PresageArchiveMember::getSize() const {
	return _size;
}

// Common::ArchiveMember API
Common::SeekableReadStream *PresageArchiveMember::createReadStream() const {
	return _parent->createReadStreamForMember(Common::Path(this->getName()));
}

// Common::ArchiveMember API
Common::String PresageArchiveMember::getName() const {
	return _name + "." + _filetype;
}

// Common::ArchiveMember API
Common::U32String PresageArchiveMember::getDisplayName() const {
	return Common::U32String(this->getName());
}

} // End of namespace Rockett
