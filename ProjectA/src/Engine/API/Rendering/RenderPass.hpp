#pragma once

#include "Engine/API/Rendering/Descriptor/AttachmentImageDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/RenderPassDescriptor.hpp"
#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	class RenderPass {
		public:
			RenderPass(const RenderPassDescriptor& descriptor) :
				depthStencil(descriptor.enableDepthStencil),
				resolve(descriptor.enableResolve),
				present(descriptor.presentAttachmentIndex != std::numeric_limits<uint32>::max()),
				colorAttachmentCount(static_cast<uint32>(descriptor.colorAttachments.size())),
				resolveAttachmentCount(descriptor.enableResolve ? colorAttachmentCount : 0),
				presentAttachmentIndex(descriptor.presentAttachmentIndex),
				colorAttachments(descriptor.colorAttachments),
				depthStencilAttachment(descriptor.depthStencilAttachment)
			{
				DEBUG_ASSERT(colorAttachmentCount > 0);

				colorAttachmentImageViews.resize(colorAttachmentCount);

				attachmentCount = colorAttachmentCount + resolveAttachmentCount;

				if (depthStencil) {
					attachmentCount += 1;
				}
			}

			virtual ~RenderPass() = default;

			uint32 getAttachmentCount() const {
				return attachmentCount;
			}

			uint32 getColorAttachmentCount() const {
				return colorAttachmentCount;
			}

			uint32 getResolveAttachmentCount() const {
				return resolveAttachmentCount;
			}

			uint32 getPresentAttachmentIndex() const {
				return presentAttachmentIndex;
			}

			virtual ref<Image> createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 colorAttachmentIndex) const = 0;

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
			}

		public:
			const bool depthStencil;
			const bool resolve;
			const bool present;

		protected:
			uint32 attachmentCount;
			uint32 colorAttachmentCount;
			uint32 resolveAttachmentCount;
			uint32 presentAttachmentIndex;

			std::vector<ColorAttachment> colorAttachments;
			DepthStencilAttachment depthStencilAttachment;

			std::vector<ref<ImageView>> colorAttachmentImageViews;
			ref<ImageView> depthStencilAttachmentImageView;
	};
}