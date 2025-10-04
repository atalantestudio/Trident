#include "pch.hpp"

#include "Engine/API/Rendering/Descriptor/AttachmentImageDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/BeginRenderPassDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/BufferDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/CommandBufferDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/ComputePipelineDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/FramebufferDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/ImageViewDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/PipelineLayoutDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/RenderPassDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/RenderPipelineDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/SamplerDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/ShaderModuleDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/SubpassDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/SwapchainDescriptor.hpp"
#include "Engine/API/Rendering/SampleCount.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	void DescriptorGuard(const AttachmentImageDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.type >= ImageType::__MAX_ENUM) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.type must be a valid ImageType value.");
		}

		if (descriptor.extent.width == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.width must be greater than 0.");
		}

		if (descriptor.extent.height == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.height must be greater than 0.");
		}

		if (descriptor.extent.depth == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.depth must be greater than 0.");
		}

		if (descriptor.mipLevelCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.mipLevelCount must be greater than 0.");
		}

		if (descriptor.layerCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.layerCount must be greater than 0.");
		}

		ENGINE_ASSERT(errorCount == 0, "Attachment image descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const BeginRenderPassDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.framebuffer == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.framebuffer must be a valid Framebuffer.");
		}
		else {
			const ref<RenderPass>& renderPass = descriptor.framebuffer->getRenderPass();

			if (renderPass == nullptr) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.framebuffer.renderPass must be a valid RenderPass.");
			}
			else {
				if (descriptor.clearValues.count > renderPass->getAttachmentReferenceCount()) {
					errorCount += 1;

					ENGINE_ERROR("descriptor.clearValues.count must be smaller than the number of attachment references in descriptor.framebuffer.renderPass.");
				}
			}
		}

		ENGINE_ASSERT(errorCount == 0, "Render pass begin descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const BufferDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.size == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.size must not be 0.");
		}

		if (descriptor.persistent != true && descriptor.persistent != false) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.persistent must be either true or false.");
		}

		ENGINE_ASSERT(errorCount == 0, "Buffer descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const CommandBufferDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		ENGINE_ASSERT(errorCount == 0, "Command buffer descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const ComputePipelineDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.pipelineLayout == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.pipelineLayout must be a valid PipelineLayout.");
		}

		if (descriptor.computeShaderModule == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.computeShaderModule must be a valid compute ShaderModule.");
		}

		ENGINE_ASSERT(errorCount == 0, "Compute pipeline descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const FramebufferDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.renderPass == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.renderPass must be a valid RenderPass.");
		}
		else {
			// TODO(subpass-refactor): The number of image views must be the same as the number of attachments (including resolve), not attachment references.
			/* if (descriptor.imageViews.count != descriptor.renderPass->getAttachmentReferenceCount()) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.imageViews.count must be equal to the attachment reference count specified in descriptor.renderPass.");
			} */
		}

		for (uint32 imageViewIndex = 0; imageViewIndex < descriptor.imageViews.count; imageViewIndex += 1) {
			const ref<ImageView>& imageView = descriptor.imageViews[imageViewIndex];

			if (imageView == nullptr) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.imageViews[[]] must be a valid ImageView.", imageViewIndex);
			}

			if (descriptor.extent.width > imageView->getImage()->getExtent().width) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.imageViews[[]] must have been created with a width greater than or equal to descriptor.extent.width.", imageViewIndex);
			}

			if (descriptor.extent.height > imageView->getImage()->getExtent().height) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.imageViews[[]] must have been created with a height greater than or equal to descriptor.extent.height.", imageViewIndex);
			}

			if (descriptor.layerCount > imageView->getLayerCount()) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.imageViews[[]] must have been created with a layer count greater than or equal to descriptor.layerCount.", imageViewIndex);
			}
		}

		if (descriptor.extent.width == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.width must be greater than 0.");
		}

		if (descriptor.extent.height == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.height must be greater than 0.");
		}

		if (descriptor.layerCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.layerCount must be greater than 0.");
		}

		ENGINE_ASSERT(errorCount == 0, "Framebuffer descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const ImageDescriptor& descriptor) {
		const PhysicalDeviceLimits& limits = Renderer::getBackend()->getDevice()->getPhysicalDevice()->getLimits();

		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.type >= ImageType::__MAX_ENUM) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.type must be a valid ImageType value.");
		}

		if (descriptor.format >= ImageFormat::__MAX_ENUM) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.format must be a valid ImageFormat value.");
		}

		if (descriptor.extent.width == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.width must not be 0.");
		}

		if (descriptor.extent.width > max(limits.maxImageDimension1D, max(limits.maxImageDimension2D, limits.maxImageDimension3D))) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.width must be less than the maximum dimension for 1D, 2D or 3D images.");
		}

		if (descriptor.extent.height == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.height must not be 0.");
		}

		if (descriptor.extent.height > max(limits.maxImageDimension2D, limits.maxImageDimension3D)) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.height must be less than the maximum dimension for 2D or 3D images.");
		}

		if (descriptor.extent.depth == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.depth must not be 0.");
		}

		if (descriptor.extent.depth > limits.maxImageDimension3D) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.depth must be less than the maximum dimension for 3D images.");
		}

		if (descriptor.mipLevelCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.mipLevelCount must not be 0.");
		}

		if (descriptor.mipLevelCount > floor(log2(max(descriptor.extent.width, max(descriptor.extent.height, descriptor.extent.depth)))) + 1) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.mipLevelCount must be less than the maximum mip level for this size.");
		}

		if (descriptor.layerCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.layerCount must not be 0.");
		}

		if (descriptor.layerCount > limits.maxImageLayerCount) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.layerCount must be less than the maximum layer count.");
		}

		if (!validateSampleCount(descriptor.sampleCount)) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.sampleCount must either be 1 or a power of 2 (up to 64).");
		}

		ENGINE_ASSERT(errorCount == 0, "Image descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const ImageViewDescriptor& descriptor, const Image* image) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.mipLevelCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.mipLevelCount must be greater than 0.");
		}

		if (descriptor.mipLevelCount > image->getMipLevelCount()) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.mipLevelCount must be less than the number of mip levels on the image.");
		}

		if (descriptor.layerCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.layerCount must be greater than 0.");
		}

		if (descriptor.layerCount > image->getLayerCount()) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.layerCount must be less than the number of layers on the image.");
		}

		ENGINE_ASSERT(errorCount == 0, "Image view descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const PipelineLayoutDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		for (uint32 descriptorSetLayoutIndex = 0; descriptorSetLayoutIndex < descriptor.descriptorSetLayouts.count; descriptorSetLayoutIndex += 1) {
			const ref<DescriptorSetLayout>& descriptorSetLayout = descriptor.descriptorSetLayouts[descriptorSetLayoutIndex];

			for (uint32 descriptorSetLayoutBindingIndex = 0; descriptorSetLayoutBindingIndex < descriptorSetLayout->bindings.count; descriptorSetLayoutBindingIndex += 1) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout->bindings[descriptorSetLayoutBindingIndex];

				if (descriptorSetLayoutBinding.descriptorType >= DescriptorType::__MAX_ENUM) {
					errorCount += 1;

					ENGINE_ERROR("descriptor.descriptorSetLayouts[[]]->bindings[[]].descriptorType must be a valid DescriptorType value.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex);
				}

				if (descriptorSetLayoutBinding.descriptorCount == 0) {
					errorCount += 1;

					ENGINE_ERROR("descriptor.descriptorSetLayouts[[]]->bindings[[]].descriptorCount must not be 0.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex);
				}
			}
		}

		ENGINE_ASSERT(errorCount == 0, "Pipeline layout descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const RenderPassDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.attachments.count == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.attachments must not be empty.");
		}

		for (uint32 attachmentIndex = 0; attachmentIndex < descriptor.attachments.count; attachmentIndex += 1) {
			const Attachment& attachment = descriptor.attachments[attachmentIndex];

			if (attachment.format >= ImageFormat::__MAX_ENUM) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.attachments[[]].format must be a valid ImageFormat value.", attachmentIndex);
			}

			if (!validateSampleCount(attachment.sampleCount)) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.attachments[[]].sampleCount must either be 1 or a power of 2 (up to 64).", attachmentIndex);
			}

			if (attachment.loadOperation >= LoadOperation::__MAX_ENUM) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.attachments[[]].loadOperation must be a valid LoadOperation value.", attachmentIndex);
			}

			if (attachment.storeOperation >= StoreOperation::__MAX_ENUM) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.attachments[[]].storeOperation must be a valid StoreOperation value.", attachmentIndex);
			}

			if (attachment.finalLayout >= ImageLayout::__MAX_ENUM) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.attachments[[]].finalLayout must be a valid ImageLayout value.", attachmentIndex);
			}
		}

		if (descriptor.subpasses.count == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.subpasses must not be empty.");
		}

		ENGINE_ASSERT(errorCount == 0, "Render pass descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const RenderPipelineDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.pipelineLayout == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.pipelineLayout must be a valid PipelineLayout.");
		}

		for (uint32 vertexInputBindingIndex = 0; vertexInputBindingIndex < descriptor.vertexInputBindings.count; vertexInputBindingIndex += 1) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[vertexInputBindingIndex];

			if (vertexInputBinding.getInputRate() >= InputRate::__MAX_ENUM) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.vertexInputBindings[[]].inputRate must be a valid InputRate value.", vertexInputBindingIndex);
			}

			const std::vector<VertexInputAttribute>& vertexInputAttributes = vertexInputBinding.getAttributes();

			for (uint32 vertexInputAttributeIndex = 0; vertexInputAttributeIndex < vertexInputAttributes.size(); vertexInputAttributeIndex += 1) {
				const VertexInputAttribute& vertexInputAttribute = vertexInputAttributes[vertexInputAttributeIndex];

				if (vertexInputAttribute.type >= VertexInputAttributeType::__MAX_ENUM) {
					errorCount += 1;

					ENGINE_ERROR("descriptor.vertexInputBindings[[]].attributes[[]].type must be a valid VertexInputAttributeType value.", vertexInputBindingIndex, vertexInputAttributeIndex);
				}
			}
		}

		if (descriptor.vertexShaderModule == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.vertexShaderModule must be a valid vertex ShaderModule.");
		}

		if (descriptor.fragmentShaderModule == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.fragmentShaderModule must be a valid fragment ShaderModule.");
		}

		if (descriptor.primitiveTopology >= PrimitiveTopology::__MAX_ENUM) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.primitiveTopology must be a valid PrimitiveTopology value.");
		}

		if (descriptor.polygonMode >= PolygonMode::__MAX_ENUM) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.polygonMode must be a valid PolygonMode value.");
		}

		if (descriptor.cullMode >= CullMode::__MAX_ENUM) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.cullMode must be a valid CullMode value.");
		}

		if (descriptor.frontFace >= FrontFace::__MAX_ENUM) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.frontFace must be a valid FrontFace value.");
		}

		if (descriptor.lineWidth <= 0.0f) {
			warningCount += 1;

			ENGINE_WARN("descriptor.lineWidth should be greater than 0.");
		}

		if (descriptor.minSampleShading < 0.0f || descriptor.minSampleShading > 1.0f) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.minSampleShading must be in the range [0, 1].");
		}

		// TODO: Compare to each subpass.
		/* if (descriptor.colorBlendState.attachments.size() != descriptor.renderPass->getColorAttachmentCount()) {
			errorCount += 1;

			ENGINE_ERROR("The number of color blend attachments in descriptor.colorBlendState.attachments must be equal to the color attachment count of descriptor.renderPass.");
		} */

		uint32 colorBlendAttachmentIndex = 0;

		for (uint32 colorBlendAttachmentIndex = 0; colorBlendAttachmentIndex < descriptor.colorBlendState.attachments.size(); colorBlendAttachmentIndex += 1) {
			const ColorBlendAttachment& colorBlendAttachment = descriptor.colorBlendState.attachments[colorBlendAttachmentIndex];

			if (colorBlendAttachment.blend != true && colorBlendAttachment.blend != false) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.colorBlendState.attachments[[]].blend must be either true or false.", colorBlendAttachmentIndex);
			}

			if (colorBlendAttachment.blend) {
				if (colorBlendAttachment.srcColorBlendFactor >= BlendFactor::__MAX_ENUM) {
					errorCount += 1;

					ENGINE_ERROR("If descriptor.colorBlendState.attachments[[]].blend is true, descriptor.colorBlendState.attachments[[]].srcColorBlendFactor must be a valid BlendFactor value.", colorBlendAttachmentIndex, colorBlendAttachmentIndex);
				}

				if (colorBlendAttachment.dstColorBlendFactor >= BlendFactor::__MAX_ENUM) {
					errorCount += 1;

					ENGINE_ERROR("If descriptor.colorBlendState.attachments[[]].blend is true, descriptor.colorBlendState.attachments[[]].dstColorBlendFactor must be a valid BlendFactor value.", colorBlendAttachmentIndex, colorBlendAttachmentIndex);
				}
			}
		}

		if (descriptor.depthStencilState.depthTest != true && descriptor.depthStencilState.depthTest != false) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.depthStencilState.depthTest must be either true or false.");
		}

		if (descriptor.depthStencilState.depthTest) {
			if (descriptor.depthStencilState.depthWrite != true && descriptor.depthStencilState.depthWrite != false) {
				errorCount += 1;

				ENGINE_ERROR("If descriptor.depthStencilState.depthStencil is true, descriptor.depthStencilState.depthWrite must be either true or false.");
			}

			if (descriptor.depthStencilState.compareOperation >= CompareOperation::__MAX_ENUM) {
				errorCount += 1;

				ENGINE_ERROR("If descriptor.depthStencilState.depthStencil is true, descriptor.depthStencilState.compareOperation must be a valid CompareOperation value.");
			}
		}

		if (!(descriptor.dynamicState & DynamicState::SCISSOR)) {
			warningCount += 1;

			ENGINE_WARN("descriptor.dynamicState should include DynamicState::SCISSOR.");
		}

		if (!(descriptor.dynamicState & DynamicState::VIEWPORT)) {
			warningCount += 1;

			ENGINE_WARN("descriptor.dynamicState should include DynamicState::VIEWPORT.");
		}

		if (descriptor.renderPass == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.renderPass must be a valid RenderPass.");
		}
		else {
			if (descriptor.subpassIndex >= descriptor.renderPass->getSubpassCount()) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.subpassIndex must be lesser than the number of subpasses in descriptor.renderPass.");
			}
			else {
				if (descriptor.depthStencilState.depthTest && !descriptor.renderPass->getSubpass(descriptor.subpassIndex)->hasDepthStencilAttachmentReference()) {
					warningCount += 1;

					ENGINE_WARN("descriptor.depthStencilState.depthTest is true but the subpass does not contain a depth stencil attachment.");
				}
			}
		}

		ENGINE_ASSERT(errorCount == 0, "Render pipeline descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const SamplerDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.magFilter >= SamplerFilter::__MAX_ENUM) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.magFilter must be a valid SamplerFilter value.");
		}

		if (descriptor.minFilter >= SamplerFilter::__MAX_ENUM) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.minFilter must be a valid SamplerFilter value.");
		}

		ENGINE_ASSERT(errorCount == 0, "Sampler descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const ShaderModuleDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.entryPoint.empty()) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.entryPoint must not be empty.");
		}

		ENGINE_ASSERT(errorCount == 0, "Shader module descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const SubpassDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;
		bool anyUsedColorAttachment = false;

		if (descriptor.colorAttachmentReferences.count == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.colorAttachmentReferences must not be empty.");
		}

		for (uint32 colorAttachmentReferenceIndex = 0; colorAttachmentReferenceIndex < descriptor.colorAttachmentReferences.count; colorAttachmentReferenceIndex += 1) {
			const AttachmentReference& colorAttachmentReference = descriptor.colorAttachmentReferences[colorAttachmentReferenceIndex];

			if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
				continue;
			}

			anyUsedColorAttachment = true;

			if (colorAttachmentReference.layout >= ImageLayout::__MAX_ENUM) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.colorAttachmentReferences[[]].layout must be a valid ImageLayout value.", colorAttachmentReferenceIndex);
			}
		}

		if (!anyUsedColorAttachment) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.colorAttachmentReferences must contain at least one used attachment.");
		}

		if (descriptor.depthStencilAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX && descriptor.depthStencilAttachmentReference.layout >= ImageLayout::__MAX_ENUM) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.depthStencilAttachmentReference.layout must be a valid ImageLayout value.");
		}

		for (uint32 inputAttachmentReferenceIndex = 0; inputAttachmentReferenceIndex < descriptor.inputAttachmentReferences.count; inputAttachmentReferenceIndex += 1) {
			const AttachmentReference& inputAttachmentReference = descriptor.inputAttachmentReferences[inputAttachmentReferenceIndex];

			if (inputAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
				continue;
			}

			anyUsedColorAttachment = true;

			if (inputAttachmentReference.layout >= ImageLayout::__MAX_ENUM) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.inputAttachmentReferences[[]].layout must be a valid ImageLayout value.", inputAttachmentReferenceIndex);
			}
		}

		ENGINE_ASSERT(errorCount == 0, "Subpass descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void DescriptorGuard(const SwapchainDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.vSync != true && descriptor.vSync != false) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.vSync must be either true or false.");
		}

		if (descriptor.extent.width == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.width must not be 0.");
		}

		if (descriptor.extent.height == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.height must not be 0.");
		}

		ENGINE_ASSERT(errorCount == 0, "Swapchain descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}
}