#include "pch.hpp"

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"

namespace ProjectA {
	VulkanRenderPass::VulkanRenderPass(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor),
		physicalDevice(physicalDevice),
		device(device),
		surface(&surface)
	{
		PROFILE_SCOPE(FUNC_SIG);

		fchain<VkAttachmentDescription> attachmentDescriptions(attachmentCount);
		fchain<VkAttachmentReference> attachmentReferences(attachmentCount);

		VkSampleCountFlags sampleCountFlags = 0;

		for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < descriptor.colorAttachments.size(); colorAttachmentIndex += 1) {
			const ColorAttachment& colorAttachment = descriptor.colorAttachments[colorAttachmentIndex];

			VkAttachmentDescription colorAttachmentDescription{};
			colorAttachmentDescription.flags = 0;
			colorAttachmentDescription.format = getVkFormat(colorAttachment.format);
			colorAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(colorAttachment.sampleCount);
			colorAttachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(colorAttachment.loadOperation);
			colorAttachmentDescription.storeOp = static_cast<VkAttachmentStoreOp>(colorAttachment.storeOperation);
			colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			if (resolveAttachmentCount == 0 && colorAttachmentIndex == presentAttachmentIndex) {
				colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			}
			else {
				colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			attachmentDescriptions[colorAttachmentIndex] = colorAttachmentDescription;

			VkAttachmentReference colorAttachmentReference{};
			colorAttachmentReference.attachment = colorAttachmentIndex;
			colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			attachmentReferences[colorAttachmentIndex] = colorAttachmentReference;

			sampleCountFlags |= colorAttachmentDescription.samples;
		}

		for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < resolveAttachmentCount; colorAttachmentIndex += 1) {
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

			attachmentDescriptions[colorAttachmentCount + colorAttachmentIndex] = resolveAttachmentDescription;

			VkAttachmentReference resolveAttachmentReference{};
			resolveAttachmentReference.attachment = colorAttachmentCount + colorAttachmentIndex;
			resolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			attachmentReferences[colorAttachmentCount + colorAttachmentIndex] = resolveAttachmentReference;
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

			sampleCountFlags |= depthStencilAttachmentDescription.samples;
		}

		sampleCountFlagBits = static_cast<VkSampleCountFlagBits>(sampleCountFlags);

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

	ref<Image> VulkanRenderPass::createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 colorAttachmentIndex) const {
		DEBUG_ASSERT(colorAttachmentIndex < colorAttachmentCount);
		DEBUG_ASSERT((resolveAttachmentCount == 0 && colorAttachmentIndex != presentAttachmentIndex) || resolveAttachmentCount > 0);

		const ColorAttachment& colorAttachment = colorAttachments[colorAttachmentIndex];

		ImageDescriptor colorImageDescriptor{};
		colorImageDescriptor.type = descriptor.type;
		colorImageDescriptor.format = colorAttachment.format;
		colorImageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT;
		colorImageDescriptor.width = descriptor.width;
		colorImageDescriptor.height = descriptor.height;
		colorImageDescriptor.depth = descriptor.depth;
		colorImageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		colorImageDescriptor.arrayLayerCount = descriptor.arrayLayerCount;
		colorImageDescriptor.sampleCount = colorAttachment.sampleCount;
		colorImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		return createRef<VulkanImage>(device.object, colorImageDescriptor);
	}

	ref<Image> VulkanRenderPass::createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor) const {
		DEBUG_ASSERT(depthStencil);

		ImageDescriptor depthStencilImageDescriptor{};
		depthStencilImageDescriptor.type = descriptor.type;
		depthStencilImageDescriptor.format = depthStencilAttachment.format;
		depthStencilImageDescriptor.usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT;
		depthStencilImageDescriptor.width = descriptor.width;
		depthStencilImageDescriptor.height = descriptor.height;
		depthStencilImageDescriptor.depth = descriptor.depth;
		depthStencilImageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		depthStencilImageDescriptor.arrayLayerCount = descriptor.arrayLayerCount;
		depthStencilImageDescriptor.sampleCount = depthStencilAttachment.sampleCount;
		depthStencilImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		return createRef<VulkanImage>(device.object, depthStencilImageDescriptor);
	}
}