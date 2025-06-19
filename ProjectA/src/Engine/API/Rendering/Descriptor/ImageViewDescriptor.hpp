#pragma once

#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	struct ImageViewDescriptor {
		uint32 mipLevelOffset;
		uint32 mipLevelCount;
		uint32 layerOffset;
		uint32 layerCount;
	};
}