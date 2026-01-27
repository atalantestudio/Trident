// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Image.hpp"

namespace Trident {
	struct ImageViewDescriptor {
		uint32 mipLevelOffset;
		uint32 mipLevelCount;
		uint32 layerOffset;
		uint32 layerCount;
	};
}