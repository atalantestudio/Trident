#pragma once

#include "Engine/Renderer/RenderPass.hpp"

namespace ProjectA {
	struct SwapchainDescriptor {
		ref<RenderPass> renderPass;
	};

	class Swapchain {
		public:
			virtual ~Swapchain() = default;
	};
}