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

#ifndef ROCKETT_PRESAGE_ARCHIVE_MEMBER_H
#define ROCKETT_PRESAGE_ARCHIVE_MEMBER_H

#include "common/archive.h"
#include "common/stream.h"

namespace Rockett {

#define PRESAGEARCHIVEMEMBER_FILETYPE_LEN 4
#define PRESAGEARCHIVEMEMBER_NAME_LEN 16

// Forward declaration
class PresageArchive;

class PresageArchiveMember : public Common::ArchiveMember {
public:
	PresageArchiveMember();
	~PresageArchiveMember();

	void setInternalId(int16 internal_id);
	void setOffset(uint32 offset);
	void setFiletype(const Common::String &filetype);
	void setResourceId(uint32 rid);
	void setSize(uint32 size);
	void setName(const Common::String &name);

	uint16 getInternalId() const;
	uint32 getOffset() const;
	Common::String getFiletype() const;
	uint32 getResourceId() const;
	uint32 getSize() const;

	// Common::ArchiveMember API:
	Common::SeekableReadStream *createReadStream() const;
	Common::String getName() const;
	Common::U32String getDisplayName() const;

	friend class PresageArchive;

private:
	PresageArchive *_parent;
	Common::String _filetype;
	uint16 _internal_id;
	uint32 _offset;
	uint32 _rid;
	uint32 _size;
	Common::String _name;
};

} // End of namespace Rockett

#endif // ROCKETT_PRESAGE_ARCHIVE_MEMBER_H
