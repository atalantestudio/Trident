// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/Surface.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::Swapchain::Swapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor) :
		Trident::Swapchain(device, descriptor)
	{}

	ref<Trident::Swapchain> DX12::createSwapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<DX12::Swapchain> swapchain = createRef<DX12::Swapchain>(device, descriptor);

		updateSwapchain(device, swapchain.get(), descriptor);

		return swapchain;
	}

	void DX12::destroySwapchain(const Trident::Device* device, const Trident::Swapchain* swapchain) {
		const DX12::Swapchain* _swapchain = static_cast<const DX12::Swapchain*>(swapchain);

		for (const ref<Trident::ImageView>& imageView : _swapchain->imageViews) {
			imageView.~shared_ptr();
		}

		for (const ref<Trident::Image>& image : _swapchain->images) {
			image.~shared_ptr();
		}

		_swapchain->swapChain->Release();
	}

	void DX12::recreateSwapchain(const Trident::Device* device, Trident::Swapchain* swapchain, const SwapchainDescriptor& descriptor) {
		destroySwapchain(device, swapchain);
		updateSwapchain(device, swapchain, descriptor);
	}

	void DX12::updateSwapchain(const Trident::Device* device, Trident::Swapchain* swapchain, const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		const ref<DX12::Surface> surface = std::static_pointer_cast<DX12::Surface>(descriptor.surface);
		HRESULT result;

		DX12::Swapchain* _swapchain = static_cast<DX12::Swapchain*>(swapchain);
		_swapchain->syncInterval = static_cast<UINT>(descriptor.verticalSync);

		swapchain->imageCount = descriptor.imageCount;
		swapchain->extent = descriptor.extent;

		DXGI_SWAP_CHAIN_DESC1 swapchainDescriptor{};
		swapchainDescriptor.Width = descriptor.extent.width;
		swapchainDescriptor.Height = descriptor.extent.height;
		swapchainDescriptor.Format = getImageFormat(surface->getFormat());
		swapchainDescriptor.Stereo = false;
		swapchainDescriptor.SampleDesc.Count = 1;
		swapchainDescriptor.SampleDesc.Quality = 0;
		swapchainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDescriptor.BufferCount = swapchain->imageCount;
		swapchainDescriptor.Scaling = DXGI_SCALING_STRETCH;
		swapchainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDescriptor.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapchainDescriptor.Flags = 0;

		const ref<DX12::Queue> graphicsQueue = std::static_pointer_cast<DX12::Queue>(device->getQueue(QueueFamilyCapability::GRAPHICS));

		IDXGISwapChain1* swapChain1;

		{
			PROFILE_SCOPE("CreateSwapChainForHwnd");

			result = _device->factory->CreateSwapChainForHwnd(graphicsQueue->commandQueue, surface->window, &swapchainDescriptor, nullptr, nullptr, &swapChain1);
		}

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create swapchain.");

		result = swapChain1->QueryInterface(&_swapchain->swapChain);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to query swapchain interface.");

		swapChain1->Release();

		swapchain->images = sequence<ref<Trident::Image>>(swapchain->imageCount);
		swapchain->imageViews = sequence<ref<Trident::ImageView>>(swapchain->imageCount);

		UINT imageIndex = 0;

		while (imageIndex < swapchain->imageCount) {
			swapchain->images[imageIndex] = createImage(device, _swapchain, imageIndex);

			imageIndex += 1;
		}

		swapchain->createImageViews();
	}
}