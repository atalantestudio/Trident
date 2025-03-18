#pragma once

#include "Engine/Renderer/RenderPass.hpp"

namespace ProjectA {
	struct SwapchainDescriptor {
		ref<RenderPass> renderPass;
	};

	class Swapchain {
		public:
			virtual ~Swapchain() = default;

			// TODO: Use vec2<sint32>.
			virtual uint32 getExtentWidth() const = 0;
			virtual uint32 getExtentHeight() const = 0;
	};
}