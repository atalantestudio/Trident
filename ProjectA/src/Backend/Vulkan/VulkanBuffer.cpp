#include "pch.hpp"

#include "Backend/Vulkan/VulkanBuffer.hpp"

#include "Backend/Vulkan/BufferUsage.hpp"

namespace ProjectA {
	VulkanBuffer::VulkanBuffer(VkDevice device, const BufferDescriptor& descriptor) :
		Buffer(descriptor.size),
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkResult result;

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = getVkBufferUsageFlagBits(descriptor.usage);
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan buffer (code: []).", result);

		VkMemoryRequirements memoryRequirements;

		vkGetBufferMemoryRequirements(device, object, &memoryRequirements);

		const uint32 memoryTypeIndex = MemoryAllocator::findMemoryType(memoryRequirements, static_cast<VkMemoryPropertyFlags>(descriptor.memoryProperties));

		memorySuballocation = MemoryAllocator::suballocateBuffer(device, object, memoryTypeIndex, memoryRequirements.size);
	}

	VulkanBuffer::~VulkanBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyBuffer(device, object, nullptr);

		MemoryAllocator::freeSuballocation(device, memorySuballocation.blockIndex);
	}

	void VulkanBuffer::unmap() const {
		MemoryAllocator::unmapSuballocation(device, memorySuballocation.blockIndex);
	}

	void* VulkanBuffer::getMappedRange(uint64 offset, uint64 size) const {
		return MemoryAllocator::mapSuballocation(device, memorySuballocation, size, offset);
	}
}