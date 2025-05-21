#pragma once

#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	struct SwapchainDescriptor {
		ref<RenderPass> renderPass;

		// These properties are only used in OpenGL.
		// In Vulkan we can get them by updating the surface.
		uint32 extentWidth = 0;
		uint32 extentHeight = 0;
	};
}