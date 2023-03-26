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

#ifndef ROCKETT_PRESAGE_ARCHIVE_H
#define ROCKETT_PRESAGE_ARCHIVE_H

#include "common/array.h"
#include "common/archive.h"
#include "common/file.h"
#include "common/path.h"
#include "common/stream.h"

#include "rockett/presage_archive_member.h"

namespace Rockett {

#define PRX_MAGIC 1

enum PresageArchiveKind {
	PRESAGE_ARCHIVE_KIND_PRDPRS,
	PRESAGE_ARCHIVE_KIND_PRX
};

class PresageArchive : public Common::Archive {
public:
	PresageArchive(const Common::Path &path);
	PresageArchive(const Common::Path &headerPath, const Common::Path &dataPath);
	~PresageArchive();

	bool read();
	uint16 nEntries();

	// Common::Archive API:
	bool hasFile(const Common::Path &path) const override;
	int listMembers(Common::ArchiveMemberList &list) const override;
	const Common::ArchiveMemberPtr getMember(const Common::Path &path) const override;
	Common::SeekableReadStream *createReadStreamForMember(const Common::Path &path) const override;

private:
	uint32 absoluteOffsetForMemberAtIndex(int index);

	Common::Path _path1; // Only file (PRX) - OR - header-file (PRD/PRS)
	Common::Path _path2; // Only file (PRX) - OR - data-file (PRD/PRS)
	Common::File *_headerFp;
	Common::File *_dataFp;
	enum PresageArchiveKind _kind;
	uint16 _nEntries;
	uint32 _dataOffsetStart;

	typedef Common::SharedPtr<PresageArchiveMember> PresageArchiveMemberPtr;
	Common::Array<PresageArchiveMemberPtr> _members;
};

} // End of namespace Rockett

#endif // ROCKETT_PRESAGE_ARCHIVE_H
