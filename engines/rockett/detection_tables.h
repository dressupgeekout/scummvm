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

namespace Rockett {

const PlainGameDescriptor rockettGames[] = {
	{ "rockett_newschool", "Rockett's New School" },
	{ "rockett_tricky", "Rockett's Tricky Decision" },
	{ "rockett_secret", "Rockett's Secret Invitation" },
	{ "rockett_dance", "Rockett's First Dance" },
	{ 0, 0 }
};

const ADGameDescription gameDescriptions[] = {
	{
		"rockett_newschool",
		nullptr,
		AD_ENTRY1s("TITLE.PRD", "a91e86f48d50dd08753a90cf16d2ed87", 552),
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},
	{
		"rockett_tricky",
		nullptr,
		AD_ENTRY1s("Title.PRX", "ea56d44ed1d7ae37c1518167c6ac2f30", 1320704),
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},
	{
		"rockett_secret",
		nullptr,
		AD_ENTRY1s("TITLE.PRX", "324c5946149505beacd4f564330363f9", 1549765),
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},
	{
		"rockett_dance",
		nullptr,
		AD_ENTRY1s("TITLE.PRX", "eb0f2b24d4c16b76142cd4615716fce7", 2524847),
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},
	AD_TABLE_END_MARKER
};

} // End of namespace Rockett
