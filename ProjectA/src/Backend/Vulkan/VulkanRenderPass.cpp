#include "pch.hpp"

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"
#include "Backend/Vulkan/ImageLayout.hpp"

namespace ProjectA {
	// TODO(subpass-refactor): Vulkan can make optimizations if we explicitly tell it that we don't care about the depth data post-rendering.
	VulkanRenderPass::VulkanRenderPass(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor),
		physicalDevice(physicalDevice),
		device(device),
		surface(&surface)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VkSurfaceFormatKHR surfaceFormat = surface.selectFormat();

		uint32 resolveAttachmentCount = 0;

		for (const ref<Subpass>& subpass : subpasses) {
			if (subpass->getRasterizationSampleCount() == 1) {
				continue;
			}

			resolveAttachmentCount += subpass->getResolveAttachmentReferenceCount();
		}

		sequence<VkAttachmentDescription> attachmentDescriptions(attachments.count + resolveAttachmentCount);

		uint32 resolveAttachmentIndex = 0;

		for (uint32 attachmentIndex = 0; attachmentIndex < attachments.count; attachmentIndex += 1) {
			const Attachment& attachment = attachments[attachmentIndex];

			VkAttachmentDescription& attachmentDescription = attachmentDescriptions[attachmentIndex];
			attachmentDescription.flags = 0;
			attachmentDescription.format = getVkFormat(attachment.format);
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

			attachmentDescription.finalLayout = getVkImageLayout(attachment.finalLayout);

			if (isColorAttachment(attachmentIndex) && attachment.sampleCount > 1) {
				// Create resolve attachment description.
				VkAttachmentDescription& resolveAttachmentDescription = attachmentDescriptions[attachments.count + resolveAttachmentIndex];
				resolveAttachmentDescription.flags = 0;
				resolveAttachmentDescription.format = surfaceFormat.format;
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

		sequence<VkAttachmentReference> attachmentReferences(attachmentReferenceCount);
		sequence<VkSubpassDescription> subpassDescriptions(descriptor.subpasses.count);
		uint32 attachmentReferenceOffset = 0;

		for (uint32 subpassIndex = 0; subpassIndex < descriptor.subpasses.count; subpassIndex += 1) {
			const ref<Subpass>& subpass = descriptor.subpasses[subpassIndex];
			const uint32 depthStencilAttachmentCount = static_cast<uint32>(subpass->hasDepthStencilAttachmentReference());

			uint32 relativeAttachmentReferenceOffset = attachmentReferenceOffset;

			for (uint32 colorAttachmentReferenceIndex = 0; colorAttachmentReferenceIndex < subpass->getColorAttachmentReferenceCount(); colorAttachmentReferenceIndex += 1) {
				const AttachmentReference& colorAttachmentReference = subpass->getColorAttachmentReferences()[colorAttachmentReferenceIndex];

				VkAttachmentReference& attachmentReference = attachmentReferences[relativeAttachmentReferenceOffset + colorAttachmentReferenceIndex];
				attachmentReference.attachment = colorAttachmentReference.index;
				attachmentReference.layout = getVkImageLayout(colorAttachmentReference.layout);
			}

			relativeAttachmentReferenceOffset += subpass->getColorAttachmentReferenceCount();

			if (subpass->hasDepthStencilAttachmentReference()) {
				const AttachmentReference& depthStencilAttachmentReference = subpass->getDepthStencilAttachmentReference();

				VkAttachmentReference& attachmentReference = attachmentReferences[relativeAttachmentReferenceOffset];
				attachmentReference.attachment = depthStencilAttachmentReference.index;
				attachmentReference.layout = getVkImageLayout(depthStencilAttachmentReference.layout);

				relativeAttachmentReferenceOffset += 1;
			}

			if (subpass->getRasterizationSampleCount() > 1) {
				// Create resolve attachment references.
				for (uint32 resolveAttachmentReferenceIndex = 0; resolveAttachmentReferenceIndex < subpass->getColorAttachmentReferenceCount(); resolveAttachmentReferenceIndex += 1) {
					VkAttachmentReference& attachmentReference = attachmentReferences[relativeAttachmentReferenceOffset + resolveAttachmentReferenceIndex];
					attachmentReference.attachment = static_cast<uint32>(attachments.count) + resolveAttachmentReferenceIndex;
					attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}

				relativeAttachmentReferenceOffset += subpass->getColorAttachmentReferenceCount();
			}

			for (uint32 inputAttachmentReferenceIndex = 0; inputAttachmentReferenceIndex < subpass->getInputAttachmentReferenceCount(); inputAttachmentReferenceIndex += 1) {
				const AttachmentReference& inputAttachmentReference = subpass->getInputAttachmentReferences()[inputAttachmentReferenceIndex];

				VkAttachmentReference& attachmentReference = attachmentReferences[relativeAttachmentReferenceOffset + inputAttachmentReferenceIndex];
				attachmentReference.attachment = inputAttachmentReference.index;
				attachmentReference.layout = getVkImageLayout(inputAttachmentReference.layout);
			}

			relativeAttachmentReferenceOffset += subpass->getInputAttachmentReferenceCount();

			/* for (uint32 colorAttachmentIndex = 0; colorAttachmentReferenceIndex < subpass->getColorAttachmentCount(); colorAttachmentIndex += 1) {
				const ColorAttachment& colorAttachment = subpass->getColorAttachments()[colorAttachmentIndex];

				VkAttachmentDescription& colorAttachmentDescription = attachmentDescriptions[attachmentReferenceOffset + colorAttachmentIndex];
				colorAttachmentDescription.flags = 0;
				colorAttachmentDescription.format = getVkFormat(colorAttachment.format);
				colorAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(colorAttachment.sampleCount);
				colorAttachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(colorAttachment.loadOperation);

				if (subpass->getResolveAttachmentCount() > 0) {
					// Set the store operation in the resolve attachment instead.
					colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				}
				else {
					colorAttachmentDescription.storeOp = static_cast<VkAttachmentStoreOp>(colorAttachment.storeOperation);
				}

				colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				if (colorAttachment.loadOperation == LoadOperation::LOAD) {
					colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}
				else {
					colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				}

				colorAttachmentDescription.finalLayout = getVkImageLayout(colorAttachment.finalLayout);

				VkAttachmentReference& colorAttachmentReference = attachmentReferences[attachmentReferenceOffset + colorAttachmentIndex];
				colorAttachmentReference.attachment = colorAttachmentIndex;

				// TODO: Use the same layout as the one from the attachment description?
				colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			for (uint32 resolveAttachmentIndex = 0; resolveAttachmentIndex < subpass->getResolveAttachmentCount(); resolveAttachmentIndex += 1) {
				const VkAttachmentDescription& colorAttachmentDescription = attachmentDescriptions[attachmentReferenceOffset - subpass->getColorAttachmentCount() - depthStencilAttachmentCount + resolveAttachmentIndex];

				VkAttachmentDescription& resolveAttachmentDescription = attachmentDescriptions[attachmentReferenceOffset + resolveAttachmentIndex];
				resolveAttachmentDescription.flags = 0;
				resolveAttachmentDescription.format = surfaceFormat.format;
				resolveAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
				resolveAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachmentDescription.storeOp = static_cast<VkAttachmentStoreOp>(subpass->getColorAttachments()[resolveAttachmentIndex].storeOperation);
				resolveAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				resolveAttachmentDescription.initialLayout = colorAttachmentDescription.initialLayout;
				resolveAttachmentDescription.finalLayout = colorAttachmentDescription.finalLayout;

				VkAttachmentReference& resolveAttachmentReference = attachmentReferences[attachmentReferenceOffset + resolveAttachmentIndex];
				resolveAttachmentReference.attachment = attachmentReferenceOffset + resolveAttachmentIndex;
				resolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			} */

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

		sequence<VkSubpassDependency> subpassDependencies(descriptor.subpassDependencies.count);

		// TODO(subpass-refactor)
		for (uint32 subpassDependencyIndex = 0; subpassDependencyIndex < descriptor.subpassDependencies.count; subpassDependencyIndex += 1) {
			const SubpassDependency& dependency = descriptor.subpassDependencies[subpassDependencyIndex];

			VkSubpassDependency& subpassDependency = subpassDependencies[subpassDependencyIndex];
			subpassDependency.srcSubpass = dependency.sourceSubpassIndex;
			subpassDependency.dstSubpass = dependency.destinationSubpassIndex;
			subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dependencyFlags = 0;

			if (dependency.destinationSubpassIndex != SubpassDependency::EXTERNAL_SUBPASS) {
				const ref<Subpass>& destinationSubpass = descriptor.subpasses[dependency.destinationSubpassIndex];

				if (destinationSubpass->hasDepthStencilAttachmentReference()) {
					subpassDependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
					subpassDependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
					subpassDependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				}
			}
		}

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = static_cast<uint32>(attachmentDescriptions.count);
		renderPassCreateInfo.pAttachments = attachmentDescriptions;
		renderPassCreateInfo.subpassCount = static_cast<uint32>(subpassDescriptions.count);
		renderPassCreateInfo.pSubpasses = subpassDescriptions;
		renderPassCreateInfo.dependencyCount = static_cast<uint32>(subpassDependencies.count);
		renderPassCreateInfo.pDependencies = subpassDependencies;

		const VkResult result = vkCreateRenderPass(device.object, &renderPassCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan render pass ([]).", string_VkResult(result));
	}

	VulkanRenderPass::~VulkanRenderPass() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyRenderPass(device.object, object, nullptr);
	}

	bool VulkanRenderPass::isColorAttachment(uint32 attachmentIndex) const {
		for (const ref<Subpass>& subpass : subpasses) {
			for (const AttachmentReference& colorAttachmentReference : subpass->getColorAttachmentReferences()) {
				if (colorAttachmentReference.index == attachmentIndex) {
					return true;
				}
			}
		}

		return false;
	}
}