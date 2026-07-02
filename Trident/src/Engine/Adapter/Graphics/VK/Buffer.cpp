// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/BufferUsage.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Buffer::Buffer(const Trident::Device* device, const BufferDescriptor& descriptor) :
		Trident::Buffer(device, descriptor)
	{}

	ref<Trident::Buffer> VK::createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const ref<VK::MemoryAllocator> memoryAllocator = std::static_pointer_cast<VK::MemoryAllocator>(device->getMemoryAllocator());
		VkResult result;

		ref<VK::Buffer> buffer = createRef<VK::Buffer>(device, descriptor);

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = buffer->getSize();
		bufferCreateInfo.usage = getBufferUsageFlagBits(descriptor.usage);
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		{
			PROFILE_SCOPE("vkCreateBuffer");

			result = _device->vkCreateBuffer(_device->device, &bufferCreateInfo, nullptr, &buffer->buffer);
		}

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create buffer ([]).", string_VkResult(result));

		_device->vkGetBufferMemoryRequirements(_device->device, buffer->buffer, &buffer->memoryRequirements);

		VkMemoryPropertyFlags memoryPropertyFlags = static_cast<VkMemoryPropertyFlags>(buffer->memorySuballocation.memoryProperties);
		uint32 memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(buffer->memoryRequirements.memoryTypeBits, memoryPropertyFlags);

		if (memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && memoryTypeIndex == max<uint32>()) {
			ADAPTER_WARNING("Failed to find [] memory for buffer. Falling back to HOST_VISIBLE|HOST_COHERENT memory.", string_VkMemoryPropertyFlags(memoryPropertyFlags));

			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(buffer->memoryRequirements.memoryTypeBits, memoryPropertyFlags);
		}

		if (memoryPropertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) && memoryTypeIndex == max<uint32>()) {
			ADAPTER_WARNING("Failed to find [] memory for buffer. Falling back to DEVICE_LOCAL memory.", string_VkMemoryPropertyFlags(memoryPropertyFlags));

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(buffer->memoryRequirements.memoryTypeBits, memoryPropertyFlags);
		}

		ADAPTER_ASSERT(memoryTypeIndex != max<uint32>(), "Undefined memory type index.");

		buffer->memorySuballocation.memoryProperties = static_cast<MemoryProperty>(memoryPropertyFlags);

		#if defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_BUFFER;
				debugUtilsObjectNameInfo.objectHandle = (uint64) buffer->buffer;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = _device->vkSetDebugUtilsObjectNameEXT(_device->device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif

		return buffer;
	}

	void VK::destroyBuffer(const Trident::Device* device, const Trident::Buffer* buffer) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const ref<VK::MemoryAllocator> memoryAllocator = std::static_pointer_cast<VK::MemoryAllocator>(device->getMemoryAllocator());
		const VK::Buffer* _buffer = static_cast<const VK::Buffer*>(buffer);

		_device->vkDestroyBuffer(_device->device, _buffer->buffer, nullptr);

		memoryAllocator->freeSuballocation(buffer->getMemorySuballocation().allocationIndex);
	}

	void* VK::mapBuffer(const Trident::Device* device, Trident::Buffer* buffer, uint64 offset, uint64 size) {
		const ref<VK::MemoryAllocator> memoryAllocator = std::static_pointer_cast<VK::MemoryAllocator>(device->getMemoryAllocator());

		return memoryAllocator->mapSuballocation(buffer->getMemorySuballocation(), offset, size);
	}

	void VK::unmapBuffer(const Trident::Device* device, Trident::Buffer* buffer) {
		const ref<VK::MemoryAllocator> memoryAllocator = std::static_pointer_cast<VK::MemoryAllocator>(device->getMemoryAllocator());

		memoryAllocator->unmapSuballocation(buffer->getMemorySuballocation().allocationIndex);
	}
}