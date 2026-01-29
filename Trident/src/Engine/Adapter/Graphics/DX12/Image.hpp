// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Rendering/Image.hpp"

namespace Trident {
	class DX12::Image final : public Trident::Image {
		private:
			static ImageDescriptor createSwapchainImageDescriptor(Extent2D extent);

		public:
			explicit Image(const ImageDescriptor& descriptor, const DX12::Device& device, ref<DX12::MemoryAllocator> memoryAllocator);

			// NOTE: Render Target View constructor
			explicit Image(const DX12::Device& device, const DX12::Swapchain& swapchain, UINT imageIndex, ref<DX12::MemoryAllocator> memoryAllocator);

		protected:
			ref<Trident::ImageView> createViewInternal(const ImageViewDescriptor& descriptor, Trident::Image* image) final override;

		public:
			ComPtr<ID3D12Resource> resource;
			D3D12_RESOURCE_STATES resourceState;
			ImageUsage usage;

		private:
			const DX12::Device& device;
			ref<DX12::MemoryAllocator> memoryAllocator;
	};
}