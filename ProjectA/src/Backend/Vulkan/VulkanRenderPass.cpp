#include "pch.hpp"

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"

namespace ProjectA {
	VulkanRenderPass::VulkanRenderPass(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor.depthStencilAttachment.enabled),
		physicalDevice(physicalDevice),
		colorAttachments(descriptor.colorAttachments),
		depthStencilAttachment(descriptor.depthStencilAttachment),
		device(device),
		surfaceExtentWidth(surface.window.getSize()[0]),
		surfaceExtentHeight(surface.window.getSize()[1])
	{
		PROFILE_SCOPE(FUNC_SIG);

		uint32 colorAttachmentCount = 0;
		uint32 resolveAttachmentCount = 0;

		for (const Attachment& colorAttachment : descriptor.colorAttachments) {
			colorAttachmentCount += 1;

			if (colorAttachment.sampleCount > 1) {
				resolveAttachmentCount += 1;
			}
		}

		DEBUG_ASSERT(colorAttachmentCount > 0);

		attachmentCount = colorAttachmentCount + resolveAttachmentCount;

		if (depthStencil) {
			attachmentCount += 1;
		}

		attachmentImageViews.resize(attachmentCount);

		fchain<VkAttachmentDescription> attachmentDescriptions(attachmentCount);
		fchain<VkAttachmentReference> attachmentReferences(attachmentCount);

		uint32 colorAttachmentIndex = 0;
		uint32 resolveAttachmentIndex = 0;

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

			attachmentDescriptions[colorAttachmentIndex] = colorAttachmentDescription;

			VkAttachmentReference colorAttachmentReference{};
			colorAttachmentReference.attachment = colorAttachmentIndex;
			colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			attachmentReferences[colorAttachmentIndex] = colorAttachmentReference;

			colorAttachmentIndex += 1;

			if (colorAttachment.sampleCount > 1) {
				VkAttachmentDescription resolveAttachmentDescription{};
				resolveAttachmentDescription.flags = 0;
				resolveAttachmentDescription.format = surface.format.format;
				resolveAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
				resolveAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				resolveAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				resolveAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				resolveAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				attachmentDescriptions[colorAttachmentCount + resolveAttachmentIndex] = resolveAttachmentDescription;

				VkAttachmentReference resolveAttachmentReference{};
				resolveAttachmentReference.attachment = colorAttachmentCount + resolveAttachmentIndex;
				resolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				attachmentReferences[colorAttachmentCount + resolveAttachmentIndex] = resolveAttachmentReference;

				resolveAttachmentIndex += 1;
			}
		}

		if (depthStencil) {
			VkAttachmentDescription depthStencilAttachmentDescription{};
			depthStencilAttachmentDescription.flags = 0;
			depthStencilAttachmentDescription.format = VK_FORMAT_D32_SFLOAT;
			depthStencilAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(descriptor.depthStencilAttachment.sampleCount);
			depthStencilAttachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(descriptor.depthStencilAttachment.loadOperation);

			// Vulkan can make optimizations if we explicitly don't care about the depth data post-rendering.
			depthStencilAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			depthStencilAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthStencilAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthStencilAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthStencilAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachmentDescriptions[colorAttachmentCount + resolveAttachmentCount] = depthStencilAttachmentDescription;

			VkAttachmentReference depthStencilAttachmentReference{};
			depthStencilAttachmentReference.attachment = colorAttachmentCount + resolveAttachmentCount;
			depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachmentReferences[colorAttachmentCount + resolveAttachmentCount] = depthStencilAttachmentReference;
		}

		VkSubpassDescription subpassDescription{};
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = colorAttachmentCount;
		subpassDescription.pColorAttachments = &attachmentReferences[0];

		if (resolveAttachmentCount > 0) {
			subpassDescription.pResolveAttachments = &attachmentReferences[colorAttachmentCount];
		}
		else {
			subpassDescription.pResolveAttachments = nullptr;
		}

		if (depthStencil) {
			subpassDescription.pDepthStencilAttachment = &attachmentReferences[colorAttachmentCount + resolveAttachmentCount];
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

	void VulkanRenderPass::setAttachmentImageView(uint32 attachmentIndex, const ref<ImageView>& imageView) {
		DEBUG_ASSERT(attachmentIndex < attachmentCount);

		attachmentImageViews[attachmentIndex] = imageView;
	}

	const chain<ref<ImageView>>& VulkanRenderPass::getAttachmentImageViews() const {
		return attachmentImageViews;
	}
}