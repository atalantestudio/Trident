// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	SubpassDescriptor getVisibilitySubpassDescriptor() {
		AttachmentReference writeVisibilityAttachmentReference{};
		writeVisibilityAttachmentReference.index = 1;
		writeVisibilityAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		AttachmentReference writeDepthStencilAttachmentReference{};
		writeDepthStencilAttachmentReference.index = 2;
		writeDepthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		SubpassDescriptor visibilitySubpassDescriptor{};
		visibilitySubpassDescriptor.colorAttachmentReferences = {writeVisibilityAttachmentReference};
		visibilitySubpassDescriptor.depthStencilAttachmentReference = writeDepthStencilAttachmentReference;

		return visibilitySubpassDescriptor;
	}

	SubpassDescriptor getDebugSubpassDescriptor() {
		AttachmentReference writeSwapchainAttachmentReference{};
		writeSwapchainAttachmentReference.index = 0;
		writeSwapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		AttachmentReference readVisibilityAttachmentReference{};
		readVisibilityAttachmentReference.index = 1;
		readVisibilityAttachmentReference.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

		AttachmentReference readDepthStencilAttachmentReference{};
		readDepthStencilAttachmentReference.index = 2;
		readDepthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		SubpassDescriptor debugSubpassDescriptor{};
		debugSubpassDescriptor.colorAttachmentReferences = {writeSwapchainAttachmentReference};
		debugSubpassDescriptor.inputAttachmentReferences = {readVisibilityAttachmentReference, readDepthStencilAttachmentReference};

		return debugSubpassDescriptor;
	}

	RenderPassDescriptor getRenderPassDescriptor(ref<Surface> surface, ref<Subpass> visibilitySubpass, ref<Subpass> debugSubpass) {
		Attachment swapchainAttachment{};
		swapchainAttachment.format = surface->getFormat();
		swapchainAttachment.sampleCount = 1;
		swapchainAttachment.loadOperation = LoadOperation::CLEAR;
		swapchainAttachment.storeOperation = StoreOperation::STORE;
		swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

		Attachment visibilityAttachment{};
		visibilityAttachment.format = ImageFormat::R32_UINT;
		visibilityAttachment.sampleCount = 1;
		visibilityAttachment.loadOperation = LoadOperation::CLEAR;
		visibilityAttachment.storeOperation = StoreOperation::UNDEFINED;
		visibilityAttachment.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		Attachment depthStencilAttachment{};
		depthStencilAttachment.format = ImageFormat::D32_SFLOAT;
		depthStencilAttachment.sampleCount = 1;
		depthStencilAttachment.loadOperation = LoadOperation::CLEAR;
		depthStencilAttachment.storeOperation = StoreOperation::UNDEFINED;
		depthStencilAttachment.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		SubpassDependency subpassDependency{};
		subpassDependency.sourceSubpassIndex = SubpassDependency::EXTERNAL_SUBPASS;
		subpassDependency.destinationSubpassIndex = 0;

		RenderPassDescriptor renderPassDescriptor{};
		renderPassDescriptor.attachments = {swapchainAttachment, visibilityAttachment, depthStencilAttachment};
		renderPassDescriptor.subpasses = {visibilitySubpass, debugSubpass};
		renderPassDescriptor.subpassDependencies = {subpassDependency};

		return renderPassDescriptor;
	}
}