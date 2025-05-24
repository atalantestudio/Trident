#pragma once

#include "Engine/API/Rendering/Descriptor/SwapchainDescriptor.hpp"

namespace ProjectA {
	class Swapchain {
		public:
			virtual ~Swapchain() = default;

			virtual uint32 getImageCount() const = 0;

			// TODO: Use vec2<sint32>.
			virtual uint32 getExtentWidth() const = 0;

			// TODO: Use vec2<sint32>.
			virtual uint32 getExtentHeight() const = 0;

			virtual void getImages(const ref<RenderPass>& renderPass) = 0;

			virtual void update(const SwapchainDescriptor& descriptor) = 0;
	};
}