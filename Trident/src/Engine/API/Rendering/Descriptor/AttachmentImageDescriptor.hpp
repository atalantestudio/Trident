// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageType.hpp"
#include "Engine/API/Rendering/ImageUsage.hpp"

namespace Trident {
	struct AttachmentImageDescriptor {
		ImageType type;
		Extent3D extent;
		uint32 mipLevelCount;
		uint32 layerCount;
		ImageUsage additionalUsage;

		#if defined(BUILD_MODE_DEBUG)
			view<char8> name;
		#endif
	};
}