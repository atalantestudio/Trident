// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Subpass> VisibilityRenderer::createPresentSubpass() const {
		return createSubpass({
			.colorAttachmentReferences = {
				// Swapchain
				{
					.index = 0,
					.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
				},
			},
		});
	}

	// TODO(visibility/debug): Restore.
	/* ref<Subpass> VisibilityRenderer::createDebugPresentSubpass() const {
		AttachmentReference writeSwapchainAttachmentReference{};
		writeSwapchainAttachmentReference.index = 0;
		writeSwapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		AttachmentReference readVisibilityAttachmentReference{};
		readVisibilityAttachmentReference.index = 1;
		readVisibilityAttachmentReference.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

		AttachmentReference readDepthStencilAttachmentReference{};
		readDepthStencilAttachmentReference.index = 2;
		readDepthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		SubpassDescriptor debugPresentSubpassDescriptor{};
		debugPresentSubpassDescriptor.colorAttachmentReferences = {writeSwapchainAttachmentReference};
		debugPresentSubpassDescriptor.inputAttachmentReferences = {readVisibilityAttachmentReference, readDepthStencilAttachmentReference};

		return createSubpass(debugPresentSubpassDescriptor);
	} */

	ref<RenderPass> VisibilityRenderer::createPresentRenderPass() const {
		const ref<Subpass> presentSubpass = createPresentSubpass();

		// TODO(visibility/debug): Restore.
		// const ref<Subpass> debugPresentSubpass = createDebugPresentSubpass();

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
				presentSubpass,
				// debugPresentSubpass,
			},
			.subpassDependencies = {},
		});
	}
}