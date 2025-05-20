#include "pch.hpp"

#include "Backend/Vulkan/VulkanRenderPass.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"
#include "Backend/Vulkan/SampleCount.hpp"

namespace ProjectA {
	VulkanRenderPass::VulkanRenderPass(const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor) :
		samples(getVkSampleCountFlagBits(descriptor.sampleCount)),
		device(device),
		surface(&surface)
	{
		PROFILE_SCOPE(FUNC_SIG);

		if (samples > VK_SAMPLE_COUNT_1_BIT) {
			attachmentCount += 1;
		}

		chain<VkAttachmentDescription> attachments(attachmentCount);

		VkAttachmentDescription& colorAttachmentDescription = attachments[0];
		colorAttachmentDescription.flags = 0;
		colorAttachmentDescription.format = surface.format.format;
		colorAttachmentDescription.samples = samples;
		colorAttachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(descriptor.loadOperation);
		colorAttachmentDescription.storeOp = static_cast<VkAttachmentStoreOp>(descriptor.storeOperation);
		colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// TODO: Fix undefined layout warning with a loadOp of VK_ATTACHMENT_LOAD_OP_LOAD.
		colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (samples > VK_SAMPLE_COUNT_1_BIT) {
			colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentDescription& resolveAttachmentDescription = attachments[1];
			resolveAttachmentDescription.flags = 0;
			resolveAttachmentDescription.format = surface.format.format;
			resolveAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
			resolveAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			resolveAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			resolveAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			resolveAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			resolveAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			resolveAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}
		else {
			colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}

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

		if (samples > VK_SAMPLE_COUNT_1_BIT) {
			VkAttachmentReference resolveAttachment{};
			resolveAttachment.attachment = 1;
			resolveAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			subpass.pResolveAttachments = &resolveAttachment;
		}
		else {
			subpass.pResolveAttachments = nullptr;
		}

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
		renderPassCreateInfo.attachmentCount = attachmentCount;
		renderPassCreateInfo.pAttachments = attachments.data();
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

	void VulkanRenderPass::recreateMultisampleImage(const VulkanSurface& surface) {
		VkImageCreateInfo multisampleImageCreateInfo{};
		multisampleImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		multisampleImageCreateInfo.pNext = nullptr;
		multisampleImageCreateInfo.flags = 0;
		multisampleImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		multisampleImageCreateInfo.format = surface.format.format;
		multisampleImageCreateInfo.extent.width = surface.window.getSize()[0];
		multisampleImageCreateInfo.extent.height = surface.window.getSize()[1];
		multisampleImageCreateInfo.extent.depth = 1;
		multisampleImageCreateInfo.mipLevels = 1;
		multisampleImageCreateInfo.arrayLayers = 1;
		multisampleImageCreateInfo.samples = samples;
		multisampleImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		multisampleImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		multisampleImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		multisampleImageCreateInfo.queueFamilyIndexCount = 0;
		multisampleImageCreateInfo.pQueueFamilyIndices = nullptr;

		multisampleImage = createScope<VulkanImage>(device, multisampleImageCreateInfo);

		const ImageViewDescriptor multisampleImageViewDescriptor;

		multisampleImageView = createScope<VulkanImageView>(device.object, *multisampleImage, multisampleImageViewDescriptor);
	}

	VkImageView VulkanRenderPass::getMultisampleImageView() const {
		return multisampleImageView->object;
	}
}