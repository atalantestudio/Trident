#pragma once

#include "Backend/Vulkan/VulkanImageView.hpp"
#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class VulkanRenderPass : public RenderPass, public ObjectWrapper<VkRenderPass> {
		public:
			VulkanRenderPass(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor);

			~VulkanRenderPass();

			void setAttachmentImageView(uint32 attachmentIndex, const ref<ImageView>& imageView) final override;

			const chain<ref<ImageView>>& getAttachmentImageViews() const;

		public:
			uint32 attachmentCount;
			const chain<Attachment> colorAttachments;
			DepthStencilAttachment depthStencilAttachment;
			uint32 surfaceExtentWidth;
			uint32 surfaceExtentHeight;

		private:
			const VulkanPhysicalDevice& physicalDevice;
			const VulkanDevice& device;

			chain<ref<ImageView>> attachmentImageViews;
	};
}