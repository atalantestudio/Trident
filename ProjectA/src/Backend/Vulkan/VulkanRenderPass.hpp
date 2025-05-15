#pragma once

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/RenderPass.hpp"

namespace ProjectA {
	class VulkanRenderPass : public RenderPass, public ObjectWrapper<VkRenderPass> {
		public:
			VulkanRenderPass(const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor);

			~VulkanRenderPass();

			void recreateMultisampleImage(const VulkanSurface& surface);

			VkImageView getMultisampleImageView() const;

		public:
			const VkSampleCountFlagBits samples;
			uint32 attachmentCount = 1;

		private:
			const VulkanDevice& device;

		private:
			scope<VulkanImage> multisampleImage;
			scope<VulkanImageView> multisampleImageView;
	};
}