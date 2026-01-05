#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"

#include "Engine/API/Rendering/Buffer.hpp"

namespace ProjectA {
	class VulkanBuffer final : public Buffer, public ObjectWrapper<VkBuffer> {
		public:
			explicit VulkanBuffer(VkDevice device, const BufferDescriptor& descriptor);

			~VulkanBuffer() final override;

			void unmap() const final override;

			void* getMappedRange(uint64 offset, uint64 size) const final override;

		private:
			VkDevice device;
			MemorySuballocation memorySuballocation;
	};
}