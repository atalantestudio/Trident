// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	ref<RenderPass> MapRenderer::createRenderPass() const {
		return Renderer::createRenderPass({
			.attachments = {
				// Swapchain
				{
					.format = getSurface()->getFormat(),
					.sampleCount = SAMPLE_COUNT,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
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