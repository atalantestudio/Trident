// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"
#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	class VK::Image final : public ProjectA::Image {
		public:
			explicit Image(const ImageDescriptor& descriptor, const VK::Device& device);

			// Render target constructor (wrapper around a swapchain image).
			explicit Image(const VK::Device& device, VkImage image, VkFormat format, Extent2D extent);

			~Image() final override;

		protected:
			ref<ProjectA::ImageView> createViewInternal(const ImageViewDescriptor& descriptor, const ProjectA::Image* image) const final override;

		private:
			static ImageDescriptor createSwapchainImageDescriptor(Extent2D extent);

		public:
			VkImage image;
			VkImageType type;
			VkFormat format;

		private:
			const VK::Device& device;
			MemorySuballocation memorySuballocation;
			bool swapchainImage;
	};
}