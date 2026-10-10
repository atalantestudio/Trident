// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// An axis-aligned rectangular room.
	struct Room {
		std::vector<uint32> faceIndices;
	};
}