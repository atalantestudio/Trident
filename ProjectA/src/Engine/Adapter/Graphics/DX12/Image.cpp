// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/HeapType.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageView.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceDimension.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceFlags.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceStates.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Image::Image(const ImageDescriptor& descriptor, const DX12::Device& device) :
		ProjectA::Image(descriptor)
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
		const D3D12_RESOURCE_STATES resourceState = getImageResourceStates(descriptor.usage, layout);

		result = device.device->CreateCommittedResource(&heapProperties, heapFlags, &resourceDescriptor, resourceState, nullptr, IID_PPV_ARGS(&resource));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create committed image resource.");
	}

	DX12::Image::Image(const DX12::Device& device, const DX12::Swapchain& swapchain, uint32 imageIndex, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;

		result = swapchain.swapChain->GetBuffer(imageIndex, IID_PPV_ARGS(&resource));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get buffer for render target view.");

		device.device->CreateRenderTargetView(resource.Get(), nullptr, cpuDescriptorHandle);
	}

	ref<ProjectA::ImageView> DX12::Image::createViewInternal(const ImageViewDescriptor& descriptor, const ProjectA::Image* image) const {
		return createRef<DX12::ImageView>(descriptor, image);
	}
}