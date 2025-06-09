#include "pch.hpp"

#include "Backend/Vulkan/VulkanImage.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"
#include "Backend/Vulkan/ImageType.hpp"
#include "Backend/Vulkan/ImageUsage.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/API/Rendering/SampleCount.hpp"
#endif

namespace ProjectA {
	VulkanImage::VulkanImage(VkDevice device, const ImageDescriptor& descriptor) :
		device(device)
	{
		DEBUG_ASSERT(checkSampleCountValidity(descriptor.sampleCount));

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = getVkImageType(descriptor.type);
		imageCreateInfo.format = getVkFormat(descriptor.format);
		imageCreateInfo.extent.width = descriptor.width;
		imageCreateInfo.extent.height = descriptor.height;
		imageCreateInfo.extent.depth = descriptor.depth;
		imageCreateInfo.mipLevels = descriptor.mipLevelCount;
		imageCreateInfo.arrayLayers = descriptor.arrayLayerCount;
		imageCreateInfo.samples = static_cast<VkSampleCountFlagBits>(descriptor.sampleCount);
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = getVkImageUsageFlagBits(descriptor.usage);
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;

		// initialLayout must either be VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED.
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		const VkResult result = vkCreateImage(device, &imageCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image ([]).", string_VkResult(result));

		VkMemoryRequirements memoryRequirements;

		vkGetImageMemoryRequirements(device, object, &memoryRequirements);

		VkMemoryPropertyFlags memoryPropertyFlags = static_cast<VkMemoryPropertyFlags>(descriptor.memoryProperties);
		uint32 memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ENGINE_WARN("Failed to find DEVICE_LOCAL | LAZILY_ALLOCATED memory for Vulkan image. Falling back to DEVICE_LOCAL | HOST_VISIBLE memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ENGINE_WARN("Failed to find DEVICE_LOCAL | HOST_VISIBLE memory for Vulkan image. Falling back to HOST_VISIBLE | HOST_COHERENT memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ENGINE_WARN("Failed to find DEVICE_LOCAL | HOST_VISIBLE memory for Vulkan image. Falling back to DEVICE_LOCAL memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		DEBUG_ASSERT(memoryTypeIndex != UNDEFINED_MEMORY_TYPE_INDEX);

		memorySuballocation = MemoryAllocator::suballocateImage(device, object, memoryTypeIndex, memoryRequirements);

		type = imageCreateInfo.imageType;
		format = imageCreateInfo.format;
		width = imageCreateInfo.extent.width;
		height = imageCreateInfo.extent.height;
		mipLevels = imageCreateInfo.mipLevels;
	}

	VulkanImage::~VulkanImage() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyImage(device, object, nullptr);

		MemoryAllocator::freeSuballocation(device, memorySuballocation.allocationIndex);
	}

	uint32 VulkanImage::getWidth() const {
		return width;
	}

	uint32 VulkanImage::getHeight() const {
		return height;
	}

	ref<ImageView> VulkanImage::createView(const ImageViewDescriptor& descriptor) const {
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		return createRef<VulkanImageView>(device, *this, descriptor);
	}
}