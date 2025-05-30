#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"
#include "Engine/API/Rendering/ImageType.hpp"
#include "Engine/API/Rendering/MemoryProperty.hpp"
#include "Engine/Renderer/ImageUsage.hpp"

namespace ProjectA {
	struct ImageDescriptor {
		ImageType type;
		ImageFormat format;
		ImageUsage usage;
		uint32 width;
		uint32 height;
		uint32 depth;
		uint32 mipLevelCount;
		uint32 arrayLayerCount;

		// The number of samples for the image. Must be a power of 2.
		uint8 sampleCount;

		MemoryProperty memoryProperties;
	};
}