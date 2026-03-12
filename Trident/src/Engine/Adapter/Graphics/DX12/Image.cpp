// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/DX12/HeapType.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageView.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceDimension.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceFlags.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceState.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Rendering/Descriptor/ImageDescriptor.hpp"

namespace Trident {
	static ImageDescriptor createSwapchainImageDescriptor(Extent2D extent, UINT index) {
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

	DX12::Image::Image(const Trident::Device* device, const ImageDescriptor& descriptor) :
		Trident::Image(device, descriptor)
	{}

	ref<Trident::Image> DX12::createImage(const Trident::Device* device, const ImageDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		HRESULT result;

		ref<DX12::Image> image = createRef<DX12::Image>(device, descriptor);
		image->resourceState = getImageResourceState(descriptor.usage, image->layout, image->sampleCount);
		image->usage = descriptor.usage;

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = getHeapType(descriptor.usage, descriptor.memoryProperties);
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDescriptor{};
		resourceDescriptor.Dimension = getImageResourceDimension(descriptor.type);
		resourceDescriptor.Alignment = 0;
		resourceDescriptor.Width = descriptor.extent.width;
		resourceDescriptor.Height = descriptor.extent.height;
		resourceDescriptor.DepthOrArraySize = descriptor.extent.depth * descriptor.layerCount;
		resourceDescriptor.MipLevels = descriptor.mipLevelCount;
		resourceDescriptor.Format = getImageFormat(descriptor.format);
		resourceDescriptor.SampleDesc.Count = descriptor.sampleCount;
		resourceDescriptor.SampleDesc.Quality = 0;
		resourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDescriptor.Flags = getImageResourceFlags(descriptor.usage);

		const D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;
		const D3D12_CLEAR_VALUE* optimizedClearValuePointer = nullptr;

		D3D12_CLEAR_VALUE optimizedClearValue{};
		optimizedClearValue.Format = resourceDescriptor.Format;

		if (descriptor.usage & ImageUsage::COLOR_ATTACHMENT) {
			optimizedClearValue.Color[0] = 0.0f;
			optimizedClearValue.Color[1] = 0.0f;
			optimizedClearValue.Color[2] = 0.0f;
			optimizedClearValue.Color[3] = 1.0f;

			optimizedClearValuePointer = &optimizedClearValue;
		}
		else if (descriptor.usage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			optimizedClearValue.DepthStencil.Depth = 1.0f;
			optimizedClearValue.DepthStencil.Stencil = 0;

			optimizedClearValuePointer = &optimizedClearValue;
		}

		result = _device->device->CreateCommittedResource(&heapProperties, heapFlags, &resourceDescriptor, image->resourceState, optimizedClearValuePointer, IID_PPV_ARGS(&image->resource));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create committed image resource.");

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&descriptor.name[0]);

				result = image->resource->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set resource name.");

				free(name);
			}
		#endif

		return image;
	}

	ref<Trident::Image> DX12::createImage(const Trident::Device* device, const DX12::Swapchain* swapchain, UINT index) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Swapchain* _swapchain = static_cast<const DX12::Swapchain*>(swapchain);
		HRESULT result;

		ImageDescriptor descriptor = createSwapchainImageDescriptor(_swapchain->getExtent(), index);

		SET_DEBUG_NAME(descriptor, Logger::format("Swapchain image []", index));

		ref<DX12::Image> image = createRef<DX12::Image>(device, descriptor);
		image->resourceState = D3D12_RESOURCE_STATE_PRESENT;
		image->usage = static_cast<ImageUsage>(0);

		result = _swapchain->swapChain->GetBuffer(index, IID_PPV_ARGS(&image->resource));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get buffer for render target view.");

		#if defined(BUILD_MODE_DEBUG)
			const sequence<char8> descriptorName = Logger::format("Swapchain image []", index);
			const LPWSTR name = ConvertUTF8ToUTF16(&descriptorName[0]);

			result = image->resource->SetName(name);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set name on image resource.");

			free(name);
		#endif

		return image;
	}

	void DX12::destroyImage(const Trident::Device* device, const Trident::Image* image) {
		const DX12::Image* _image = static_cast<const DX12::Image*>(image);

		_image->resource->Release();
	}
}