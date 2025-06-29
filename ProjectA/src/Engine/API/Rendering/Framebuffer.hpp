#pragma once

#include "Engine/API/Rendering/Descriptor/FramebufferDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class Framebuffer {
		public:
			virtual ~Framebuffer() = default;

			const ref<RenderPass>& getRenderPass() const {
				return renderPass;
			}

			Extent2D getExtent() const {
				return extent;
			}

			uint32 getLayerCount() const {
				return layerCount;
			}

			uint32 getColorAttachmentCount() const {
				return colorAttachmentCount;
			}

			uint32 getDepthStencilAttachmentIndex() const {
				return depthStencilAttachmentIndex;
			}

		protected:
			explicit Framebuffer(const FramebufferDescriptor& descriptor) :
				renderPass(descriptor.renderPass),
				extent(descriptor.extent),
				layerCount(descriptor.layerCount)
			{
				DEBUG_DESCRIPTOR_GUARD(descriptor);

				for (uint32 imageViewIndex = 0; imageViewIndex < descriptor.imageViews.count; imageViewIndex += 1) {
					if (isDepthFormat(descriptor.imageViews[imageViewIndex]->getImage()->getFormat())) {
						depthStencilAttachmentIndex = imageViewIndex;

						continue;
					}

					colorAttachmentCount += 1;
				}
			}

		protected:
			ref<RenderPass> renderPass;
			Extent2D extent;
			uint32 layerCount;
			uint32 colorAttachmentCount = 0;
			uint32 depthStencilAttachmentIndex = ~0;
	};
}