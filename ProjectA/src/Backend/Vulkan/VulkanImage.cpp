#include "pch.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"
#include "Backend/Vulkan/ImageType.hpp"
#include "Backend/Vulkan/ImageUsage.hpp"
#include "Backend/Vulkan/VulkanImage.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

namespace ProjectA {
	VulkanImage::VulkanImage(const VulkanDevice& device, const ImageDescriptor& descriptor) :
		device(device.object),
		type(descriptor.type),
		format(descriptor.format),
		width(descriptor.width),
		height(descriptor.height)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_ASSERT(descriptor.sampleCount >= 1 && descriptor.sampleCount <= 64, "Invalid sample count ([]).", descriptor.sampleCount);

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = getVkImageType(type);
		imageCreateInfo.format = getVkFormat(format);
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = descriptor.depth;
		imageCreateInfo.mipLevels = descriptor.mipLevelCount;
		imageCreateInfo.arrayLayers = descriptor.arrayLayerCount;
		imageCreateInfo.samples = static_cast<VkSampleCountFlagBits>(BIT((descriptor.sampleCount - 1)));
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = getVkImageUsageFlagBits(descriptor.usage);
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;

		// initialLayout must be VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED.
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VkResult result = vkCreateImage(device.object, &imageCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image (code: []).", result);

		VkMemoryRequirements memoryRequirements;

		vkGetImageMemoryRequirements(device.object, object, &memoryRequirements);

		// TODO: Fix memory type not being found on HP Victus (laptop).
		// The only way to get a memory type index is by specifying a memory property flag of 0.
		// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT avoids caching.
		const uint32 memoryTypeIndex = device.findMemoryType(memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		result = vkAllocateMemory(device.object, &memoryAllocateInfo, nullptr, &memory);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to allocate Vulkan memory (code: []).", result);

		result = vkBindImageMemory(device.object, object, memory, 0);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to bind Vulkan image memory (code: []).", result);
	}

	VulkanImage::~VulkanImage() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyImage(device, object, nullptr);
		vkFreeMemory(device, memory, nullptr);
	}

	uint32 VulkanImage::getWidth() const {
		return width;
	}

	uint32 VulkanImage::getHeight() const {
		return height;
	}

	ref<ImageView> VulkanImage::createView(const ImageViewDescriptor& descriptor) const {
		return createRef<VulkanImageView>(device, *this, descriptor);
	}
}