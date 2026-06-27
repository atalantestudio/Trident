// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/Image.hpp"

namespace Trident {
	uint32 calculateMaxMipLevelCount(Extent3D extent) {
		// ⌊log2(max(width, height, depth))⌋ + 1
		return static_cast<uint32>(floor(log2(max(extent.width, max(extent.height, extent.depth))))) + 1;
	}

	Image::Image(const Device* device, const ImageDescriptor& descriptor) :
		device(device),
		type(descriptor.type),
		format(descriptor.format),
		extent(descriptor.extent),
		mipLevelCount(descriptor.mipLevelCount),
		layerCount(descriptor.layerCount),
		sampleCount(descriptor.sampleCount),
		usage(descriptor.usage),
		memorySuballocation({
			.allocationIndex = max<uint32>(),
			.memoryProperties = descriptor.memoryProperties,
			.offset = 0,
			.size = 0,
		})
	{}

	Image::~Image() {
		device->adapter->destroyImageImplementation(device, this);
	}

	ImageType Image::getType() const {
		return type;
	}

	ImageFormat Image::getFormat() const {
		return format;
	}

	Extent3D Image::getExtent() const {
		return extent;
	}

	uint64 Image::getSize() const {
		return getChannelCount(format) * extent.width * extent.height * extent.depth * layerCount;
	}

	uint32 Image::getMipLevelCount() const {
		return mipLevelCount;
	}

	uint32 Image::getLayerCount() const {
		return layerCount;
	}

	uint8 Image::getSampleCount() const {
		return sampleCount;
	}

	ImageUsage Image::getUsage() const {
		return usage;
	}

	ImageLayout Image::getLayout() const {
		return layout;
	}

	Access Image::getAccess() const {
		return access;
	}

	MemorySuballocation Image::getMemorySuballocation() const {
		return memorySuballocation;
	}

	ref<ImageView> Image::createView(const ImageViewDescriptor& descriptor) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 warningCount = 0;
			uint32 errorCount = 0;

			if (descriptor.mipLevelOffset + descriptor.mipLevelCount > mipLevelCount) {
				ENGINE_ERROR("descriptor.mipLevelOffset and descriptor.mipLevelCount must represent a valid mip level range.");

				errorCount += 1;
			}

			if (descriptor.mipLevelCount == 0) {
				ENGINE_ERROR("descriptor.mipLevelCount must be greater than 0.");

				errorCount += 1;
			}

			if (descriptor.layerOffset + descriptor.layerCount > layerCount) {
				ENGINE_ERROR("descriptor.layerOffset and descriptor.layerCount must represent a valid layer range.");

				errorCount += 1;
			}

			if (descriptor.layerCount == 0) {
				ENGINE_ERROR("descriptor.layerCount must be greater than 0.");

				errorCount += 1;
			}

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this image view.");

				warningCount += 1;
			}

			ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
		#endif

		return device->adapter->createImageViewImplementation(device, this, descriptor);
	}
}