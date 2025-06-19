#pragma once

// #include "Engine/API/Rendering/Descriptor/AttachmentImageDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/RenderPassDescriptor.hpp"
// #include "Engine/API/Rendering/Image.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class RenderPass {
		public:
			RenderPass::RenderPass(const RenderPassDescriptor& descriptor) :
				subpasses(descriptor.subpasses)
			{
				DEBUG_VALIDATE_DESCRIPTOR(descriptor);

				for (const Subpass& subpass : descriptor.subpasses) {
					colorAttachmentCount += static_cast<uint32>(subpass.colorAttachments.count);
					attachmentCount += static_cast<uint32>(subpass.colorAttachments.count);

					if (subpass.resolve) {
						// If resolve is enabled, then every color attachment must be resolved,
						// so this is twice the number of color attachments.
						attachmentCount += static_cast<uint32>(subpass.colorAttachments.count);
					}

					if (subpass.depthStencil) {
						// TODO: +1 if depth stencil resolve attachment?
						attachmentCount += 1;

						depthStencilSubpass = true;
					}
				}

				DEBUG_ASSERT(attachmentCount > 0);

				// colorAttachmentImageViews.resize(colorAttachmentCount);

				/* if (depthStencil && depthStencilAttachment.loadOperation == LoadOperation::CLEAR) {
					clearValueCount += 1;
				}
				else {
					for (sint32 colorAttachmentIndex = static_cast<sint32>(colorAttachmentCount - 1); colorAttachmentIndex >= 0; colorAttachmentIndex -= 1) {
						const ColorAttachment& colorAttachment = colorAttachments[colorAttachmentIndex];

						if (colorAttachment.loadOperation != LoadOperation::CLEAR) {
							clearValueCount = colorAttachmentIndex;

							break;
						}
					}
				} */
			}

			virtual ~RenderPass() = default;

			uint32 getColorAttachmentCount() const {
				return colorAttachmentCount;
			}

			uint32 getAttachmentCount() const {
				return attachmentCount;
			}

			uint32 getClearValueCount() const {
				return clearValueCount;
			}

			bool hasDepthStencilSubpass() const {
				return depthStencilSubpass;
			}

			const Subpass& getSubpass(uint32 index) const {
				ENGINE_ASSERT(index < subpasses.count, "Subpass index overflow.");

				return subpasses[index];
			}

			uint32 getSubpassCount() const {
				return static_cast<uint32>(subpasses.count);
			}

			/* virtual ref<Image> createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 colorAttachmentIndex) const = 0;

			virtual ref<Image> createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor) const = 0;

			const std::vector<ref<ImageView>>& getColorAttachmentImageViews() const {
				return colorAttachmentImageViews;
			}

			void setColorAttachmentImageView(const ref<ImageView>& imageView, uint32 colorAttachmentIndex) {
				DEBUG_ASSERT(colorAttachmentIndex < colorAttachmentCount);
				DEBUG_ASSERT((resolveAttachmentCount == 0 && colorAttachmentIndex != presentAttachmentIndex) || resolveAttachmentCount > 0);

				colorAttachmentImageViews[colorAttachmentIndex] = imageView;
			}

			const ref<ImageView>& getDepthStencilAttachmentImageView() const {
				return depthStencilAttachmentImageView;
			}

			void setDepthStencilAttachmentImageView(const ref<ImageView>& imageView) {
				DEBUG_ASSERT(depthStencil);

				depthStencilAttachmentImageView = imageView;
			} */

		protected:
			uint32 colorAttachmentCount = 0;
			uint32 attachmentCount = 0;
			uint32 clearValueCount = 0;

		private:
			bool depthStencilSubpass = false;

			// std::vector<ColorAttachment> colorAttachments;
			// std::vector<ref<ImageView>> colorAttachmentImageViews;

			sequence<Subpass> subpasses;
	};
}