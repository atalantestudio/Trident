#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Buffer/Buffer.hpp"

#include "Backend/Vulkan/VulkanDevice.hpp"

namespace ProjectA {
	class VulkanBuffer : public Buffer, public ObjectWrapper<VkBuffer> {
		public:
			// TODO: Implement staging buffer with vertex and index buffers.
			VulkanBuffer(VkPhysicalDevice physicalDevice, VkCommandPool commandPool, const VulkanDevice& device, const BufferDescriptor& descriptor);

			~VulkanBuffer();

			BufferMappedRange map(uint64 offset, uint64 size) const override;

			void unmap() const override;

		public:
			const uint64 size;

		private:
			const VulkanDevice& device;
			VkDeviceMemory memory;
			VkCommandPool commandPool;
	};
}