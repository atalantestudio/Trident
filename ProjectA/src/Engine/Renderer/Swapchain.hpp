#pragma once

#include "Engine/API/Rendering/Descriptor/SwapchainDescriptor.hpp"

namespace ProjectA {
	class Swapchain {
		public:
			virtual ~Swapchain() = default;

			virtual void update(const SwapchainDescriptor& descriptor) = 0;

			// TODO: Use vec2<sint32>.
			virtual uint32 getExtentWidth() const = 0;
			virtual uint32 getExtentHeight() const = 0;
	};
}