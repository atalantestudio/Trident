#pragma once

#include <vulkan/vulkan.h>

#include "Backend/Vulkan/VulkanTexture.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/TextureView.hpp"

namespace ProjectA {
	class VulkanTextureView : public TextureView, public ObjectWrapper<VkImageView> {
		public:
			VulkanTextureView(VkDevice device, const VulkanTexture& texture, const TextureViewDescriptor& descriptor);

			~VulkanTextureView();

		private:
			const VkDevice device;
	};
}