#include "pch.hpp"

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanImage.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"
#include "Backend/Vulkan/ImageType.hpp"
#include "Backend/Vulkan/ImageUsage.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/API/Rendering/SampleCount.hpp"
#endif

namespace ProjectA {
	VulkanImage::VulkanImage(const VulkanDevice& device, const VkImageCreateInfo& createInfo) {
		construct(device, createInfo);
	}

	VulkanImage::VulkanImage(const VulkanDevice& device, const ImageDescriptor& descriptor) {
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

		construct(device, imageCreateInfo);
	}

	void VulkanImage::construct(const VulkanDevice& device, const VkImageCreateInfo& createInfo) {
		PROFILE_SCOPE(FUNC_SIG);

		VkResult result = vkCreateImage(device.object, &createInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image (code: []).", result);

		VkMemoryRequirements memoryRequirements;

		vkGetImageMemoryRequirements(device.object, object, &memoryRequirements);

		// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT avoids caching.
		// [HP Victus laptop] TODO: Fix memory type not being found on HP Victus (laptop).
		//                    The only way to get a memory type index is by specifying a memory property flag of 0.
		uint32 memoryTypeIndex = device.findMemoryType(memoryRequirements, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);

		if (memoryTypeIndex == std::numeric_limits<uint32>::max()) {
			// Fall back to device local memory.
			ENGINE_WARN("Failed to find lazily allocated memory for Vulkan image. Falling back to device local memory.");

			memoryTypeIndex = device.findMemoryType(memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		}

		DEBUG_ASSERT(memoryTypeIndex != std::numeric_limits<uint32>::max());

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		result = vkAllocateMemory(device.object, &memoryAllocateInfo, nullptr, &memory);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to allocate Vulkan memory (code: []).", result);

		result = vkBindImageMemory(device.object, object, memory, 0);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to bind Vulkan image memory (code: []).", result);

		this->device = device.object;
		type = createInfo.imageType;
		format = createInfo.format;
		width = createInfo.extent.width;
		height = createInfo.extent.height;
		mipLevels = createInfo.mipLevels;
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