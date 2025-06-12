#pragma once

#include "Engine/Extent.hpp"

namespace ProjectA {
	struct BufferImageCopyRegion {
		uint64 bufferOffset;
		Extent3D imageExtent;
		sint32 imageOffsetX;
		sint32 imageOffsetY;
		sint32 imageOffsetZ;
		uint32 imageMipLevel;
		uint32 imageLayerOffset;
		uint32 imageLayerCount;
	};
}