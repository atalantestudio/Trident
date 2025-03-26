#include "pch.hpp"

#include "Backend/Vulkan/TextureFormat.hpp"
#include "Backend/Vulkan/TextureType.hpp"
#include "Backend/Vulkan/TextureUsage.hpp"
#include "Backend/Vulkan/VulkanTexture.hpp"
#include "Backend/Vulkan/VulkanTextureView.hpp"

namespace ProjectA {
	VulkanTexture::VulkanTexture(const VulkanDevice& device, const TextureDescriptor& descriptor) :
		device(device),
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
		imageCreateInfo.samples = static_cast<VkSampleCountFlagBits>(1 << (descriptor.sampleCount - 1));
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

		// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT avoid caching.
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

	VulkanTexture::~VulkanTexture() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyImage(device.object, object, nullptr);
		vkFreeMemory(device.object, memory, nullptr);
	}

	uint32 VulkanTexture::getWidth() const {
		return width;
	}

	uint32 VulkanTexture::getHeight() const {
		return height;
	}

	ref<TextureView> VulkanTexture::createView(const TextureViewDescriptor& descriptor) const {
		return createRef<VulkanTextureView>(device.object, *this, descriptor);
	}
}