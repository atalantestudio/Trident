// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageType.hpp"
#include "Engine/API/Graphics/ImageUsage.hpp"

namespace Trident {
	struct AttachmentImageDescriptor {
		ImageType type;
		Extent3D extent;
		uint32 mipLevelCount;
		uint32 layerCount;
		ImageUsage additionalUsage;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}