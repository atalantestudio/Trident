// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Subpass> VisibilityRenderer::createPresentSubpass() const {
		/* AttachmentReference readOutputAttachmentReference{};
		readOutputAttachmentReference.index = 1;
		readOutputAttachmentReference.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL; */

		AttachmentReference writeSwapchainAttachmentReference{};
		writeSwapchainAttachmentReference.index = 0;
		writeSwapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		SubpassDescriptor presentSubpassDescriptor{};
		presentSubpassDescriptor.colorAttachmentReferences = {writeSwapchainAttachmentReference};
		presentSubpassDescriptor.inputAttachmentReferences = {/* readOutputAttachmentReference */};

		return createSubpass(presentSubpassDescriptor);
	}

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

		// TODO: Restore debug present subpass.
		// const ref<Subpass> debugPresentSubpass = createDebugPresentSubpass();

		Attachment swapchainAttachment{};
		swapchainAttachment.format = getSurface()->getFormat();
		swapchainAttachment.sampleCount = 1;
		swapchainAttachment.loadOperation = LoadOperation::CLEAR;
		swapchainAttachment.storeOperation = StoreOperation::STORE;
		swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

		/* Attachment outputAttachment{};
		outputAttachment.format = getSurface()->getFormat();
		outputAttachment.sampleCount = 1;
		outputAttachment.loadOperation = LoadOperation::CLEAR;
		outputAttachment.storeOperation = StoreOperation::UNDEFINED;
		outputAttachment.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL; */

		RenderPassDescriptor renderPassDescriptor{};
		renderPassDescriptor.attachments = {swapchainAttachment/*, outputAttachment */};
		renderPassDescriptor.subpasses = {presentSubpass/*, debugPresentSubpass */};
		renderPassDescriptor.subpassDependencies = {};

		return createRenderPass(renderPassDescriptor);
	}
}