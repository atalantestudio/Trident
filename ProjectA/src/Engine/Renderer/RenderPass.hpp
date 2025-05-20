#pragma once

#include "Engine/Renderer/LoadOperation.hpp"
#include "Engine/Renderer/StoreOperation.hpp"

namespace ProjectA {
	struct RenderPassDescriptor {
		uint8 sampleCount = 1;
		LoadOperation loadOperation;
		StoreOperation storeOperation;
	};

	// In Vulkan, the render pass is used:
	// - in the descriptors for the swapchain framebuffers
	// - in the descriptor for the graphics pipeline
	// - in the descriptor for the render pass begin info
	class RenderPass {
		public:
			virtual ~RenderPass() = default;
	};
}