#include "pch.hpp"

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"
#include "Backend/Vulkan/ImageLayout.hpp"

namespace ProjectA {
	VulkanRenderPass::VulkanRenderPass(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor),
		physicalDevice(physicalDevice),
		device(device),
		surface(&surface)
	{
		PROFILE_SCOPE(FUNC_SIG);

		const VkSurfaceFormatKHR surfaceFormat = surface.selectFormat();

		sequence<VkAttachmentDescription> attachmentDescriptions(attachmentCount);
		sequence<VkAttachmentReference> attachmentReferences(attachmentCount);
		sequence<VkSubpassDescription> subpassDescriptions(descriptor.subpasses.count);

		uint32 subpassAttachmentOffset = 0;

		for (uint32 subpassIndex = 0; subpassIndex < descriptor.subpasses.count; subpassIndex += 1) {
			const ref<Subpass>& subpass = descriptor.subpasses[subpassIndex];
			const sequence<ColorAttachment>& colorAttachments = subpass->getColorAttachments();
			const uint32 colorAttachmentCount = static_cast<uint32>(colorAttachments.count);
			const uint32 resolveAttachmentCount = subpass->getResolveAttachmentCount();
			const uint32 depthStencilAttachmentCount = static_cast<uint32>(subpass->hasDepthStencilAttachment());

			for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < colorAttachmentCount; colorAttachmentIndex += 1) {
				const ColorAttachment& colorAttachment = colorAttachments[colorAttachmentIndex];

				VkAttachmentDescription& colorAttachmentDescription = attachmentDescriptions[subpassAttachmentOffset + colorAttachmentIndex];
				colorAttachmentDescription.flags = 0;
				colorAttachmentDescription.format = getVkFormat(colorAttachment.format);
				colorAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(colorAttachment.sampleCount);
				colorAttachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(colorAttachment.loadOperation);

				if (resolveAttachmentCount > 0) {
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

				colorAttachmentDescription.finalLayout = getVkImageLayout(colorAttachment.finalImageLayout);

				VkAttachmentReference& colorAttachmentReference = attachmentReferences[subpassAttachmentOffset + colorAttachmentIndex];
				colorAttachmentReference.attachment = colorAttachmentIndex;

				// TODO: Use the same layout as the one from the attachment description?
				colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			subpassAttachmentOffset += colorAttachmentCount;

			if (subpass->hasDepthStencilAttachment()) {
				const DepthStencilAttachment depthStencilAttachment = subpass->getDepthStencilAttachment();

				VkAttachmentDescription& depthStencilAttachmentDescription = attachmentDescriptions[subpassAttachmentOffset];
				depthStencilAttachmentDescription.flags = 0;
				depthStencilAttachmentDescription.format = VK_FORMAT_D32_SFLOAT;
				depthStencilAttachmentDescription.samples = static_cast<VkSampleCountFlagBits>(depthStencilAttachment.sampleCount);
				depthStencilAttachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(depthStencilAttachment.loadOperation);

				// Vulkan can make optimizations if we explicitly tell it that we don't care about the depth data post-rendering.
				depthStencilAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				depthStencilAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				depthStencilAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthStencilAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				depthStencilAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkAttachmentReference& depthStencilAttachmentReference = attachmentReferences[subpassAttachmentOffset];
				depthStencilAttachmentReference.attachment = subpassAttachmentOffset;
				depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}

			subpassAttachmentOffset += depthStencilAttachmentCount;

			for (uint32 resolveAttachmentIndex = 0; resolveAttachmentIndex < resolveAttachmentCount; resolveAttachmentIndex += 1) {
				const VkAttachmentDescription& colorAttachmentDescription = attachmentDescriptions[subpassAttachmentOffset - colorAttachmentCount - depthStencilAttachmentCount + resolveAttachmentIndex];

				VkAttachmentDescription& resolveAttachmentDescription = attachmentDescriptions[subpassAttachmentOffset + resolveAttachmentIndex];
				resolveAttachmentDescription.flags = 0;
				resolveAttachmentDescription.format = surfaceFormat.format;
				resolveAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
				resolveAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachmentDescription.storeOp = static_cast<VkAttachmentStoreOp>(colorAttachments[resolveAttachmentIndex].storeOperation);
				resolveAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				resolveAttachmentDescription.initialLayout = colorAttachmentDescription.initialLayout;
				resolveAttachmentDescription.finalLayout = colorAttachmentDescription.finalLayout;

				VkAttachmentReference& resolveAttachmentReference = attachmentReferences[subpassAttachmentOffset + resolveAttachmentIndex];
				resolveAttachmentReference.attachment = subpassAttachmentOffset + resolveAttachmentIndex;
				resolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			VkSubpassDescription& subpassDescription = subpassDescriptions[subpassIndex];
			subpassDescription.flags = 0;
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.inputAttachmentCount = 0;
			subpassDescription.pInputAttachments = nullptr;
			subpassDescription.colorAttachmentCount = colorAttachmentCount;
			subpassDescription.pColorAttachments = &attachmentReferences[0];

			if (resolveAttachmentCount > 0) {
				subpassDescription.pResolveAttachments = &attachmentReferences[colorAttachmentCount + depthStencilAttachmentCount];
			}
			else {
				subpassDescription.pResolveAttachments = nullptr;
			}

			if (subpass->hasDepthStencilAttachment()) {
				subpassDescription.pDepthStencilAttachment = &attachmentReferences[colorAttachmentCount];
			}
			else {
				subpassDescription.pDepthStencilAttachment = nullptr;
			}

			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = nullptr;
		}

		sequence<VkSubpassDependency> subpassDependencies(descriptor.subpassDependencies.count);

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

				if (destinationSubpass->hasDepthStencilAttachment()) {
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
}