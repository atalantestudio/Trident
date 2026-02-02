// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Subpass> VisibilityRenderer::createVisibilitySubpass() const {
		AttachmentReference writeVisibilityAttachmentReference{};
		writeVisibilityAttachmentReference.index = VISIBILITY_ATTACHMENT_INDEX;
		writeVisibilityAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		AttachmentReference writeMaterialIndexAttachmentReference{};
		writeMaterialIndexAttachmentReference.index = MATERIAL_INDEX_ATTACHMENT_INDEX;
		writeMaterialIndexAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		AttachmentReference writeDepthStencilAttachmentReference{};
		writeDepthStencilAttachmentReference.index = DEPTH_STENCIL_ATTACHMENT_INDEX;
		writeDepthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		SubpassDescriptor visibilitySubpassDescriptor{};
		visibilitySubpassDescriptor.colorAttachmentReferences = {
			writeVisibilityAttachmentReference,
			writeMaterialIndexAttachmentReference,
		};
		visibilitySubpassDescriptor.depthStencilAttachmentReference = writeDepthStencilAttachmentReference;

		return createSubpass(visibilitySubpassDescriptor);
	}

	ref<Subpass> VisibilityRenderer::createDebugSubpass() const {
		AttachmentReference writeSwapchainAttachmentReference{};
		writeSwapchainAttachmentReference.index = SWAPCHAIN_ATTACHMENT_INDEX;
		writeSwapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		AttachmentReference readVisibilityAttachmentReference{};
		readVisibilityAttachmentReference.index = VISIBILITY_ATTACHMENT_INDEX;
		readVisibilityAttachmentReference.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

		AttachmentReference readMaterialIndexAttachmentReference{};
		readMaterialIndexAttachmentReference.index = MATERIAL_INDEX_ATTACHMENT_INDEX;
		readMaterialIndexAttachmentReference.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

		AttachmentReference readDepthStencilAttachmentReference{};
		readDepthStencilAttachmentReference.index = DEPTH_STENCIL_ATTACHMENT_INDEX;
		readDepthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		SubpassDescriptor debugSubpassDescriptor{};
		debugSubpassDescriptor.colorAttachmentReferences = {writeSwapchainAttachmentReference};
		debugSubpassDescriptor.inputAttachmentReferences = {
			readVisibilityAttachmentReference,
			readMaterialIndexAttachmentReference,
			readDepthStencilAttachmentReference,
		};

		return createSubpass(debugSubpassDescriptor);
	}

	ref<RenderPass> VisibilityRenderer::_createRenderPass() const {
		const ref<Subpass> visibilitySubpass = createVisibilitySubpass();
		const ref<Subpass> debugSubpass = createDebugSubpass();

		Attachment swapchainAttachment{};
		swapchainAttachment.format = getSurface()->getFormat();
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

		Attachment materialIndexAttachment{};
		materialIndexAttachment.format = ImageFormat::R32_UINT;
		materialIndexAttachment.sampleCount = 1;
		materialIndexAttachment.loadOperation = LoadOperation::CLEAR;
		materialIndexAttachment.storeOperation = StoreOperation::UNDEFINED;
		materialIndexAttachment.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

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
		renderPassDescriptor.attachments = {
			swapchainAttachment,
			visibilityAttachment,
			materialIndexAttachment,
			depthStencilAttachment,
		};
		renderPassDescriptor.subpasses = {
			visibilitySubpass,
			debugSubpass,
		};
		renderPassDescriptor.subpassDependencies = {subpassDependency};

		return createRenderPass(renderPassDescriptor);
	}
}