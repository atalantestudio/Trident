// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct BufferImageCopyRegion {
		uint64 bufferOffset;
		vec3<sint32> imageOffset;
		Extent3D imageExtent;
		uint32 imageMipLevel;
		uint32 imageLayerOffset;
		uint32 imageLayerCount;
	};
}