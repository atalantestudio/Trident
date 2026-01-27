// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Graphics/GL/Surface.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace Trident {
	class GL::Swapchain final : public Trident::Swapchain {
		public:
			explicit Swapchain(const SwapchainDescriptor& descriptor, const GL::Instance& instance, const GL::Surface& surface);

			uint32 getImageCount() const final override;

			void update(const SwapchainDescriptor& descriptor) final override;

		private:
			const GL::Instance& instance;
			const GL::Surface& surface;
	};
}