#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Buffer/Buffer.hpp"

#include "Backend/Vulkan/VulkanDevice.hpp"

namespace ProjectA {
	class VulkanBuffer : public Buffer, public ObjectWrapper<VkBuffer> {
		public:
			// TODO: Use staging buffer with vertex and index buffers.
			VulkanBuffer(const VkPhysicalDevice physicalDevice, VulkanDevice& device, const BufferDescriptor& descriptor);

			~VulkanBuffer();

			void map(const uint64 offset, const uint64 size) override;

			void write(const void* data, const uint64 offset, const uint64 size) override;

			void unmap() override;

		public:
			const uint64 size;

		private:
			const VulkanDevice& device;
			VkDeviceMemory memory;
			void* mappedRange = 0;
	};
}