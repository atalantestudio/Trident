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

	ref<Trident::Buffer> VK::createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor, UINT stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		VkResult result;

		ref<VK::Buffer> buffer = createRef<VK::Buffer>(device, descriptor);
		buffer->stride = stride;

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = getBufferUsageFlagBits(descriptor.usage);
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		{
			PROFILE_SCOPE("vkCreateBuffer");

			result = vkCreateBuffer(_device->device, &bufferCreateInfo, nullptr, &buffer->buffer);
		}

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create buffer ([]).", string_VkResult(result));

		VkMemoryRequirements memoryRequirements;

		vkGetBufferMemoryRequirements(_device->device, buffer->buffer, &memoryRequirements);

		VkMemoryPropertyFlags memoryPropertyFlags = static_cast<VkMemoryPropertyFlags>(descriptor.memoryProperties);
		uint32 memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ADAPTER_WARNING("Failed to find DEVICE_LOCAL|HOST_VISIBLE memory for image. Falling back to HOST_VISIBLE|HOST_COHERENT memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ADAPTER_WARNING("Failed to find HOST_VISIBLE|HOST_COHERENT memory for image. Falling back to DEVICE_LOCAL memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		ADAPTER_ASSERT(memoryTypeIndex != UNDEFINED_MEMORY_TYPE_INDEX, "Undefined memory type index.");

		memorySuballocation = MemoryAllocator::suballocateBuffer(_device->device, buffer->buffer, memoryTypeIndex, memoryRequirements);

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_BUFFER;
				debugUtilsObjectNameInfo.objectHandle = (uint64) buffer->buffer;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = vkSetDebugUtilsObjectNameEXT(_device->device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif

		return buffer;
	}

	void VK::destroyBuffer(const Trident::Device* device, const Trident::Buffer* buffer) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Buffer* _buffer = static_cast<const VK::Buffer*>(buffer);

		vkDestroyBuffer(_device->device, _buffer->buffer, nullptr);

		MemoryAllocator::freeSuballocation(_device->device, _buffer->memorySuballocation.allocationIndex);
	}

	void* VK::mapBuffer(const Trident::Device* device, const Trident::Buffer* buffer, uint64 offset, uint64 size) {
		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Buffer* _buffer = static_cast<const VK::Buffer*>(buffer);

		return MemoryAllocator::mapSuballocation(_device->device, _buffer->memorySuballocation, offset, size);
	}

	void VK::unmapBuffer(const Trident::Device* device, const Trident::Buffer* buffer) {
		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Buffer* _buffer = static_cast<const VK::Buffer*>(buffer);

		MemoryAllocator::unmapSuballocation(_device->device, _buffer->memorySuballocation.allocationIndex);
	}
}