#pragma once

#include "Backend/Vulkan/VulkanDevice.hpp"

#include "Backend/Vulkan/MemoryAllocator.hpp"

#include "Engine/API/Rendering/Buffer.hpp"

namespace ProjectA {
	class VulkanBuffer : public Buffer, public ObjectWrapper<VkBuffer> {
		public:
			VulkanBuffer(VkDevice device, const BufferDescriptor& descriptor);

			~VulkanBuffer();

			void unmap() const override;

			void* getMappedRange(uint64 offset, uint64 size) const override;

		private:
			VkDevice device;
			MemorySuballocation memorySuballocation;
	};
}