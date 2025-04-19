#pragma once

#include "Backend/Vulkan/VulkanDevice.hpp"

#include "Engine/Renderer/Buffer/Buffer.hpp"

namespace ProjectA {
	class VulkanBuffer : public Buffer, public ObjectWrapper<VkBuffer> {
		public:
			VulkanBuffer(VkPhysicalDevice physicalDevice, const VulkanDevice& device, const BufferDescriptor& descriptor);

			~VulkanBuffer();

			void unmap() const override;

			void* getMappedRange(uint64 offset, uint64 size) const override;

		public:
			const uint64 size;

		private:
			const VulkanDevice& device;
			VkDeviceMemory memory;
	};
}