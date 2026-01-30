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

namespace Trident {
	ImageDescriptor DX12::Image::createSwapchainImageDescriptor(Extent2D extent) {
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

	DX12::Image::Image(const ImageDescriptor& descriptor, const DX12::Device& device, ref<DX12::MemoryAllocator> memoryAllocator) :
		Trident::Image(descriptor),
		resourceState(getImageResourceState(descriptor.usage, layout, getSampleCount())),
		usage(descriptor.usage),
		device(device),
		memoryAllocator(memoryAllocator)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = getHeapType(descriptor.memoryProperties);
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

		if (isDepthFormat(descriptor.format)) {
			optimizedClearValue.DepthStencil.Depth = 1.0f;
			optimizedClearValue.DepthStencil.Stencil = 0;

			optimizedClearValuePointer = &optimizedClearValue;
		}
		else if (descriptor.usage & ImageUsage::COLOR_ATTACHMENT || descriptor.usage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			optimizedClearValue.Color[0] = 0.0f;
			optimizedClearValue.Color[1] = 0.0f;
			optimizedClearValue.Color[2] = 0.0f;
			optimizedClearValue.Color[3] = 1.0f;

			optimizedClearValuePointer = &optimizedClearValue;
		}

		result = device.device->CreateCommittedResource(&heapProperties, heapFlags, &resourceDescriptor, resourceState, optimizedClearValuePointer, IID_PPV_ARGS(&resource));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create committed image resource.");

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&descriptor.name[0]);

				result = resource->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set name on image resource.");

				free(name);
			}
		#endif
	}

	DX12::Image::Image(const DX12::Device& device, const DX12::Swapchain& swapchain, UINT index, ref<DX12::MemoryAllocator> memoryAllocator) :
		Trident::Image(createSwapchainImageDescriptor(swapchain.getExtent())),
		resourceState(D3D12_RESOURCE_STATE_PRESENT),
		usage(static_cast<ImageUsage>(0)),
		device(device),
		memoryAllocator(memoryAllocator)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;

		result = swapchain.swapChain->GetBuffer(index, IID_PPV_ARGS(&resource));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get buffer for render target view.");

		#if defined(BUILD_MODE_DEBUG)
			const sequence<char8> descriptorName = Logger::format("Swapchain image []", index);
			const LPWSTR name = ConvertUTF8ToUTF16(&descriptorName[0]);

			result = resource->SetName(name);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set name on image resource.");

			free(name);
		#endif
	}

	DX12::Image::~Image() {
		resource->Release();
	}

	ref<Trident::ImageView> DX12::Image::createViewInternal(const ImageViewDescriptor& descriptor, Trident::Image* image) {
		return createRef<DX12::ImageView>(descriptor, image, memoryAllocator);
	}
}