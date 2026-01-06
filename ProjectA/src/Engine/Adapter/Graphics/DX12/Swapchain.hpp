// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/Adapter/Graphics/DX12/Surface.hpp"

#include "Engine/API/Rendering/Swapchain.hpp"

namespace ProjectA {
	class DX12::Swapchain final : public ProjectA::Swapchain {
		public:
			explicit Swapchain(const SwapchainDescriptor& descriptor, const DX12::Instance& instance, const DX12::Surface& surface, const DX12::Device& device);

			uint32 getImageCount() const final override;

			void update(const SwapchainDescriptor& descriptor) final override;

			// TODO (debug-only)
			ref<DX12::Image> getImage(uint32 imageIndex) const;

		private:
			void recreate(const SwapchainDescriptor& descriptor);

		public:
			ComPtr<IDXGISwapChain3> swapChain;
			ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
			uint32 imageCount = 3;
			uint32 rtvDescriptorSize;

		private:
			const DX12::Instance& instance;
			const DX12::Surface& surface;
			const DX12::Device& device;
	};
}