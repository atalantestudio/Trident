#include "pch.hpp"

#include "Backend/Vulkan/VulkanImage.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"
#include "Backend/Vulkan/ImageType.hpp"
#include "Backend/Vulkan/ImageUsage.hpp"

namespace ProjectA {
	VulkanImage::VulkanImage(VkDevice device, const ImageDescriptor& descriptor) :
		Image(descriptor),
		device(device),
		swapchainImage(false),
		type(getVkImageType(descriptor.type)),
		format(getVkFormat(descriptor.format))
	{
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = type;
		imageCreateInfo.format = format;
		imageCreateInfo.extent.width = extent.width;
		imageCreateInfo.extent.height = extent.height;
		imageCreateInfo.extent.depth = extent.depth;
		imageCreateInfo.mipLevels = mipLevelCount;
		imageCreateInfo.arrayLayers = layerCount;
		imageCreateInfo.samples = static_cast<VkSampleCountFlagBits>(getSampleCount());
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
			ENGINE_WARNING("Failed to find DEVICE_LOCAL | LAZILY_ALLOCATED memory for Vulkan image. Falling back to DEVICE_LOCAL | HOST_VISIBLE memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ENGINE_WARNING("Failed to find DEVICE_LOCAL | HOST_VISIBLE memory for Vulkan image. Falling back to HOST_VISIBLE | HOST_COHERENT memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ENGINE_WARNING("Failed to find DEVICE_LOCAL | HOST_VISIBLE memory for Vulkan image. Falling back to DEVICE_LOCAL memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		DEBUG_ASSERT(memoryTypeIndex != UNDEFINED_MEMORY_TYPE_INDEX);

		memorySuballocation = MemoryAllocator::suballocateImage(device, object, memoryTypeIndex, memoryRequirements);
	}

	VulkanImage::VulkanImage(VkDevice device, VkImage image, VkFormat format, Extent2D extent) :
		Image(createSwapchainImageDescriptor(extent)),
		ObjectWrapper(image),
		device(device),
		swapchainImage(true),
		type(VK_IMAGE_TYPE_2D),
		format(format)
	{}

	VulkanImage::~VulkanImage() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		if (swapchainImage) {
			return;
		}

		vkDestroyImage(device, object, nullptr);

		MemoryAllocator::freeSuballocation(device, memorySuballocation.allocationIndex);
	}

	ref<ImageView> VulkanImage::createViewInternal(const Image* image, const ImageViewDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor, image);

		return createRef<VulkanImageView>(device, image, descriptor);
	}

	ImageDescriptor VulkanImage::createSwapchainImageDescriptor(Extent2D extent) {
		ImageDescriptor imageDescriptor{};
		imageDescriptor.extent.width = extent.width;
		imageDescriptor.extent.height = extent.height;
		imageDescriptor.extent.depth = 1;
		imageDescriptor.mipLevelCount = 1;
		imageDescriptor.layerCount = 1;

		// These properties aren't used when creating a swapchain image.
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = ImageFormat::B8G8R8A8_UNORM;
		imageDescriptor.sampleCount = 1;
		imageDescriptor.usage = static_cast<ImageUsage>(0);
		imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		return imageDescriptor;
	}
}