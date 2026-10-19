// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<RenderPass> VisibilityRenderer::createPresentRenderPass() const {
		return createRenderPass({
			.attachments = {
				// Swapchain
				{
					.format = getSurface()->getFormat(),
					.sampleCount = ImageSampleCount::x1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::PRESENT_SRC,
				},
			},
			.subpasses = {
				// Present
				{
					.colorAttachmentReferences = {
						{
							.index = 0,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
				},

				// Debug present
				/* {
					.colorAttachmentReferences = {
						{
							.index = 0,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
					.inputAttachmentReferences = {
						{
							.index = 1,
							.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						},
						{
							.index = 2,
							.layout = ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL,
						},
					},
				}, */
			},
			.subpassDependencies = {},
		});
	}
}