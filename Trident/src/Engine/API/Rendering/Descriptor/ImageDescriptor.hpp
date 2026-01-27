#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"
#include "Engine/API/Rendering/ImageType.hpp"
#include "Engine/API/Rendering/ImageUsage.hpp"
#include "Engine/API/Rendering/MemoryProperty.hpp"

namespace Trident {
	struct ImageDescriptor {
		// The basic dimensionality of the image.
		ImageType type;

		// The format and type of the texel blocks that will be contained in the image.
		ImageFormat format;

		// The number of texels in each dimension of the base level of the image.
		Extent3D extent;

		// The number of LOD available for minified sampling of the image.
		uint32 mipLevelCount;

		// The number of layers in the image.
		uint32 layerCount;

		// The number of samples per texel for the image.
		// NOTE: Valid sample counts range from 1 to 64, as powers of 2.
		uint8 sampleCount : 7;

		// The intended usage of the image.
		ImageUsage usage;

		// TODO: Replace by new MemoryResourceType enum.
		MemoryProperty memoryProperties;
	};
}