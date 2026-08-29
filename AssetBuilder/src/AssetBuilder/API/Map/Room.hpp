// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// An axis-aligned rectangular room.
	struct Room {
		std::vector<uint32> faceIndices;

		// TODO(asset-builder/map/room): Keep for rectangular rooms?
		/* union {
			struct {
				uint32 frontFaceIndex;
				uint32 backFaceIndex;
				uint32 leftFaceIndex;
				uint32 rightFaceIndex;
				uint32 topFaceIndex;
				uint32 bottomFaceIndex;
			};

			uint32 faceIndices[6];
		}; */
	};
}