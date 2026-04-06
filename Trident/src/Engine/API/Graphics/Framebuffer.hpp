// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Descriptor/FramebufferDescriptor.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class Framebuffer {
		public:
			~Framebuffer();

			inline ref<RenderPass> getRenderPass() const {
				return renderPass;
			}

			inline Extent2D getExtent() const {
				return extent;
			}

		protected:
			explicit Framebuffer(const Device* device, const FramebufferDescriptor& descriptor) :
				device(device),
				renderPass(descriptor.renderPass),
				extent(descriptor.extent)
			{}

		protected:
			const Device* device;
			ref<RenderPass> renderPass;
			Extent2D extent;
	};
}