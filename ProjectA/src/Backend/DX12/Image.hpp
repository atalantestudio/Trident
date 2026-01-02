// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Backend/DX12/Device.hpp"

#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	namespace DX12 {
		class Image;
	}

	class DX12::Image final : public ProjectA::Image {
		public:
			// Render Target View constructor.
			explicit Image(const DX12::Device& device, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle);

			~Image() final override;

			ref<ProjectA::ImageView> createViewInternal(const ProjectA::Image* image, const ImageViewDescriptor& descriptor) const final override;

		public:
			ComPtr<ID3D12Resource> resource;
	};
}