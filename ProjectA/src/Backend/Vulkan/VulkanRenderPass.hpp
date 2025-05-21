#pragma once

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class VulkanRenderPass : public RenderPass, public ObjectWrapper<VkRenderPass> {
		private:
			static void countAttachments(const RenderPassDescriptor& descriptor, uint32& colorAttachmentCount, uint32& resolveAttachmentCount, uint32& depthStencilAttachmentCount);

		public:
			VulkanRenderPass(const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor);

			~VulkanRenderPass();

			VkImageView getAttachmentImageView(uint32 attachmentIndex) const;

			void recreateResolveAttachmentImageViews(const VulkanSurface& surface);

			void recreateDepthStencilAttachmentImageView(const VulkanSurface& surface);

		public:
			uint32 attachmentCount;
			const chain<Attachment> colorAttachments;
			bool hasDepthStencilAttachment;
			uint32 surfaceExtentWidth;
			uint32 surfaceExtentHeight;

			chain<scope<VulkanImage>> resolveImages;
			chain<scope<VulkanImageView>> resolveImageViews;
			scope<scope<VulkanImage>> depthStencilImage;
			scope<VulkanImageView> depthStencilImageView;

		private:
			const VulkanDevice& device;

		private:
			fchain<VkAttachmentReference> resolveAttachmentReferences;
			VkAttachmentReference depthStencilAttachmentReference;
	};
}