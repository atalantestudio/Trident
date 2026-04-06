// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Image.hpp"

namespace Trident {
	struct ImageViewDescriptor {
		uint32 mipLevelOffset;
		uint32 mipLevelCount;
		uint32 layerOffset;
		uint32 layerCount;

		#if defined(BUILD_MODE_DEBUG)
			view<char8> name;
		#endif
	};
}