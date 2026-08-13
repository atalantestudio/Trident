// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Descriptor/SwapchainDescriptor.hpp"
#include "Engine/API/Graphics/Image.hpp"
#include "Engine/API/Graphics/ImageView.hpp"
#include "Engine/API/Graphics/RenderPass.hpp"

namespace Trident {
	class Swapchain {
		public:
			~Swapchain();

			Extent2D getExtent() const;

			uint32 getImageCount() const;

			ref<ImageView> getImageView(uint32 index) const;

			// TODO: Don't expose to the client side.
			void createImageViews();

			void update(const SwapchainDescriptor& descriptor);

		protected:
			explicit Swapchain(const Device* device, const SwapchainDescriptor& descriptor);

		// TODO: Don't expose to the client side.
		public:
			const Device* device;
			uint32 imageCount;
			Extent2D extent;
			sequence<private_ref<Image>> images;
			sequence<private_ref<ImageView>> imageViews;
	};
}