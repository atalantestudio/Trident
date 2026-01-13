// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	class DX12::Image final : public ProjectA::Image {
		public:
			explicit Image(const ImageDescriptor& descriptor);

			// NOTE: Render Target View constructor
			explicit Image(const DX12::Device& device, const DX12::Swapchain& swapchain, uint32 imageIndex, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle);

		protected:
			ref<ProjectA::ImageView> createViewInternal(const ImageViewDescriptor& descriptor, const ProjectA::Image* image) const final override;

		public:
			ComPtr<ID3D12Resource> resource;
	};
}