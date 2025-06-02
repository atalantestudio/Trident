#pragma once

#include "Engine/API/Rendering/Descriptor/SwapchainDescriptor.hpp"

namespace ProjectA {
	class Swapchain {
		public:
			virtual ~Swapchain() = default;

			uint32 getExtentWidth() const {
				return extentWidth;
			}

			uint32 getExtentHeight() const {
				return extentHeight;
			}

			virtual uint32 getImageCount() const = 0;

			virtual void getImages(const ref<RenderPass>& renderPass) = 0;

			virtual void update(const SwapchainDescriptor& descriptor) = 0;

		protected:
			// TODO: Use vec2<uint32>.
			uint32 extentWidth;
			uint32 extentHeight;
	};
}