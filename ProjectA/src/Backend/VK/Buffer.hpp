#pragma once

#include "Backend/VK/Device.hpp"
#include "Backend/VK/MemoryAllocator.hpp"

#include "Engine/API/Rendering/Buffer.hpp"

namespace ProjectA {
	class VulkanBuffer final : public Buffer, public ObjectWrapper<VkBuffer> {
		public:
			VulkanBuffer(VkDevice device, const BufferDescriptor& descriptor);

			~VulkanBuffer();

			void unmap() const final override;

			void* getMappedRange(uint64 offset, uint64 size) const final override;

		private:
			VkDevice device;
			MemorySuballocation memorySuballocation;
	};
}