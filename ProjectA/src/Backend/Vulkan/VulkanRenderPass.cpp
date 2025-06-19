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

		const VkSurfaceFormatKHR surfaceFormat = surface.selectFormat(physicalDevice.object);

		sequence<VkAttachmentDescription> attachmentDescriptions(attachmentCount);
		sequence<VkAttachmentReference> attachmentReferences(attachmentCount);
		sequence<VkSubpassDescription> subpassDescriptions(descriptor.subpasses.count);
		sequence<VkSubpassDependency> subpassDependencies(descriptor.subpasses.count);

		uint32 subpassAttachmentOffset = 0;

		for (uint32 subpassIndex = 0; subpassIndex < descriptor.subpasses.count; subpassIndex += 1) {
			const Subpass& subpass = descriptor.subpasses[subpassIndex];

			uint32 colorAttachmentCount = static_cast<uint32>(subpass.colorAttachments.count);
			uint32 resolveAttachmentCount = 0;

			if (subpass.resolve) {
				resolveAttachmentCount = static_cast<uint32>(subpass.colorAttachments.count);
			}

			for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < subpass.colorAttachments.count; colorAttachmentIndex += 1) {
				const ColorAttachment& colorAttachment = subpass.colorAttachments[colorAttachmentIndex];

				VkAttachmentDescription& colorAttachmentDescription = attachmentDescriptions[subpassAttachmentOffset + colorAttachmentIndex];
				colorAttachmentDescription.flags = 0;
				colorAttachmentDescription.format = getVkFormat(colorAttachment.format);
				colorAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(colorAttachment.sampleCount);
				colorAttachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(colorAttachment.loadOperation);
				colorAttachmentDescription.storeOp = static_cast<VkAttachmentStoreOp>(colorAttachment.storeOperation);
				colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

				if (resolveAttachmentCount == 0 && colorAttachmentIndex == subpass.presentAttachmentIndex) {
					colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				}
				else {
					colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}

				VkAttachmentReference& colorAttachmentReference = attachmentReferences[subpassAttachmentOffset + colorAttachmentIndex];
				colorAttachmentReference.attachment = colorAttachmentIndex;

				// TODO: Use the same layout as the one from the attachment description?
				colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			subpassAttachmentOffset += colorAttachmentCount;

			for (uint32 resolveAttachmentIndex = 0; resolveAttachmentIndex < resolveAttachmentCount; resolveAttachmentIndex += 1) {
				VkAttachmentDescription& resolveAttachmentDescription = attachmentDescriptions[subpassAttachmentOffset + resolveAttachmentIndex];
				resolveAttachmentDescription.flags = 0;
				resolveAttachmentDescription.format = surfaceFormat.format;
				resolveAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
				resolveAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				resolveAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				resolveAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				resolveAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				VkAttachmentReference& resolveAttachmentReference = attachmentReferences[subpassAttachmentOffset + resolveAttachmentIndex];
				resolveAttachmentReference.attachment = subpassAttachmentOffset + resolveAttachmentIndex;
				resolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			subpassAttachmentOffset += resolveAttachmentCount;

			if (subpass.depthStencil) {
				VkAttachmentDescription& depthStencilAttachmentDescription = attachmentDescriptions[subpassAttachmentOffset];
				depthStencilAttachmentDescription.flags = 0;
				depthStencilAttachmentDescription.format = VK_FORMAT_D32_SFLOAT;
				depthStencilAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(subpass.depthStencilAttachment.sampleCount);
				depthStencilAttachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(subpass.depthStencilAttachment.loadOperation);

				// Vulkan can make optimizations if we explicitly tell it that we don't care about the depth data post-rendering.
				depthStencilAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				depthStencilAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				depthStencilAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthStencilAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				depthStencilAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkAttachmentReference& depthStencilAttachmentReference = attachmentReferences[subpassAttachmentOffset];
				depthStencilAttachmentReference.attachment = colorAttachmentCount + resolveAttachmentCount;
				depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}

			VkSubpassDescription& subpassDescription = subpassDescriptions[subpassIndex];
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

			if (subpass.depthStencil) {
				subpassDescription.pDepthStencilAttachment = &attachmentReferences[colorAttachmentCount + resolveAttachmentCount];
			}
			else {
				subpassDescription.pDepthStencilAttachment = nullptr;
			}

			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = nullptr;

			VkSubpassDependency& subpassDependency = subpassDependencies[subpassIndex];
			subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependency.dstSubpass = 0;
			subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.srcAccessMask = VK_ACCESS_NONE;
			subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dependencyFlags = 0;

			if (subpass.depthStencil) {
				subpassDependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				subpassDependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				subpassDependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			}
		}

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = attachmentCount;
		renderPassCreateInfo.pAttachments = attachmentDescriptions;
		renderPassCreateInfo.subpassCount = static_cast<uint32>(subpassDescriptions.count);
		renderPassCreateInfo.pSubpasses = subpassDescriptions;
		renderPassCreateInfo.dependencyCount = static_cast<uint32>(subpassDependencies.count);
		renderPassCreateInfo.pDependencies = subpassDependencies;

		const VkResult result = vkCreateRenderPass(device.object, &renderPassCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan render pass ([]).", string_VkResult(result));
	}

	VulkanRenderPass::~VulkanRenderPass() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyRenderPass(device.object, object, nullptr);
	}

	/* ref<Image> VulkanRenderPass::createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 colorAttachmentIndex) const {
		DEBUG_ASSERT(colorAttachmentIndex < colorAttachmentCount);
		DEBUG_ASSERT((resolveAttachmentCount == 0 && colorAttachmentIndex != presentAttachmentIndex) || resolveAttachmentCount > 0);

		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		const ColorAttachment& colorAttachment = colorAttachments[colorAttachmentIndex];

		ImageDescriptor colorImageDescriptor{};
		colorImageDescriptor.type = descriptor.type;
		colorImageDescriptor.format = colorAttachment.format;
		colorImageDescriptor.extent = descriptor.extent;
		colorImageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		colorImageDescriptor.layerCount = descriptor.layerCount;
		colorImageDescriptor.sampleCount = colorAttachment.sampleCount;
		colorImageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT;
		colorImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		return createRef<VulkanImage>(device.object, colorImageDescriptor);
	}

	ref<Image> VulkanRenderPass::createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor) const {
		DEBUG_ASSERT(depthStencil);

		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		ImageDescriptor depthStencilImageDescriptor{};
		depthStencilImageDescriptor.type = descriptor.type;
		depthStencilImageDescriptor.format = depthStencilAttachment.format;
		depthStencilImageDescriptor.extent = descriptor.extent;
		depthStencilImageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		depthStencilImageDescriptor.layerCount = descriptor.layerCount;
		depthStencilImageDescriptor.sampleCount = depthStencilAttachment.sampleCount;
		depthStencilImageDescriptor.usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT;
		depthStencilImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		return createRef<VulkanImage>(device.object, depthStencilImageDescriptor);
	} */
}