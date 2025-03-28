#include "pch.hpp"

#include "Backend/Vulkan/VulkanRenderPass.hpp"

namespace ProjectA {
	VulkanRenderPass::VulkanRenderPass(const VulkanSurface& surface, VkDevice device, const RenderPassDescriptor& descriptor) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkAttachmentDescription attachment{};
		attachment.flags = 0;
		attachment.format = surface.format.format;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = static_cast<VkAttachmentLoadOp>(descriptor.loadOperation);
		attachment.storeOp = static_cast<VkAttachmentStoreOp>(descriptor.storeOperation);
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// TODO: Fix undefined layout warning with a loadOp of VK_ATTACHMENT_LOAD_OP_LOAD.
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachment{};
		colorAttachment.attachment = 0;
		colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.flags = 0;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = nullptr;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachment;
		subpass.pResolveAttachments = nullptr;
		subpass.pDepthStencilAttachment = nullptr;
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = nullptr;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dependencyFlags = 0;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &attachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency;

		const VkResult result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &object);
		
		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan render pass (code: []).", result);
	}

	VulkanRenderPass::~VulkanRenderPass() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyRenderPass(device, object, nullptr);
	}
}