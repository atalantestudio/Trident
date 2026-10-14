// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/RenderPass.hpp"
#include "Engine/API/Graphics/ClearValue.hpp"
#include "Engine/API/Window/LayerGroup/LayerGroup.hpp"

namespace Trident {
	class RenderPassLayerGroup : public LayerGroup {
		public:
			explicit RenderPassLayerGroup(range<uint32, uint32> layerRange);

			virtual RenderPassDescriptor getRenderPassDescriptor() const = 0;

			virtual sequence<ClearValue> getClearValues() const = 0;

			void initialize() override;

			void createFramebufferImages();

			void createFramebuffers();

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const override;

			void resize() override;

		private:
			uint32 getImageIndex(uint32 attachmentIndex) const;

			void createFramebufferImageView(
				uint32 imageIndex,
				uint32 attachmentIndex
				#if defined(BUILD_MODE_DEBUG)
					, view<char8> name
				#endif
			);

		private:
			ref<RenderPass> renderPass;
			sequence<ref<Image>> framebufferImages;
			sequence<ref<ImageView>> framebufferImageViews;
			sequence<ref<Framebuffer>> framebuffers;
			sequence<ClearValue> clearValues;

			uint32 swapchainAttachmentIndex = max<uint32>();
			uint32 swapchainImageIndex;
	};
}