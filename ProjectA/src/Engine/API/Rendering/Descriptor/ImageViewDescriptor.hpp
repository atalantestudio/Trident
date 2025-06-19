#pragma once

#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	struct ImageViewDescriptor {
		ref<Image> image;
		uint32 mipLevelOffset;
		uint32 mipLevelCount;
		uint32 layerOffset;
		uint32 layerCount;
	};
}