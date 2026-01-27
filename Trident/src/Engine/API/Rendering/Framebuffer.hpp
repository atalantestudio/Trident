// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/FramebufferDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class Framebuffer {
		public:
			virtual ~Framebuffer() = default;

			ref<RenderPass> getRenderPass() const {
				return renderPass;
			}

			Extent2D getExtent() const {
				return extent;
			}

			uint32 getLayerCount() const {
				return layerCount;
			}

		protected:
			explicit Framebuffer(const FramebufferDescriptor& descriptor) :
				renderPass(descriptor.renderPass),
				extent(descriptor.extent),
				layerCount(descriptor.layerCount)
			{
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}

		protected:
			ref<RenderPass> renderPass;
			Extent2D extent;
			uint32 layerCount;
	};
}