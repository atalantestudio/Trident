// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::Swapchain::Swapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor) :
		Trident::Swapchain(device, descriptor),
		surface(static_cast<const DX12::Surface*>(descriptor.surface.get()))
	{}

	uint32 DX12::Swapchain::getImageCount() const {
		return imageCount;
	}

	void DX12::Swapchain::update(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (const ref<Trident::ImageView> imageView : imageViews) {
			imageView.~shared_ptr();
		}

		for (const ref<Trident::Image> image : images) {
			image.~shared_ptr();
		}

		swapChain->Release();

		recreate(descriptor);
	}

	void DX12::Swapchain::recreate(const SwapchainDescriptor& descriptor) {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		HRESULT result;

		extent = descriptor.extent;

		DXGI_SWAP_CHAIN_DESC1 swapchainDescriptor{};
		swapchainDescriptor.Width = descriptor.extent.width;
		swapchainDescriptor.Height = descriptor.extent.height;
		swapchainDescriptor.Format = getImageFormat(surface->getFormat());
		swapchainDescriptor.Stereo = false;
		swapchainDescriptor.SampleDesc.Count = 1;
		swapchainDescriptor.SampleDesc.Quality = 0;
		swapchainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDescriptor.BufferCount = imageCount;
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

		result = swapChain1->QueryInterface(&swapChain);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to query swapchain interface.");

		swapChain1->Release();

		images = sequence<ref<Trident::Image>>(imageCount);
		imageViews = sequence<ref<Trident::ImageView>>(imageCount);

		UINT imageIndex = 0;

		while (imageIndex < imageCount) {
			images[imageIndex] = createImage(device, this, imageIndex);

			imageIndex += 1;
		}

		createImageViews();
	}

	ref<Trident::Swapchain> DX12::createSwapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		ref<DX12::Surface> surface = std::static_pointer_cast<DX12::Surface>(descriptor.surface);
		ref<DX12::Swapchain> swapchain = createRef<DX12::Swapchain>(device, descriptor);
		HRESULT result;

		swapchain->recreate(descriptor);

		// TODO: Move to Surface.
		{
			PROFILE_SCOPE("MakeWindowAssociation");

			result = _device->factory->MakeWindowAssociation(surface->window, DXGI_MWA_NO_ALT_ENTER);
		}

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to make window association.");

		return swapchain;
	}

	void DX12::destroySwapchain(const Trident::Device* device, const Trident::Swapchain* swapchain) {
		const DX12::Swapchain* _swapchain = static_cast<const DX12::Swapchain*>(swapchain);

		for (const ref<Trident::ImageView> imageView : _swapchain->imageViews) {
			imageView.~shared_ptr();
		}

		for (const ref<Trident::Image> image : _swapchain->images) {
			image.~shared_ptr();
		}

		_swapchain->swapChain->Release();
	}
}