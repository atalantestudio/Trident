// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/StaticSceneRayTracer/StaticSceneRayTracer.hpp"

namespace Trident {
	ref<RenderPass> StaticSceneRayTracer::createPresentRenderPass() const {
		return createRenderPass({
			.attachments = {
				// Swapchain
				{
					.format = getSurface()->getFormat(),
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::PRESENT_SRC,
				},
			},
			.subpasses = {
				createSubpass({
					.colorAttachmentReferences = {
						// Write swapchain
						{
							.index = 0,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
				}),
			},
		});
	}
}