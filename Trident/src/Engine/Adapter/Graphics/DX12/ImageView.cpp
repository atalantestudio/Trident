// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageView.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceDimension.hpp"

namespace Trident {
	static void createSwapchainRenderTargetView(ref<DX12::ImageView> imageView, const DX12::Image* image, ref<DX12::MemoryAllocator> memoryAllocator) {
		imageView->descriptorHandle = memoryAllocator->allocateRenderTargetView(image->resource, nullptr);
	}

	static void createRenderTargetView(ref<DX12::ImageView> imageView, const DX12::Image* image, ref<DX12::MemoryAllocator> memoryAllocator) {
		D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDescriptor{};
		renderTargetViewDescriptor.Format = DX12::getImageFormat(image->getFormat());

		switch (image->getType()) {
			case ImageType::TYPE_2D: {
				if (image->getSampleCount() > 1) {
					if (imageView->layerCount > 1) {
						renderTargetViewDescriptor.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
						renderTargetViewDescriptor.Texture2DMSArray.FirstArraySlice = imageView->layerOffset;
						renderTargetViewDescriptor.Texture2DMSArray.ArraySize = imageView->layerCount;
					}
					else {
						renderTargetViewDescriptor.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
					}
				}
				else {
					if (imageView->layerCount > 1) {
						renderTargetViewDescriptor.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
						renderTargetViewDescriptor.Texture2DArray.MipSlice = imageView->mipLevelOffset;
						renderTargetViewDescriptor.Texture2DArray.FirstArraySlice = imageView->layerOffset;
						renderTargetViewDescriptor.Texture2DArray.ArraySize = imageView->layerCount;
						renderTargetViewDescriptor.Texture2DArray.PlaneSlice = 0;
					}
					else {
						renderTargetViewDescriptor.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
						renderTargetViewDescriptor.Texture2D.MipSlice = imageView->mipLevelOffset;
						renderTargetViewDescriptor.Texture2D.PlaneSlice = 0;
					}
				}

				break;
			}
		}

		imageView->descriptorHandle = memoryAllocator->allocateRenderTargetView(image->resource, &renderTargetViewDescriptor);
	}

	static void createDepthStencilView(ref<DX12::ImageView> imageView, const DX12::Image* image, ref<DX12::MemoryAllocator> memoryAllocator) {
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescriptor{};
		depthStencilViewDescriptor.Format = DX12::getImageFormat(image->getFormat());
		depthStencilViewDescriptor.Flags = D3D12_DSV_FLAG_NONE;

		switch (image->getType()) {
			case ImageType::TYPE_2D: {
				if (image->getSampleCount() > 1) {
					if (imageView->layerCount > 1) {
						depthStencilViewDescriptor.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
						depthStencilViewDescriptor.Texture2DMSArray.FirstArraySlice = imageView->layerOffset;
						depthStencilViewDescriptor.Texture2DMSArray.ArraySize = imageView->layerCount;
					}
					else {
						depthStencilViewDescriptor.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
					}
				}
				else {
					if (imageView->layerCount > 1) {
						depthStencilViewDescriptor.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
						depthStencilViewDescriptor.Texture2DArray.MipSlice = imageView->mipLevelOffset;
						depthStencilViewDescriptor.Texture2DArray.FirstArraySlice = imageView->layerOffset;
						depthStencilViewDescriptor.Texture2DArray.ArraySize = imageView->layerCount;
					}
					else {
						depthStencilViewDescriptor.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
						depthStencilViewDescriptor.Texture2D.MipSlice = imageView->mipLevelOffset;
					}
				}

				break;
			}
		}

		imageView->descriptorHandle = memoryAllocator->allocateDepthStencilView(image->resource, &depthStencilViewDescriptor);
	}

	DX12::ImageView::ImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor) :
		Trident::ImageView(device, image, descriptor)
	{}

	ref<Trident::ImageView> DX12::createImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor) {
		const DX12::Image* _image = static_cast<const DX12::Image*>(image);
		const ref<DX12::MemoryAllocator> memoryAllocator = ref_cast<DX12::MemoryAllocator>(device->getMemoryAllocator());

		ref<DX12::ImageView> imageView = allocateRef<DX12::ImageView>(device, image, descriptor);

		if (_image->usage == 0) {
			createSwapchainRenderTargetView(imageView, _image, memoryAllocator);
		}
		else if (_image->usage & ImageUsage::COLOR_ATTACHMENT) {
			createRenderTargetView(imageView, _image, memoryAllocator);
		}
		else if (_image->usage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			createDepthStencilView(imageView, _image, memoryAllocator);
		}

		return imageView;
	}

	void DX12::destroyImageView(const Trident::Device* device, const Trident::ImageView* imageView) {}
}