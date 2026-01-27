// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageType.hpp"

namespace Trident {
	struct AttachmentImageDescriptor {
		ImageType type;
		Extent3D extent;
		uint32 mipLevelCount;
		uint32 layerCount;
	};
}