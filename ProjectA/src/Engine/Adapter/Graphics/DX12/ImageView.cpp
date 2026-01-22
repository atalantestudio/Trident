// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageView.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceDimension.hpp"

namespace ProjectA {
	DX12::ImageView::ImageView(const ImageViewDescriptor& descriptor, const ProjectA::Image* image, const DX12::Device& device) :
		ProjectA::ImageView(descriptor, image),
		image(static_cast<const DX12::Image*>(image)),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		if (this->image->usage == 0) {
			createSwapchainRenderTargetView();
		}
		else if (this->image->usage & ImageUsage::COLOR_ATTACHMENT) {
			createRenderTargetView();
		}
		else if (this->image->usage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			createDepthStencilView();
		}
	}

	void DX12::ImageView::createSwapchainRenderTargetView() {
		cpuDescriptorHandle = image->descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		cpuDescriptorHandle.ptr += image->swapchainImageIndex * device.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		device.device->CreateRenderTargetView(image->resource.Get(), nullptr, cpuDescriptorHandle);
	}

	void DX12::ImageView::createRenderTargetView() {
		// TODO: Find how to set the mip level count in the render target view descriptor.
		ADAPTER_ASSERT(mipLevelCount == 1, "Cannot create a view of a render target image with multiple mip levels.");

		cpuDescriptorHandle = image->descriptorHeap->GetCPUDescriptorHandleForHeapStart();

		D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDescriptor{};
		renderTargetViewDescriptor.Format = getImageFormat(image->getFormat());

		switch (image->getType()) {
			case ImageType::TEXTURE_2D: {
				if (image->getSampleCount() > 1) {
					if (layerCount > 1) {
						renderTargetViewDescriptor.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
						renderTargetViewDescriptor.Texture2DMSArray.FirstArraySlice = layerOffset;
						renderTargetViewDescriptor.Texture2DMSArray.ArraySize = layerCount;
					}
					else {
						renderTargetViewDescriptor.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
					}
				}
				else {
					if (layerCount > 1) {
						renderTargetViewDescriptor.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
						renderTargetViewDescriptor.Texture2DArray.MipSlice = mipLevelOffset;
						renderTargetViewDescriptor.Texture2DArray.FirstArraySlice = layerOffset;
						renderTargetViewDescriptor.Texture2DArray.ArraySize = layerCount;
						renderTargetViewDescriptor.Texture2DArray.PlaneSlice = 0;
					}
					else {
						renderTargetViewDescriptor.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
						renderTargetViewDescriptor.Texture2D.MipSlice = mipLevelOffset;
						renderTargetViewDescriptor.Texture2D.PlaneSlice = 0;
					}
				}

				break;
			}
		}

		device.device->CreateRenderTargetView(image->resource.Get(), &renderTargetViewDescriptor, cpuDescriptorHandle);
	}

	void DX12::ImageView::createDepthStencilView() {
		// TODO: Find how to set the mip level count in the render target view descriptor.
		ADAPTER_ASSERT(mipLevelCount == 1, "Cannot create a view of a depth stencil image with multiple mip levels.");

		cpuDescriptorHandle = image->descriptorHeap->GetCPUDescriptorHandleForHeapStart();

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescriptor{};
		depthStencilViewDescriptor.Format = getImageFormat(image->getFormat());
		depthStencilViewDescriptor.Flags = D3D12_DSV_FLAG_NONE;

		switch (image->getType()) {
			case ImageType::TEXTURE_2D: {
				if (image->getSampleCount() > 1) {
					if (layerCount > 1) {
						depthStencilViewDescriptor.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
						depthStencilViewDescriptor.Texture2DMSArray.FirstArraySlice = layerOffset;
						depthStencilViewDescriptor.Texture2DMSArray.ArraySize = layerCount;
					}
					else {
						depthStencilViewDescriptor.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
					}
				}
				else {
					if (layerCount > 1) {
						depthStencilViewDescriptor.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
						depthStencilViewDescriptor.Texture2DArray.MipSlice = mipLevelOffset;
						depthStencilViewDescriptor.Texture2DArray.FirstArraySlice = layerOffset;
						depthStencilViewDescriptor.Texture2DArray.ArraySize = layerCount;
					}
					else {
						depthStencilViewDescriptor.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
						depthStencilViewDescriptor.Texture2D.MipSlice = mipLevelOffset;
					}
				}

				break;
			}
		}

		device.device->CreateDepthStencilView(image->resource.Get(), &depthStencilViewDescriptor, cpuDescriptorHandle);
	}
}