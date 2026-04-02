// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Subpass> VisibilityRenderer::createVisibilitySubpass() const {
		return createSubpass({
			.colorAttachmentReferences = {
				// Visibility
				{
					.index = 0,
					.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
				},
			},
			.depthStencilAttachmentReference = {
				.index = 1,
				.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
		});
	}

	ref<RenderPass> VisibilityRenderer::createVisibilityRenderPass() const {
		const ref<Subpass> visibilitySubpass = createVisibilitySubpass();

		return createRenderPass({
			.attachments = {
				// Visibility
				{
					.format = ImageFormat::R32_UINT,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::UNDEFINED,
					.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
				},

				// Depth stencil
				{
					.format = ImageFormat::D32_SFLOAT,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::UNDEFINED,
					.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				},
			},
			.subpasses = {visibilitySubpass},
			.subpassDependencies = {},
		});
	}
}