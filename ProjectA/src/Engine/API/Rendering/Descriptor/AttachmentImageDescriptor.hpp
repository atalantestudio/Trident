#pragma once

#include "Engine/API/Rendering/ImageType.hpp"

namespace ProjectA {
	struct AttachmentImageDescriptor {
		ImageType type;
		Extent3D extent;
		uint32 mipLevelCount;
		uint32 layerCount;
	};
}