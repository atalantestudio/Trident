#pragma once

#include <vulkan/vulkan.h>

#include "Backend/Vulkan/VulkanDevice.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Texture.hpp"

namespace ProjectA {
	class VulkanTexture : public Texture, public ObjectWrapper<VkImage> {
		public:
			VulkanTexture(const VulkanDevice& device, const TextureDescriptor& descriptor);

			~VulkanTexture();

		private:
			const VulkanDevice& device;
			VkDeviceMemory memory;
	};
}