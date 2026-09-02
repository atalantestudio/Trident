// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/ImageView.hpp"

namespace Trident {
	ImageView::ImageView(const Device* device, Image* image, const ImageViewDescriptor& descriptor) :
		device(device),
		image(image),
		type(descriptor.type),
		mipLevelOffset(descriptor.mipLevelOffset),
		mipLevelCount(descriptor.mipLevelCount),
		layerOffset(descriptor.layerOffset),
		layerCount(descriptor.layerCount)
	{}

	ImageView::~ImageView() {
		device->adapter->destroyImageViewImplementation(device, this);
	}

	Image* ImageView::getImage() {
		return image;
	}

	ImageViewType ImageView::getType() const {
		return type;
	}

	uint32 ImageView::getMipLevelOffset() const {
		return mipLevelOffset;
	}

	uint32 ImageView::getMipLevelCount() const {
		return mipLevelCount;
	}

	uint32 ImageView::getLayerOffset() const {
		return layerOffset;
	}

	uint32 ImageView::getLayerCount() const {
		return layerCount;
	}
}