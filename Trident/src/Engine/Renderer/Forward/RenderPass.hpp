// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<RenderPass> ForwardRenderer::createRenderPass() const {
		return Renderer::createRenderPass({
			.attachments = {
				// Color
				{
					.format = getSurface()->getFormat(),
					.sampleCount = FORWARD_SAMPLE_COUNT,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::PRESENT_SRC,
				},

				// Depth stencil
				{
					.format = ImageFormat::D32_SFLOAT,
					.sampleCount = FORWARD_SAMPLE_COUNT,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::UNDEFINED,
					.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				},
			},
			.subpasses = {
				{
					.colorAttachmentReferences = {
						{
							.index = 0,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
					.depthStencilAttachmentReference = {
						.index = 1,
						.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					},
				},
			},
			.subpassDependencies = {},
		});
	}
}