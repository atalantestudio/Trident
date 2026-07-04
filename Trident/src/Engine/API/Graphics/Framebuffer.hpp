// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Descriptor/FramebufferDescriptor.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/ImageView.hpp"
#include "Engine/API/Graphics/RenderPass.hpp"

namespace Trident {
	class Framebuffer {
		public:
			explicit Framebuffer(const Device* device, const FramebufferDescriptor& descriptor);

			/// Destroys the framebuffer.
			~Framebuffer();

			/// Returns the framebuffer render pass.
			ref<RenderPass> getRenderPass() const;

			/// Returns the framebuffer attachments.
			const sequence<ref<ImageView>>& getImageViews() const;

			/// Returns the framebuffer extent.
			Extent2D getExtent() const;

		protected:
			const Device* device;
			ref<RenderPass> renderPass;
			Extent2D extent;

		// TODO: Make private.
		public:
			sequence<ref<ImageView>> imageViews;
	};
}