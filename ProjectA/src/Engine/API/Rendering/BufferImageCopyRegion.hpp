#pragma once

namespace ProjectA {
	struct BufferImageCopyRegion {
		uint64 bufferOffset;

		// TODO: Use Extent3D or vec3<uint32>.
		sint32 imageOffsetX;
		sint32 imageOffsetY;
		sint32 imageOffsetZ;

		Extent3D imageExtent;
		uint32 imageMipLevel;
		uint32 imageLayerOffset;
		uint32 imageLayerCount;
	};
}