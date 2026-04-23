// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Descriptor/FramebufferDescriptor.hpp"
#include "Engine/API/Graphics/Device.hpp"

namespace Trident {
	class Framebuffer {
		public:
			~Framebuffer();

			ref<RenderPass> getRenderPass() const;

			Extent2D getExtent() const;

		protected:
			explicit Framebuffer(const Device* device, const FramebufferDescriptor& descriptor);

		protected:
			const Device* device;
			ref<RenderPass> renderPass;
			Extent2D extent;
	};
}