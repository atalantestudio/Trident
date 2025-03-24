#pragma once

#include "Engine/Renderer/RenderPass.hpp"

namespace ProjectA {
	struct SwapchainDescriptor {
		ref<RenderPass> renderPass;

		// TODO: Use vec2<sint32>.
		uint32 extentWidth = 0;
		uint32 extentHeight = 0;
	};

	class Swapchain {
		public:
			virtual ~Swapchain() = default;

			virtual void recreate(const SwapchainDescriptor& descriptor) = 0;

			// TODO: Use vec2<sint32>.
			virtual uint32 getExtentWidth() const = 0;
			virtual uint32 getExtentHeight() const = 0;
	};
}