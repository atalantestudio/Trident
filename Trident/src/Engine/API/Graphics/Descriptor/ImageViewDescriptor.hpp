// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Image.hpp"
#include "Engine/API/Graphics/ImageViewType.hpp"

namespace Trident {
	struct ImageViewDescriptor {
		ImageViewType type;
		uint32 mipLevelOffset;
		uint32 mipLevelCount;
		uint32 layerOffset;
		uint32 layerCount;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}