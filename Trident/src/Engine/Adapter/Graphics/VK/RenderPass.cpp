// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/VK/ImageLayout.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPass.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	static bool isColorAttachment(view<ref<Subpass>> subpasses, uint32 attachmentIndex) {
		for (const ref<Subpass>& subpass : subpasses) {
			for (const AttachmentReference& colorAttachmentReference : subpass->getColorAttachmentReferences()) {
				if (colorAttachmentReference.index == attachmentIndex) {
					return true;
				}
			}
		}

		return false;
	}

	// TODO(subpass-refactor): Vulkan can make optimizations if we explicitly tell it that we don't care about the depth data post-rendering.
	VK::RenderPass::RenderPass(const Trident::Device* device, const RenderPassDescriptor& descriptor) :
		Trident::RenderPass(device, descriptor)
	{}

	ref<Trident::RenderPass> VK::createRenderPass(const Trident::Device* device, ref<Trident::Surface> surface, const RenderPassDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const ref<VK::Surface> _surface = std::static_pointer_cast<VK::Surface>(surface);
		VkResult result;

		ref<VK::RenderPass> renderPass = createRef<VK::RenderPass>(device, descriptor);
		renderPass->surfaceExtent = _surface->capabilities.currentExtent;

		uint32 resolveAttachmentCount = 0;

		for (const ref<Subpass>& subpass : renderPass->subpasses) {
			if (subpass->getRasterizationSampleCount() == 1) {
				continue;
			}

			resolveAttachmentCount += subpass->getResolveAttachmentReferenceCount();
		}

		sequence<VkAttachmentDescription> attachmentDescriptions(renderPass->attachments.count() + resolveAttachmentCount);

		uint32 resolveAttachmentIndex = 0;

		for (uint32 attachmentIndex = 0; attachmentIndex < renderPass->attachments.count(); attachmentIndex += 1) {
			const Attachment& attachment = renderPass->attachments[attachmentIndex];

			VkAttachmentDescription& attachmentDescription = attachmentDescriptions[attachmentIndex];
			attachmentDescription.flags = 0;
			attachmentDescription.format = getFormat(attachment.format);
			attachmentDescription.samples = static_cast<VkSampleCountFlagBits>(attachment.sampleCount);
			attachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(attachment.loadOperation);
			attachmentDescription.storeOp = static_cast<VkAttachmentStoreOp>(attachment.storeOperation);
			attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			// TODO: Allow custom initial layouts?
			if (attachment.loadOperation == LoadOperation::LOAD) {
				if (isDepthFormat(attachmentDescription.format)) {
					attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				}
				else {
					attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}
			}
			else {
				attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}

			attachmentDescription.finalLayout = getImageLayout(attachment.finalLayout);

			if (isColorAttachment(renderPass->subpasses, attachmentIndex) && attachment.sampleCount > 1) {
				// Create resolve attachment description.
				VkAttachmentDescription& resolveAttachmentDescription = attachmentDescriptions[renderPass->attachments.count() + resolveAttachmentIndex];
				resolveAttachmentDescription.flags = 0;
				resolveAttachmentDescription.format = _surface->surfaceFormat.format;
				resolveAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
				resolveAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachmentDescription.storeOp = attachmentDescription.storeOp;
				resolveAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				resolveAttachmentDescription.initialLayout = attachmentDescription.initialLayout;
				resolveAttachmentDescription.finalLayout = attachmentDescription.finalLayout;

				// Set the store operation in the resolve attachment instead.
				attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				resolveAttachmentIndex += 1;
			}
		}

		sequence<VkAttachmentReference> attachmentReferences(renderPass->attachmentReferenceCount);
		sequence<VkSubpassDescription> subpassDescriptions(descriptor.subpasses.count());
		uint32 attachmentReferenceOffset = 0;

		for (uint32 subpassIndex = 0; subpassIndex < descriptor.subpasses.count(); subpassIndex += 1) {
			const ref<Subpass> subpass = descriptor.subpasses[subpassIndex];
			const uint32 depthStencilAttachmentCount = static_cast<uint32>(subpass->hasDepthStencilAttachmentReference());

			uint32 relativeAttachmentReferenceOffset = attachmentReferenceOffset;

			for (uint32 colorAttachmentReferenceIndex = 0; colorAttachmentReferenceIndex < subpass->getColorAttachmentReferenceCount(); colorAttachmentReferenceIndex += 1) {
				const AttachmentReference& colorAttachmentReference = subpass->getColorAttachmentReferences()[colorAttachmentReferenceIndex];

				VkAttachmentReference& attachmentReference = attachmentReferences[relativeAttachmentReferenceOffset + colorAttachmentReferenceIndex];
				attachmentReference.attachment = colorAttachmentReference.index;
				attachmentReference.layout = getImageLayout(colorAttachmentReference.layout);
			}

			relativeAttachmentReferenceOffset += subpass->getColorAttachmentReferenceCount();

			if (subpass->hasDepthStencilAttachmentReference()) {
				const AttachmentReference& depthStencilAttachmentReference = subpass->getDepthStencilAttachmentReference();

				VkAttachmentReference& attachmentReference = attachmentReferences[relativeAttachmentReferenceOffset];
				attachmentReference.attachment = depthStencilAttachmentReference.index;
				attachmentReference.layout = getImageLayout(depthStencilAttachmentReference.layout);

				relativeAttachmentReferenceOffset += 1;
			}

			if (subpass->getRasterizationSampleCount() > 1) {
				// Create resolve attachment references.
				for (uint32 resolveAttachmentReferenceIndex = 0; resolveAttachmentReferenceIndex < subpass->getColorAttachmentReferenceCount(); resolveAttachmentReferenceIndex += 1) {
					VkAttachmentReference& attachmentReference = attachmentReferences[relativeAttachmentReferenceOffset + resolveAttachmentReferenceIndex];
					attachmentReference.attachment = static_cast<uint32>(renderPass->attachments.count()) + resolveAttachmentReferenceIndex;
					attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}

				relativeAttachmentReferenceOffset += subpass->getColorAttachmentReferenceCount();
			}

			for (uint32 inputAttachmentReferenceIndex = 0; inputAttachmentReferenceIndex < subpass->getInputAttachmentReferenceCount(); inputAttachmentReferenceIndex += 1) {
				const AttachmentReference& inputAttachmentReference = subpass->getInputAttachmentReferences()[inputAttachmentReferenceIndex];

				VkAttachmentReference& attachmentReference = attachmentReferences[relativeAttachmentReferenceOffset + inputAttachmentReferenceIndex];
				attachmentReference.attachment = inputAttachmentReference.index;
				attachmentReference.layout = getImageLayout(inputAttachmentReference.layout);
			}

			relativeAttachmentReferenceOffset += subpass->getInputAttachmentReferenceCount();

			VkSubpassDescription& subpassDescription = subpassDescriptions[subpassIndex];
			subpassDescription.flags = 0;
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.inputAttachmentCount = static_cast<uint32>(subpass->getInputAttachmentReferenceCount());

			if (subpass->getInputAttachmentReferenceCount() > 0) {
				subpassDescription.pInputAttachments = &attachmentReferences[attachmentReferenceOffset + (subpass->getColorAttachmentReferenceCount() * (subpass->getRasterizationSampleCount() > 1 ? 2 : 1)) + depthStencilAttachmentCount];
			}
			else {
				subpassDescription.pInputAttachments = nullptr;
			}

			subpassDescription.colorAttachmentCount = static_cast<uint32>(subpass->getColorAttachmentReferenceCount());
			subpassDescription.pColorAttachments = &attachmentReferences[attachmentReferenceOffset];

			if (subpass->getRasterizationSampleCount() > 1) {
				subpassDescription.pResolveAttachments = &attachmentReferences[attachmentReferenceOffset + subpass->getColorAttachmentReferenceCount() + depthStencilAttachmentCount];
			}
			else {
				subpassDescription.pResolveAttachments = nullptr;
			}

			if (subpass->hasDepthStencilAttachmentReference()) {
				subpassDescription.pDepthStencilAttachment = &attachmentReferences[attachmentReferenceOffset + subpass->getColorAttachmentReferenceCount()];
			}
			else {
				subpassDescription.pDepthStencilAttachment = nullptr;
			}

			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = nullptr;

			attachmentReferenceOffset = relativeAttachmentReferenceOffset;
		}

		sequence<VkSubpassDependency> subpassDependencies(descriptor.subpassDependencies.count());

		// TODO(subpass-refactor)
		for (uint32 subpassDependencyIndex = 0; subpassDependencyIndex < descriptor.subpassDependencies.count(); subpassDependencyIndex += 1) {
			const SubpassDependency& dependency = descriptor.subpassDependencies[subpassDependencyIndex];

			VkSubpassDependency& subpassDependency = subpassDependencies[subpassDependencyIndex];
			subpassDependency.srcSubpass = dependency.sourceSubpassIndex;
			subpassDependency.dstSubpass = dependency.destinationSubpassIndex;
			subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.srcAccessMask = 0;
			subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dependencyFlags = 0;

			/* if (dependency.destinationSubpassIndex != SubpassDependency::EXTERNAL_SUBPASS) {
				const ref<Subpass> destinationSubpass = descriptor.subpasses[dependency.destinationSubpassIndex];

				if (destinationSubpass->hasDepthStencilAttachmentReference()) {
					subpassDependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
					subpassDependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
					subpassDependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				}
			} */
		}

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = static_cast<uint32>(attachmentDescriptions.count());
		renderPassCreateInfo.pAttachments = &attachmentDescriptions[0];
		renderPassCreateInfo.subpassCount = static_cast<uint32>(subpassDescriptions.count());
		renderPassCreateInfo.pSubpasses = &subpassDescriptions[0];
		renderPassCreateInfo.dependencyCount = static_cast<uint32>(subpassDependencies.count());
		renderPassCreateInfo.pDependencies = &subpassDependencies[0];

		result = _device->vkCreateRenderPass(_device->device, &renderPassCreateInfo, nullptr, &renderPass->renderPass);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create render pass ([]).", string_VkResult(result));

		return renderPass;
	}

	void VK::destroyRenderPass(const Trident::Device* device, const Trident::RenderPass* renderPass) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::RenderPass* _renderPass = static_cast<const VK::RenderPass*>(renderPass);

		_device->vkDestroyRenderPass(_device->device, _renderPass->renderPass, nullptr);
	}
}