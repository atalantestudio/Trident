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

			uint32 getWidth() const override;

			uint32 getHeight() const override;

			ref<TextureView> createView(const TextureViewDescriptor& descriptor) const override;

		public:
			const TextureType type;
			const TextureFormat format;

		private:
			VkDevice device;
			VkDeviceMemory memory;
			const uint32 width;
			const uint32 height;
	};
}