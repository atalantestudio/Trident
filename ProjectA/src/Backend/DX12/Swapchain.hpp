// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Backend/DX12/Device.hpp"
#include "Backend/DX12/Instance.hpp"
#include "Backend/DX12/Surface.hpp"

#include "Engine/API/Rendering/Swapchain.hpp"

namespace ProjectA {
	namespace DX12 {
		class Swapchain;
	}

	class DX12::Swapchain final : public ProjectA::Swapchain {
		public:
			explicit Swapchain(const SwapchainDescriptor& descriptor, const DX12::Instance& instance, const DX12::Surface& surface, const DX12::Device& device);

			~Swapchain() final override;

			uint32 getImageCount() const final override;

			void update(const SwapchainDescriptor& descriptor) final override;

		private:
			void recreate(const SwapchainDescriptor& descriptor);

		public:
			ComPtr<IDXGISwapChain3> swapChain;
			ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;

			uint32 imageCount = 3;

		private:
			const DX12::Instance& instance;
			const DX12::Surface& surface;
			const DX12::Device& device;

			uint32 rtvDescriptorSize;
	};
}