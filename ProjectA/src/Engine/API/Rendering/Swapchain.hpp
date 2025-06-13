#pragma once

#include "Engine/API/Rendering/Descriptor/SwapchainDescriptor.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class Swapchain {
		public:
			virtual ~Swapchain() = default;

			Extent2D getExtent() const {
				return extent;
			}

			virtual uint32 getImageCount() const = 0;

			virtual void getImages(const ref<RenderPass>& renderPass) = 0;

			virtual void update(const SwapchainDescriptor& descriptor) = 0;

		protected:
			Extent2D extent;
	};
}