// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<RenderPass> ForwardRenderer::createDirectionalShadowMapRenderPass() const {
		return Renderer::createRenderPass({
			.attachments = {
				{
					.format = ImageFormat::D32_SFLOAT,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,

					// TODO(renderer/forward/directional-shadow-map): Use UNDEFINED instead?
					.storeOperation = StoreOperation::STORE,

					.finalLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				},
			},
			.subpasses = {
				{
					.depthStencilAttachmentReference = {
						.index = 0,
						.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					},
				},
			},
		});
	}
}