// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/VK/ImageType.hpp"
#include "Engine/Adapter/Graphics/VK/ImageUsage.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Image::Image(const ImageDescriptor& descriptor, const VK::Device& device) :
		Trident::Image(descriptor),
		device(device),
		swapchainImage(false),
		type(getImageType(descriptor.type)),
		format(VK::getFormat(descriptor.format))
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
		imageCreateInfo.usage = getImageUsageFlagBits(descriptor.usage);
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;

		// initialLayout must either be VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED.
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		const VkResult result = vkCreateImage(device.device, &imageCreateInfo, nullptr, &image);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create image ([]).", string_VkResult(result));

		VkMemoryRequirements memoryRequirements;

		vkGetImageMemoryRequirements(device.device, image, &memoryRequirements);

		VkMemoryPropertyFlags memoryPropertyFlags = static_cast<VkMemoryPropertyFlags>(descriptor.memoryProperties);
		uint32 memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ADAPTER_WARNING("Failed to find DEVICE_LOCAL|LAZILY_ALLOCATED memory for image. Falling back to DEVICE_LOCAL|HOST_VISIBLE memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ADAPTER_WARNING("Failed to find DEVICE_LOCAL|HOST_VISIBLE memory for image. Falling back to HOST_VISIBLE|HOST_COHERENT memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ADAPTER_WARNING("Failed to find DEVICE_LOCAL|HOST_VISIBLE memory for image. Falling back to DEVICE_LOCAL memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		ADAPTER_ASSERT(memoryTypeIndex != UNDEFINED_MEMORY_TYPE_INDEX, "Undefined memory type index.");

		memorySuballocation = MemoryAllocator::suballocateImage(device.device, image, memoryTypeIndex, memoryRequirements);
	}

	VK::Image::Image(const VK::Device& device, VkImage image, VkFormat format, Extent2D extent) :
		Trident::Image(createSwapchainImageDescriptor(extent)),
		image(image),
		device(device),
		swapchainImage(true),
		type(VK_IMAGE_TYPE_2D),
		format(format)
	{}

	VK::Image::~Image() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		if (swapchainImage) {
			return;
		}

		vkDestroyImage(device.device, image, nullptr);

		MemoryAllocator::freeSuballocation(device.device, memorySuballocation.allocationIndex);
	}

	ref<ImageView> VK::Image::createViewInternal(const ImageViewDescriptor& descriptor, const Trident::Image* image) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor, image);

		return createRef<VK::ImageView>(descriptor, image, device);
	}

	ImageDescriptor VK::Image::createSwapchainImageDescriptor(Extent2D extent) {
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