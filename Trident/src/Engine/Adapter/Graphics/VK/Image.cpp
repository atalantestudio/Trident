// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/ImageAspectFlags.hpp"
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
		const ref<VK::MemoryAllocator> memoryAllocator = static_pointer_cast<VK::MemoryAllocator>(device->getMemoryAllocator());
		VkResult result;

		ref<VK::Image> image = allocateRef<VK::Image>(device, descriptor);
		image->type = getImageType(descriptor.type);
		image->format = getFormat(descriptor.format);
		image->aspectFlags = getImageAspectFlags(image->format);
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

		// NOTE: The initial layout must either be UNDEFINED or PREINITIALIZED.
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		result = _device->vkCreateImage(_device->device, &imageCreateInfo, nullptr, &image->image);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create image ([]).", string_VkResult(result));

		_device->vkGetImageMemoryRequirements(_device->device, image->image, &image->memoryRequirements);

		VkMemoryPropertyFlags memoryPropertyFlags = static_cast<VkMemoryPropertyFlags>(image->memorySuballocation.memoryProperties);
		uint32 memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(image->memoryRequirements.memoryTypeBits, memoryPropertyFlags);

		if (memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) && memoryTypeIndex == max<uint32>()) {
			#if defined(BUILD_MODE_DEBUG)
				ADAPTER_WARNING("Failed to find [] memory for image. Falling back to DEVICE_LOCAL|HOST_VISIBLE memory.", string_VkMemoryPropertyFlags(memoryPropertyFlags));
			#else
				ADAPTER_WARNING("Failed to find adequate memory for image. Falling back to DEVICE_LOCAL|HOST_VISIBLE memory.");
			#endif

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(image->memoryRequirements.memoryTypeBits, memoryPropertyFlags);
		}

		if (memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && memoryTypeIndex == max<uint32>()) {
			#if defined(BUILD_MODE_DEBUG)
				ADAPTER_WARNING("Failed to find [] memory for image. Falling back to HOST_VISIBLE|HOST_COHERENT memory.", string_VkMemoryPropertyFlags(memoryPropertyFlags));
			#else
				ADAPTER_WARNING("Failed to find adequate memory for image. Falling back to HOST_VISIBLE|HOST_COHERENT memory.");
			#endif

			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(image->memoryRequirements.memoryTypeBits, memoryPropertyFlags);
		}

		if (memoryPropertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) && memoryTypeIndex == max<uint32>()) {
			#if defined(BUILD_MODE_DEBUG)
				ADAPTER_WARNING("Failed to find [] memory for image. Falling back to DEVICE_LOCAL memory.", string_VkMemoryPropertyFlags(memoryPropertyFlags));
			#else
				ADAPTER_WARNING("Failed to find adequate memory for image. Falling back to DEVICE_LOCAL memory.");
			#endif

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(image->memoryRequirements.memoryTypeBits, memoryPropertyFlags);
		}

		ADAPTER_ASSERT(memoryTypeIndex != max<uint32>(), "Undefined memory type index.");

		image->memorySuballocation.memoryProperties = static_cast<MemoryProperty>(memoryPropertyFlags);

		#if defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_IMAGE;
				debugUtilsObjectNameInfo.objectHandle = (uint64) image->image;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = _device->vkSetDebugUtilsObjectNameEXT(_device->device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif

		return image;
	}

	ref<Trident::Image> VK::createImage(const Trident::Device* device, VkImage vkImage, VkFormat format, Extent2D extent, uint32 index) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ImageDescriptor descriptor = createSwapchainImageDescriptor(extent);

		#if defined(DEBUG_OBJECT_NAME)
			const sequence<char8> name = Logger::format("Swapchain image []", index);

			descriptor.name = name;
		#endif

		ref<VK::Image> image = allocateRef<VK::Image>(device, descriptor);
		image->image = vkImage;
		image->type = VK_IMAGE_TYPE_2D;
		image->format = format;
		image->aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		image->memorySuballocation = {};
		image->swapchainImage = true;

		return image;
	}

	void VK::destroyImage(const Trident::Device* device, const Trident::Image* image) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Image* _image = static_cast<const VK::Image*>(image);

		if (_image->swapchainImage) {
			return;
		}

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const ref<VK::MemoryAllocator> memoryAllocator = static_pointer_cast<VK::MemoryAllocator>(device->getMemoryAllocator());

		_device->vkDestroyImage(_device->device, _image->image, nullptr);

		memoryAllocator->freeSuballocation(image->getMemorySuballocation().allocationIndex);
	}
}