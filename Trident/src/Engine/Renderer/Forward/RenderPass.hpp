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
					.sampleCount = SAMPLE_COUNT,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::UNDEFINED,
					.finalLayout = ImageLayout::PRESENT_SRC,
				},

				// Depth stencil
				{
					.format = ImageFormat::D32_SFLOAT,
					.sampleCount = SAMPLE_COUNT,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::UNDEFINED,
					.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				},
			},
			.subpasses = {
				createSubpass({
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
				}),
			},
		});
	}
}