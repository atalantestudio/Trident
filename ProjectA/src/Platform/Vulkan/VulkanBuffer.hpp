#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Buffer/Buffer.hpp"

#include "Platform/Vulkan/VulkanDevice.hpp"

namespace ProjectA {
	class VulkanBuffer : public Buffer {
		public:
			VulkanBuffer(const VkPhysicalDevice physicalDevice, VulkanDevice& device, const BufferDescriptor& descriptor);

			~VulkanBuffer();

			void write(const void* data, const uint64 size) const override;

		public:
			VkBuffer object;
			const uint64 size;

		private:
			const VulkanDevice& device;
			uint32 memoryTypeIndex;
	};
}