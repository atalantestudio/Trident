#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class VulkanRenderPass final : public RenderPass, public ObjectWrapper<VkRenderPass> {
		public:
			explicit VulkanRenderPass(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor);

			~VulkanRenderPass() final override;

		private:
			// Returns true if this attachment is a color attachment used in any subpass.
			bool isColorAttachment(uint32 attachmentIndex) const;

		public:
			const VulkanSurface* surface;

		private:
			const VulkanPhysicalDevice& physicalDevice;
			const VulkanDevice& device;
	};
}