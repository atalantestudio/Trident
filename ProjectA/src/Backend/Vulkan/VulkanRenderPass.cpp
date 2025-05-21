#include "pch.hpp"

#include "Backend/Vulkan/VulkanRenderPass.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"

namespace ProjectA {
	void VulkanRenderPass::countAttachments(const RenderPassDescriptor& descriptor, uint32& colorAttachmentCount, uint32& resolveAttachmentCount, uint32& depthStencilAttachmentCount) {
		colorAttachmentCount = 0;
		resolveAttachmentCount = 0;

		for (const Attachment& colorAttachment : descriptor.colorAttachments) {
			colorAttachmentCount += 1;

			if (colorAttachment.sampleCount > 1) {
				resolveAttachmentCount += 1;
			}
		}

		depthStencilAttachmentCount = 0;

		if (descriptor.enableDepthStencil) {
			depthStencilAttachmentCount = 1;
		}
	}

	VulkanRenderPass::VulkanRenderPass(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor.enableDepthStencil),
		physicalDevice(physicalDevice),
		colorAttachments(descriptor.colorAttachments),
		depthStencilAttachment(descriptor.depthStencilAttachment),
		device(device),
		surfaceExtentWidth(surface.window.getSize()[0]),
		surfaceExtentHeight(surface.window.getSize()[1])
	{
		PROFILE_SCOPE(FUNC_SIG);

		uint32 colorAttachmentCount;
		uint32 resolveAttachmentCount;
		uint32 depthStencilAttachmentCount;

		countAttachments(descriptor, colorAttachmentCount, resolveAttachmentCount, depthStencilAttachmentCount);

		attachmentCount = colorAttachmentCount + resolveAttachmentCount + depthStencilAttachmentCount;

		fchain<VkAttachmentDescription> attachmentDescriptions(attachmentCount);
		fchain<VkAttachmentReference> colorAttachmentReferences(colorAttachmentCount);
		fchain<VkAttachmentReference> resolveAttachmentReferences(resolveAttachmentCount);

		uint32 attachmentIndex = 0;
		uint32 colorAttachmentReferenceIndex = 0;
		uint32 resolveAttachmentReferenceIndex = 0;

		for (const Attachment& colorAttachment : descriptor.colorAttachments) {
			VkAttachmentDescription colorAttachmentDescription{};
			colorAttachmentDescription.flags = 0;
			colorAttachmentDescription.format = surface.format.format;
			colorAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(colorAttachment.sampleCount);
			colorAttachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(colorAttachment.loadOperation);
			colorAttachmentDescription.storeOp = static_cast<VkAttachmentStoreOp>(colorAttachment.storeOperation);
			colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			// TODO: Fix undefined layout warning with a loadOp of VK_ATTACHMENT_LOAD_OP_LOAD.
			colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			if (colorAttachment.sampleCount > 1) {
				// Attachments that are not presented to the screen should use VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL.
				colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			else {
				// Other attachments presented to the screen must use VK_IMAGE_LAYOUT_PRESENT_SRC_KHR.
				colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			}

			attachmentDescriptions[attachmentIndex] = colorAttachmentDescription;

			VkAttachmentReference colorAttachmentReference{};
			colorAttachmentReference.attachment = attachmentIndex;
			colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			colorAttachmentReferences[colorAttachmentReferenceIndex] = colorAttachmentReference;

			attachmentIndex += 1;
			colorAttachmentReferenceIndex += 1;

			if (colorAttachment.sampleCount > 1) {
				VkAttachmentDescription multisampleColorAttachmentDescription{};
				multisampleColorAttachmentDescription.flags = 0;
				multisampleColorAttachmentDescription.format = surface.format.format;
				multisampleColorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
				multisampleColorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				multisampleColorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				multisampleColorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				multisampleColorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				multisampleColorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				multisampleColorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				attachmentDescriptions[attachmentIndex] = multisampleColorAttachmentDescription;

				VkAttachmentReference multisampleColorAttachmentReference{};
				multisampleColorAttachmentReference.attachment = attachmentIndex;
				multisampleColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				resolveAttachmentReferences[resolveAttachmentReferenceIndex] = multisampleColorAttachmentReference;

				attachmentIndex += 1;
				resolveAttachmentReferenceIndex += 1;
			}
		}

		VkSubpassDescription subpassDescription{};
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = colorAttachmentCount;
		subpassDescription.pColorAttachments = &colorAttachmentReferences[0];

		if (resolveAttachmentCount > 0) {
			subpassDescription.pResolveAttachments = &resolveAttachmentReferences[0];
		}
		else {
			subpassDescription.pResolveAttachments = nullptr;
		}

		if (depthStencilAttachmentCount > 0) {
			DEBUG_ASSERT(depthStencilAttachmentCount == 1);

			VkAttachmentDescription depthStencilAttachmentDescription{};
			depthStencilAttachmentDescription.flags = 0;
			depthStencilAttachmentDescription.format = physicalDevice.findSuitableDepthStencilFormat();
			depthStencilAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(descriptor.depthStencilAttachment.sampleCount);
			depthStencilAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthStencilAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthStencilAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthStencilAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthStencilAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthStencilAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachmentDescriptions[attachmentIndex] = depthStencilAttachmentDescription;

			VkAttachmentReference depthStencilAttachmentReference{};
			depthStencilAttachmentReference.attachment = attachmentIndex;
			depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			subpassDescription.pDepthStencilAttachment = &depthStencilAttachmentReference;
		}
		else {
			subpassDescription.pDepthStencilAttachment = nullptr;
		}

		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkSubpassDependency subpassDependency{};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.srcAccessMask = VK_ACCESS_NONE;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependency.dependencyFlags = 0;

		if (depthStencil) {
			subpassDependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			subpassDependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			subpassDependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = attachmentCount;
		renderPassCreateInfo.pAttachments = &attachmentDescriptions[0];
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &subpassDependency;

		const VkResult result = vkCreateRenderPass(device.object, &renderPassCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan render pass (code: []).", result);
	}

	VulkanRenderPass::~VulkanRenderPass() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyRenderPass(device.object, object, nullptr);
	}
}