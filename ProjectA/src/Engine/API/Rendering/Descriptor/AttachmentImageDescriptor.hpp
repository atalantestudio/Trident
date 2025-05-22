#pragma once

#include "Engine/API/Rendering/ImageType.hpp"

namespace ProjectA {
	struct AttachmentImageDescriptor {
		ImageType type;
		uint32 width;
		uint32 height;
		uint32 depth;
		uint32 mipLevelCount;
		uint32 arrayLayerCount;
	};
}