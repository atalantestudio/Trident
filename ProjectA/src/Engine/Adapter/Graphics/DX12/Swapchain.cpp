// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Swapchain::Swapchain(const SwapchainDescriptor& descriptor, const DX12::Instance& instance, const DX12::Surface& surface, const DX12::Device& device) :
		ProjectA::Swapchain(descriptor),
		instance(instance),
		surface(surface),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		recreate(descriptor);

		const HRESULT result = instance.factory->MakeWindowAssociation(surface.window, DXGI_MWA_NO_ALT_ENTER);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to disable fullscreen transitions.");	
	}

	uint32 DX12::Swapchain::getImageCount() const {
		return imageCount;
	}

	void DX12::Swapchain::update(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (const ref<ProjectA::ImageView> imageView : imageViews) {
			imageView.~shared_ptr();
		}

		recreate(descriptor);
	}

	ref<DX12::Image> DX12::Swapchain::getImage(uint32 imageIndex) const {
		return std::static_pointer_cast<DX12::Image>(images[imageIndex]);
	}

	void DX12::Swapchain::recreate(const SwapchainDescriptor& descriptor) {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		HRESULT result;

		extent = descriptor.extent;

		DXGI_SWAP_CHAIN_DESC1 swapchainDescriptor{};
		swapchainDescriptor.Width = descriptor.extent.width;
		swapchainDescriptor.Height = descriptor.extent.height;
		swapchainDescriptor.Format = getFormat(surface.getFormat());
		swapchainDescriptor.Stereo = false;
		swapchainDescriptor.SampleDesc.Count = 1;
		swapchainDescriptor.SampleDesc.Quality = 0;
		swapchainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDescriptor.BufferCount = imageCount;
		swapchainDescriptor.Scaling = DXGI_SCALING_STRETCH;
		swapchainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDescriptor.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapchainDescriptor.Flags = 0;

		const ref<DX12::Queue> graphicsQueue = std::static_pointer_cast<DX12::Queue>(device.getQueue(QueueFamilyCapability::GRAPHICS));

		ComPtr<IDXGISwapChain1> swapChain1;

		result = instance.factory->CreateSwapChainForHwnd(graphicsQueue->commandQueue.Get(), surface.window, &swapchainDescriptor, nullptr, nullptr, &swapChain1);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create swapchain.");

		result = swapChain1.As(&swapChain);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to move swapchain.");

		D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDescriptor{};
		rtvDescriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDescriptorHeapDescriptor.NumDescriptors = imageCount;
		rtvDescriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDescriptorHeapDescriptor.NodeMask = 0;

		result = device.device->CreateDescriptorHeap(&rtvDescriptorHeapDescriptor, IID_PPV_ARGS(&rtvDescriptorHeap));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create RTV descriptor heap.");

		rtvDescriptorSize = device.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		images = sequence<ref<ProjectA::Image>>(imageCount);
		imageViews = sequence<ref<ProjectA::ImageView>>(imageCount);

		uint32 imageIndex = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvDescriptorHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		while (imageIndex < imageCount) {
			images[imageIndex] = createRef<DX12::Image>(device, *this, imageIndex, rtvDescriptorHandle);

			rtvDescriptorHandle.ptr += rtvDescriptorSize;
			imageIndex += 1;
		}
	}
}