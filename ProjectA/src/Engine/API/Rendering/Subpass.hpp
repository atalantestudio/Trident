#pragma once

#include "Engine/API/Rendering/Descriptor/AttachmentImageDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/SubpassDescriptor.hpp"
#include "Engine/API/Rendering/Image.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	struct Subpass {
		public:
			Subpass() {}

			explicit Subpass(const SubpassDescriptor& descriptor) :
				colorAttachments(descriptor.colorAttachments),
				rasterizationSampleCount(1),
				presentAttachmentIndex(descriptor.presentAttachmentIndex),
				depthStencil(descriptor.depthStencil),
				depthStencilAttachment(descriptor.depthStencilAttachment)
			{
				DEBUG_VALIDATE_DESCRIPTOR(descriptor);

				for (const ColorAttachment& colorAttachment : colorAttachments) {
					if (colorAttachment.sampleCount > 1) {
						rasterizationSampleCount = colorAttachment.sampleCount;
						resolveAttachmentCount = static_cast<uint32>(colorAttachments.count);

						break;
					}
				}
			}

			const sequence<ColorAttachment>& getColorAttachments() const {
				return colorAttachments;
			}

			uint32 getColorAttachmentCount() const {
				return static_cast<uint32>(colorAttachments.count);
			}

			uint32 getResolveAttachmentCount() const {
				return resolveAttachmentCount;
			}

			uint8 getRasterizationSampleCount() const {
				return rasterizationSampleCount;
			}

			uint32 getPresentAttachmentIndex() const {
				return presentAttachmentIndex;
			}

			bool hasDepthStencilAttachment() const {
				return depthStencil;
			}

			DepthStencilAttachment getDepthStencilAttachment() const {
				return depthStencilAttachment;
			}

			ref<Image> createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 colorAttachmentIndex) const;

			ref<Image> createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor) const;

		private:
			sequence<ColorAttachment> colorAttachments;
			uint32 resolveAttachmentCount = 0;
			uint8 rasterizationSampleCount : 7;
			uint32 presentAttachmentIndex = 0;
			bool depthStencil;
			DepthStencilAttachment depthStencilAttachment;
	};

	/* class SubpassOld {
		public:
			Subpass(const SubpassDescriptor& subpassDescriptor) :
				depthStencilAttachment(subpassDescriptor.depthStencil)
			{
				// TODO
			}

			bool hasDepthStencilAttachment() const {
				return depthStencilAttachment;
			}

		private:
			const bool depthStencilAttachment;
	}; */
}