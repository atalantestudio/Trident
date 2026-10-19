// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Experimental/GI2D/Renderer.hpp"

namespace Trident {
	ref<RenderPass> GI2DExperimentalRenderer::createRenderPass() const {
		return Renderer::createRenderPass({
			.attachments = {
				{
					.format = getSurface()->getFormat(),
					.sampleCount = ImageSampleCount::x1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::PRESENT_SRC,
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
				},
			},
			.subpassDependencies = {},
		});
	}
}