// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Image.hpp"

namespace Trident {
	class DX12::Image final : public Trident::Image {
		public:
			explicit Image(const Trident::Device* device, const ImageDescriptor& descriptor);

		public:
			ID3D12Resource* resource;
			D3D12_BARRIER_ACCESS barrierAccess = D3D12_BARRIER_ACCESS_NO_ACCESS;
			D3D12_BARRIER_LAYOUT barrierLayout = D3D12_BARRIER_LAYOUT_UNDEFINED;
			ImageUsage usage;
	};

	namespace DX12 {
		ref<Trident::Image> createImage(const Trident::Device* device, const ImageDescriptor& descriptor);
		ref<Trident::Image> createImage(const Trident::Device* device, const DX12::Swapchain* swapchain, UINT index);

		void destroyImage(const Trident::Device* device, const Trident::Image* image);
	}
}