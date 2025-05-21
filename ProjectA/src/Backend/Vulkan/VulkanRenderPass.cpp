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

		if (descriptor.depthStencilAttachment != nullptr) {
			depthStencilAttachmentCount = 1;
		}
	}

	VulkanRenderPass::VulkanRenderPass(const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor) :
		colorAttachments(descriptor.colorAttachments),
		hasDepthStencilAttachment(descriptor.depthStencilAttachment != nullptr),
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

		resolveAttachmentReferences.allocate(resolveAttachmentCount);
		resolveImages.resize(resolveAttachmentCount);
		resolveImageViews.resize(resolveAttachmentCount);

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

		VkSubpassDescription subpass{};
		subpass.flags = 0;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = nullptr;
		subpass.colorAttachmentCount = colorAttachmentCount;
		subpass.pColorAttachments = &colorAttachmentReferences[0];

		if (resolveAttachmentCount > 0) {
			subpass.pResolveAttachments = &resolveAttachmentReferences[0];
		}
		else {
			subpass.pResolveAttachments = nullptr;
		}

		if (depthStencilAttachmentCount > 0) {
			DEBUG_ASSERT(depthStencilAttachmentCount == 1);

			VkAttachmentDescription depthStencilAttachmentDescription{};
			depthStencilAttachmentDescription.flags = 0;
			depthStencilAttachmentDescription.format = surface.format.format;
			depthStencilAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(descriptor.depthStencilAttachment->sampleCount);
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

			subpass.pDepthStencilAttachment = &depthStencilAttachmentReference;
		}
		else {
			subpass.pDepthStencilAttachment = nullptr;
		}

		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = nullptr;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = VK_ACCESS_NONE;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dependencyFlags = 0;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = colorAttachmentCount + resolveAttachmentCount + depthStencilAttachmentCount;
		renderPassCreateInfo.pAttachments = &attachmentDescriptions[0];
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency;

		const VkResult result = vkCreateRenderPass(device.object, &renderPassCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan render pass (code: []).", result);
	}

	VulkanRenderPass::~VulkanRenderPass() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyRenderPass(device.object, object, nullptr);
	}

	VkImageView VulkanRenderPass::getAttachmentImageView(uint32 attachmentIndex) const {
		DEBUG_ASSERT(attachmentIndex < attachmentCount);

		if (attachmentIndex < resolveImageViews.size()) {
			return resolveImageViews[attachmentIndex]->object;
		}

		if (hasDepthStencilAttachment) {
			return depthStencilImageView->object;
		}

		DEBUG_ASSERT(false);

		return 0;
	}

	void VulkanRenderPass::recreateResolveAttachmentImageViews(const VulkanSurface& surface) {
		for (uint64 resolveAttachmentIndex = 0; resolveAttachmentIndex < resolveAttachmentReferences.count; resolveAttachmentIndex += 1) {
			const VkAttachmentReference& resolveAttachmentReference = resolveAttachmentReferences[resolveAttachmentIndex];

			VkImageCreateInfo resolveImageCreateInfo{};
			resolveImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			resolveImageCreateInfo.pNext = nullptr;
			resolveImageCreateInfo.flags = 0;
			resolveImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			resolveImageCreateInfo.format = surface.format.format;
			resolveImageCreateInfo.extent.width = surface.window.getSize()[0];
			resolveImageCreateInfo.extent.height = surface.window.getSize()[1];
			resolveImageCreateInfo.extent.depth = 1;
			resolveImageCreateInfo.mipLevels = 1;
			resolveImageCreateInfo.arrayLayers = 1;

			// TODO: Write correct sample count.
			resolveImageCreateInfo.samples = static_cast<VkSampleCountFlagBits>(colorAttachments[resolveAttachmentIndex].sampleCount);

			resolveImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			resolveImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
			resolveImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			resolveImageCreateInfo.queueFamilyIndexCount = 0;
			resolveImageCreateInfo.pQueueFamilyIndices = nullptr;

			resolveImages[resolveAttachmentIndex] = createScope<VulkanImage>(device, resolveImageCreateInfo);

			constexpr ImageViewDescriptor resolveImageViewDescriptor;

			resolveImageViews[resolveAttachmentIndex] = createScope<VulkanImageView>(device.object, *resolveImages[resolveAttachmentIndex], resolveImageViewDescriptor);
		}
	}

	void VulkanRenderPass::recreateDepthStencilAttachmentImageView(const VulkanSurface& surface) {
		// TODO
	}
}