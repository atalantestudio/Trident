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
	static ImageDescriptor createSwapchainImageDescriptor(Extent2D extent) {
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

	VK::Image::Image(const Trident::Device* device, const ImageDescriptor& descriptor) :
		Trident::Image(device, descriptor)
	{}

	ref<Trident::Image> VK::createImage(const Trident::Device* device, const ImageDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const ref<VK::MemoryAllocator> memoryAllocator = std::static_pointer_cast<VK::MemoryAllocator>(device->getMemoryAllocator());
		VkResult result;

		ref<VK::Image> image = createRef<VK::Image>(device, descriptor);
		image->type = getImageType(descriptor.type);
		image->format = getFormat(descriptor.format);
		image->swapchainImage = false;

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = image->type;
		imageCreateInfo.format = image->format;
		imageCreateInfo.extent.width = descriptor.extent.width;
		imageCreateInfo.extent.height = descriptor.extent.height;
		imageCreateInfo.extent.depth = descriptor.extent.depth;
		imageCreateInfo.mipLevels = descriptor.mipLevelCount;
		imageCreateInfo.arrayLayers = descriptor.layerCount;
		imageCreateInfo.samples = static_cast<VkSampleCountFlagBits>(descriptor.sampleCount);
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = getImageUsageFlagBits(descriptor.usage);
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;

		// initialLayout must either be VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED.
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		result = _device->vkCreateImage(_device->device, &imageCreateInfo, nullptr, &image->image);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create image ([]).", string_VkResult(result));

		VkMemoryRequirements memoryRequirements;

		_device->vkGetImageMemoryRequirements(_device->device, image->image, &memoryRequirements);

		VkMemoryPropertyFlags memoryPropertyFlags = static_cast<VkMemoryPropertyFlags>(descriptor.memoryProperties);
		uint32 memoryTypeIndex = memoryAllocator->findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ADAPTER_WARNING("Failed to find DEVICE_LOCAL|LAZILY_ALLOCATED memory for image. Falling back to DEVICE_LOCAL|HOST_VISIBLE memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			memoryTypeIndex = memoryAllocator->findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ADAPTER_WARNING("Failed to find DEVICE_LOCAL|HOST_VISIBLE memory for image. Falling back to HOST_VISIBLE|HOST_COHERENT memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			memoryTypeIndex = memoryAllocator->findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ADAPTER_WARNING("Failed to find DEVICE_LOCAL|HOST_VISIBLE memory for image. Falling back to DEVICE_LOCAL memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memoryTypeIndex = memoryAllocator->findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		ADAPTER_ASSERT(memoryTypeIndex != UNDEFINED_MEMORY_TYPE_INDEX, "Undefined memory type index.");

		image->memorySuballocation = memoryAllocator->suballocateImage(_device->device, image->image, memoryTypeIndex, memoryRequirements);

		return image;
	}

	ref<Trident::Image> VK::createImage(const Trident::Device* device, VkImage image, VkFormat format, Extent2D extent, uint32 index) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ImageDescriptor descriptor = createSwapchainImageDescriptor(extent);

		SET_DEBUG_NAME(descriptor, Logger::format("Swapchain image []", index));

		ref<VK::Image> _image = createRef<VK::Image>(device, descriptor);
		_image->image = image;
		_image->type = VK_IMAGE_TYPE_2D;
		_image->format = format;
		_image->memorySuballocation = {};
		_image->swapchainImage = true;

		return _image;
	}

	void VK::destroyImage(const Trident::Device* device, const Trident::Image* image) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Image* _image = static_cast<const VK::Image*>(image);

		if (_image->swapchainImage) {
			return;
		}

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const ref<VK::MemoryAllocator> memoryAllocator = std::static_pointer_cast<VK::MemoryAllocator>(device->getMemoryAllocator());

		_device->vkDestroyImage(_device->device, _image->image, nullptr);

		memoryAllocator->freeSuballocation(_device->device, _image->memorySuballocation.allocationIndex);
	}
}