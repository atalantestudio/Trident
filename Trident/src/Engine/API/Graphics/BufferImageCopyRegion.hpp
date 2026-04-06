// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct BufferImageCopyRegion {
		uint64 bufferOffset;

		// TODO: Use vec3<sint32> instead.
		sint32 imageOffsetX;
		sint32 imageOffsetY;
		sint32 imageOffsetZ;

		Extent3D imageExtent;
		uint32 imageMipLevel;
		uint32 imageLayerOffset;
		uint32 imageLayerCount;
	};
}