// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/Surface.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace Trident {
	class DX12::Swapchain final : public Trident::Swapchain {
		public:
			explicit Swapchain(const SwapchainDescriptor& descriptor, const DX12::Device* device);

			~Swapchain() final override;

			uint32 getImageCount() const final override;

			void update(const SwapchainDescriptor& descriptor) final override;

		private:
			void recreate(const SwapchainDescriptor& descriptor);

		public:
			IDXGISwapChain3* swapChain;
			UINT imageCount = 3;

		private:
			const DX12::Surface* surface;
			const DX12::Device* device;
	};
}