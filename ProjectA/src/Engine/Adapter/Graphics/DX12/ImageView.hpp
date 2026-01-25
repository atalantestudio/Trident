// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class DX12::ImageView final : public ProjectA::ImageView {
		public:
			explicit ImageView(const ImageViewDescriptor& descriptor, const ProjectA::Image* image, ref<DX12::MemoryAllocator> memoryAllocator);

		private:
			void createSwapchainRenderTargetView();
			void createRenderTargetView();
			void createDepthStencilView();

		public:
			D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;

		private:
			const DX12::Image* image;
			ref<DX12::MemoryAllocator> memoryAllocator;
	};
}