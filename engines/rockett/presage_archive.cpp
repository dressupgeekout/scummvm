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
#include "common/file.h"
#include "common/path.h"
#include "common/stream.h"

#include "rockett/presage_archive.h"

namespace Rockett {

PresageArchive::PresageArchive(const Common::Path &path) {
	_path1 = path;
	_path2 = path;
}

PresageArchive::PresageArchive(const Common::Path &headerPath, const Common::Path &dataPath) {
	_path1 = headerPath;
	_path2 = dataPath;
}

PresageArchive::~PresageArchive() {
	_headerFp->close();
	_dataFp->close();
}

bool PresageArchive::read() {
	if (_path1 == _path2) {
		_kind = PRESAGE_ARCHIVE_KIND_PRX;
	} else {
		_kind = PRESAGE_ARCHIVE_KIND_PRDPRS;
	}

	// At this stage in the process, we are only concerned about the members'
	// metadata. In any case, that info is in the first file.
	_headerFp = new Common::File;
	_headerFp->open(_path1);

	byte magic = _headerFp->readByte();
	if (magic != PRX_MAGIC) {
		return false;
	}

	_headerFp->skip(137); // 128 + 10 - 1, I guess -- all zeroes

	// This is the number of entries _including_ dummy ones, which may be present
	// at the start and/or end of the archive's table of contents.
	uint16 actualNEntries = _headerFp->readUint16LE();

	// This is the real count of entries we ultimately care about.
	_nEntries = _headerFp->readUint16LE();

	_headerFp->skip(2); // Ignore

	_members = Common::Array<PresageArchiveMemberPtr>(_nEntries);

	// Purposefully skip over the zeroeth entry (which has internal_id of 1),
	// it's always a "dummy" entry which serves little purpose.
	_headerFp->skip(24);

	// OK now we *finally* can access the individual archive members.
	for (int i = 0; i < _nEntries; i++) {
		PresageArchiveMember *member = new PresageArchiveMember;

		// The last internal_id is always stored as '0' rather than what you'd
		// expect. Anthony Kozar suspects this is to merely signify the end of the
		// list, since we already know how many members there are by now.
		uint16 internal_id = _headerFp->readUint16LE();

		if (internal_id == 0) {
			member->setInternalId(_nEntries+1);
		} else {
			member->setInternalId(internal_id);
		}

		_headerFp->skip(6);

		uint32 offset = _headerFp->readUint32LE();
		member->setOffset(offset);

		Common::String filetype = _headerFp->readString(0, PRESAGEARCHIVEMEMBER_FILETYPE_LEN);
		member->setFiletype(filetype);

		uint32 rid = _headerFp->readUint32LE();
		member->setResourceId(rid);

		uint32 size = _headerFp->readUint32LE();
		member->setSize(size);

		_members.insert_at(i, PresageArchiveMemberPtr(member));
	}

	// Account for the dummy entries at the end of the table of contents, if
	// necessary.
	_headerFp->skip(24*(actualNEntries - _nEntries));

	// In PRXes, the "offset" field for each member does NOT indicate an absolute
	// offset. It's actually an offset from the position of the file we're
	// currently at right now. So let's keep track of it.
	if (_kind == PRESAGE_ARCHIVE_KIND_PRX)
		_dataOffsetStart = _headerFp->pos();

	// OK now let's figure out the individual member names. We start with the
	// 1st member (not the 0th member) because remember we actually can't
	// obtain information about the "dummy" entry at the beginning.
	//
	// Also, the offset brings us to the start of the data, but that skips
	// past the filename!  So that's why we rewind a little.
	//
	// This is a little tricky because, in PRD/PRS pairs, the member names are
	// NOT stored in the header. So we have to bring the other file pointer into
	// the picture now.
	_dataFp = new Common::File;

	switch (_kind) {
	case PRESAGE_ARCHIVE_KIND_PRDPRS:
		_dataFp->open(_path2);
		break;
	case PRESAGE_ARCHIVE_KIND_PRX:
		_dataFp = _headerFp;
		break;
	}

	for (int i = 0; i < _nEntries; i++) {
		_dataFp->seek(this->absoluteOffsetForMemberAtIndex(i), SEEK_SET);

		// Rewind to get the filename. Purposefully avoid an extra underscore in
		// PRD/PRS members.
		uint32 rewind_amt = PRESAGEARCHIVEMEMBER_NAME_LEN;
		rewind_amt += (_kind == PRESAGE_ARCHIVE_KIND_PRDPRS) ? 5 : 6;
		_dataFp->seek(_dataFp->pos() - rewind_amt, SEEK_SET);

		Common::String name = _dataFp->readString(0, PRESAGEARCHIVEMEMBER_NAME_LEN);
		_members[i]->setName(name);
	}

	return true;
}

uint16 PresageArchive::nEntries()
{
	return _nEntries;
}

uint32 PresageArchive::absoluteOffsetForMemberAtIndex(int index) {
	uint32 offset;

	switch (_kind) {
	case PRESAGE_ARCHIVE_KIND_PRDPRS:
		offset = _members[index]->_offset;
		break;
	case PRESAGE_ARCHIVE_KIND_PRX:
		offset = _dataOffsetStart + _members[index]->_offset;
		break;
	}

	return offset;
}

// Common::Archive API
bool PresageArchive::hasFile(const Common::Path &path) const {
	bool result = false;

	for (int i = 0; i < _nEntries; i++) {
		if (_members[i]->getName() == path.toString()) {
			return true;
			break;
		}
	}

	return result;
}

// Common::Archive API
int PresageArchive::listMembers(Common::ArchiveMemberList &list) const {
	for (int i = 0; i < _nEntries; i++) {
		list.push_back(Common::ArchiveMemberList::value_type(_members[i]));
	}

	return _nEntries;
}

// Common::Archive API
//
// XXX this should use a hash-map instead of walking through the list every time
const Common::ArchiveMemberPtr PresageArchive::getMember(const Common::Path &path) const
{
	if (!this->hasFile(path))
		return Common::ArchiveMemberPtr();

	for (int i = 0; i < _nEntries; i++) {
		if (_members[i]->getName() == path.toString()) {
			return _members[i];
		}
	}

	return Common::ArchiveMemberPtr();
}

// Common::Archive API
Common::SeekableReadStream *PresageArchive::createReadStreamForMember(const Common::Path &path) const {
	if (!this->hasFile(path))
		return nullptr;

	return nullptr;
}

} // End of namespace Rockett
