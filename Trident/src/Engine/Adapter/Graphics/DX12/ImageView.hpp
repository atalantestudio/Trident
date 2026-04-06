// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/API/Graphics/ImageView.hpp"

namespace Trident {
	class DX12::ImageView final : public Trident::ImageView {
		public:
			explicit ImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor);

		public:
			D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle;
	};

	namespace DX12 {
		ref<Trident::ImageView> createImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor);

		void destroyImageView(const Trident::Device* device, const Trident::ImageView* imageView);
	}
}