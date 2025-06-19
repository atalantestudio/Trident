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
#include "Engine/API/Rendering/Descriptor/SwapchainDescriptor.hpp"
#include "Engine/API/Rendering/Subpass.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	static bool validateSampleCount(uint8 sampleCount) {
		return (
			sampleCount == 1 ||
			sampleCount == 2 ||
			sampleCount == 4 ||
			sampleCount == 8 ||
			sampleCount == 16 ||
			sampleCount == 32 ||
			sampleCount == 64
		);
	}

	void validateDescriptor(const AttachmentImageDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.type > ImageType::TEXTURE_2D) {
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

	void validateDescriptor(const BeginRenderPassDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.framebuffer == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.framebuffer must be a valid Framebuffer.");
		}
		else {
			if (descriptor.clearValues.size() < descriptor.framebuffer->getRenderPass()->getClearValueCount()) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.clearValues must have a number of clear values greater than or equal to the last attachment index in descriptor.framebuffer.renderPass having a load operation of LoadOperation::CLEAR.");
			}
		}

		ENGINE_ASSERT(errorCount == 0, "Render pass begin descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void validateDescriptor(const BufferDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		// TODO: Also check size upper bound.
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

	void validateDescriptor(const CommandBufferDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		ENGINE_ASSERT(errorCount == 0, "Command buffer descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void validateDescriptor(const ComputePipelineDescriptor& descriptor) {
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

	void validateDescriptor(const FramebufferDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.renderPass == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.renderPass must be a valid RenderPass.");
		}
		else {
			if (descriptor.imageViews.count != descriptor.renderPass->getAttachmentCount()) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.imageViews.count must be equal to the attachment count specified in descriptor.renderPass.");
			}
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

	void validateDescriptor(const ImageDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.type > ImageType::TEXTURE_2D) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.type must be a valid ImageType value.");
		}

		if (descriptor.format > ImageFormat::D24_UNORM_S8_UINT) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.format must be a valid ImageFormat value.");
		}

		// TODO: Check max width.
		if (descriptor.extent.width == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.width must not be 0.");
		}

		// TODO: Check max height.
		if (descriptor.extent.height == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.height must not be 0.");
		}

		// TODO: Check max depth.
		if (descriptor.extent.depth == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.extent.depth must not be 0.");
		}

		// TODO: Check max mip level count.
		if (descriptor.mipLevelCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.mipLevelCount must not be 0.");
		}

		// TODO: Check max array layer count.
		if (descriptor.layerCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.layerCount must not be 0.");
		}

		if (!validateSampleCount(descriptor.sampleCount)) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.sampleCount must either be 1 or a power of 2 (up to 64).");
		}

		ENGINE_ASSERT(errorCount == 0, "Image descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void validateDescriptor(const ImageViewDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		// TODO: "subresourceRange.baseMipLevel must be less than the mipLevels specified in VkImageCreateInfo when image was created"
		//       To implement this, we need a reference to the Image that was used to create this ImageView. Maybe put it in the descriptor?

		if (descriptor.mipLevelCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.mipLevelCount must be greater than 0.");
		}

		if (descriptor.layerCount == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.layerCount must be greater than 0.");
		}

		ENGINE_ASSERT(errorCount == 0, "Image view descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void validateDescriptor(const PipelineLayoutDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		for (uint32 descriptorSetLayoutIndex = 0; descriptorSetLayoutIndex < descriptor.layouts.size(); descriptorSetLayoutIndex += 1) {
			const DescriptorSetLayout& descriptorSetLayout = descriptor.layouts[descriptorSetLayoutIndex];

			for (uint32 descriptorSetLayoutBindingIndex = 0; descriptorSetLayoutBindingIndex < descriptorSetLayout.bindings.size(); descriptorSetLayoutBindingIndex += 1) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout.bindings[descriptorSetLayoutBindingIndex];

				if (descriptorSetLayoutBinding.descriptorType > DescriptorType::STORAGE_IMAGE) {
					errorCount += 1;

					ENGINE_ERROR("descriptor.layouts[[]].bindings[[]].descriptorType must be a valid DescriptorType value.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex);
				}

				if (descriptorSetLayoutBinding.stageFlags > ShaderStage::COMPUTE) {
					errorCount += 1;

					ENGINE_ERROR("descriptor.layouts[[]].bindings[[]].stageFlags must be a valid ShaderStage value.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex);
				}

				for (uint32 bufferIndex = 0; bufferIndex < descriptorSetLayoutBinding.buffers.size(); bufferIndex += 1) {
					const ref<Buffer>& buffer = descriptorSetLayoutBinding.buffers[bufferIndex];

					if (buffer == nullptr) {
						errorCount += 1;

						ENGINE_ERROR("descriptor.layouts[[]].bindings[[]].buffers[[]] must be a valid Buffer.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex, bufferIndex);
					}
				}

				if (!descriptorSetLayoutBinding.buffers.empty() && descriptorSetLayoutBinding.bufferOffset > 0) {
					errorCount += 1;

					ENGINE_ERROR("descriptor.layouts[[]].bindings[[]].bufferOffset must be 0 if the binding doesn't have any buffers.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex);
				}

				for (uint32 imageViewIndex = 0; imageViewIndex < descriptorSetLayoutBinding.imageViews.size(); imageViewIndex += 1) {
					const ref<ImageView>& imageView = descriptorSetLayoutBinding.imageViews[imageViewIndex];

					if (imageView == nullptr) {
						errorCount += 1;

						ENGINE_ERROR("descriptor.layouts[[]].bindings[[]].imageViews[[]] must be a valid ImageView.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex, imageViewIndex);
					}
				}

				for (uint32 samplerIndex = 0; samplerIndex < descriptorSetLayoutBinding.samplers.size(); samplerIndex += 1) {
					const ref<Sampler>& sampler = descriptorSetLayoutBinding.samplers[samplerIndex];

					if (sampler == nullptr) {
						errorCount += 1;

						ENGINE_ERROR("descriptor.layouts[[]].bindings[[]].samplers[[]] must be a valid Sampler.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex, samplerIndex);
					}
				}

				if (descriptorSetLayoutBinding.imageViews.size() != descriptorSetLayoutBinding.samplers.size()) {
					errorCount += 1;

					ENGINE_ERROR("The number of image views in descriptor.layouts[[]].bindings[[]].imageViews must be the same as the number of samplers in descriptor.layouts[[]].bindings[[]].samplers.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex, descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex);
				}
			}
		}

		ENGINE_ASSERT(errorCount == 0, "Pipeline layout descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void validateDescriptor(const RenderPassDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.subpasses.count == 0) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.subpasses must not be empty.");
		}

		ENGINE_ASSERT(errorCount == 0, "Render pass descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void validateDescriptor(const RenderPipelineDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.pipelineLayout == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.pipelineLayout must be a valid PipelineLayout.");
		}

		for (uint32 vertexInputBindingIndex = 0; vertexInputBindingIndex < descriptor.vertexInputBindings.count; vertexInputBindingIndex += 1) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[vertexInputBindingIndex];

			if (vertexInputBinding.getInputRate() > InputRate::INSTANCE) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.vertexInputBindings[[]].inputRate must be a valid InputRate value.", vertexInputBindingIndex);
			}

			const std::vector<VertexInputAttribute>& vertexInputAttributes = vertexInputBinding.getAttributes();

			for (uint32 vertexInputAttributeIndex = 0; vertexInputAttributeIndex < vertexInputAttributes.size(); vertexInputAttributeIndex += 1) {
				const VertexInputAttribute& vertexInputAttribute = vertexInputAttributes[vertexInputAttributeIndex];

				if (vertexInputAttribute.type > VertexInputAttributeType::R64G64B64A64_FLOAT) {
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

		if (descriptor.primitiveTopology > PrimitiveTopology::TRIANGLE_STRIP) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.primitiveTopology must be a valid PrimitiveTopology value.");
		}

		if (descriptor.polygonMode > PolygonMode::FILL) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.polygonMode must be a valid PolygonMode value.");
		}

		if (descriptor.cullMode > CullMode::FRONT_AND_BACK) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.cullMode must be a valid CullMode value.");
		}

		if (descriptor.frontFace > FrontFace::COUNTER_CLOCKWISE) {
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
				if (colorBlendAttachment.srcColorBlendFactor > BlendFactor::ONE_MINUS_SRC1_ALPHA) {
					errorCount += 1;

					ENGINE_ERROR("If descriptor.colorBlendState.attachments[[]].blend is true, descriptor.colorBlendState.attachments[[]].srcColorBlendFactor must be a valid BlendFactor value.", colorBlendAttachmentIndex, colorBlendAttachmentIndex);
				}

				if (colorBlendAttachment.dstColorBlendFactor > BlendFactor::ONE_MINUS_SRC1_ALPHA) {
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

			if (descriptor.depthStencilState.compareOperation > CompareOperation::ALWAYS) {
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
				if (descriptor.depthStencilState.depthTest != descriptor.renderPass->getSubpass(descriptor.subpassIndex).depthStencil) {
					warningCount += 1;

					ENGINE_WARN("descriptor.depthStencilState.depthTest is not the same as descriptor.renderPass.subpasses[descriptor.subpassIndex].depthStencil.");
				}
			}
		}

		ENGINE_ASSERT(errorCount == 0, "Render pipeline descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void validateDescriptor(const SamplerDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.magFilter > SamplerFilter::LINEAR) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.magFilter must be a valid SamplerFilter value.");
		}

		if (descriptor.minFilter > SamplerFilter::LINEAR) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.minFilter must be a valid SamplerFilter value.");
		}

		ENGINE_ASSERT(errorCount == 0, "Sampler descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void validateDescriptor(const ShaderModuleDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.stage > ShaderStage::COMPUTE) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.stage must be a valid ShaderStage value.");
		}

		if (descriptor.entryPoint.empty()) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.entryPoint must not be empty.");
		}

		ENGINE_ASSERT(errorCount == 0, "Shader module descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	void validateDescriptor(const Subpass& subpass) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		/* if (descriptor.colorAttachments.empty()) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.colorAttachments must not be empty.");
		}

		if (descriptor.resolve != true && descriptor.resolve != false) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.resolve must be either true or false.");
		}

		if (descriptor.depthStencil != true && descriptor.depthStencil != false) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.depthStencil must be either true or false.");
		}

		if (descriptor.presentAttachmentIndex >= descriptor.colorAttachments.size()) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.presentAttachmentIndex must be less than the number of color attachments.");
		}

		for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < descriptor.colorAttachments.size(); colorAttachmentIndex += 1) {
			const ColorAttachment& colorAttachment = descriptor.colorAttachments[colorAttachmentIndex];

			if (colorAttachment.format > ImageFormat::R8G8B8A8_UNORM) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.colorAttachments[[]].format must be a valid color ImageFormat value.", colorAttachmentIndex);
			}

			if (!validateSampleCount(colorAttachment.sampleCount)) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.colorAttachments[[]].sampleCount must either be 1 or a power of 2 (up to 64).", colorAttachmentIndex);
			}

			if (colorAttachment.sampleCount != 1 && !descriptor.resolve) {
				errorCount += 1;

				ENGINE_ERROR("If descriptor.resolve is false, descriptor.colorAttachments[[]].sampleCount must be 1.", colorAttachmentIndex);
			}

			if (colorAttachment.loadOperation > LoadOperation::UNDEFINED) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.colorAttachments[[]].loadOperation must be a valid LoadOperation value.", colorAttachmentIndex);
			}

			if (colorAttachment.storeOperation > StoreOperation::UNDEFINED) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.colorAttachments[[]].storeOperation must be a valid StoreOperation value.", colorAttachmentIndex);
			}
		}

		if (descriptor.depthStencil) {
			if (descriptor.depthStencilAttachment.format < ImageFormat::D32_SFLOAT || descriptor.depthStencilAttachment.format > ImageFormat::D24_UNORM_S8_UINT) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.depthStencilAttachment.format must be a valid depth stencil ImageFormat value.");
			}

			if (!validateSampleCount(descriptor.depthStencilAttachment.sampleCount)) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.depthStencilAttachment.sampleCount must either be 1 or a power of 2 (up to 64).");
			}

			if (descriptor.depthStencilAttachment.sampleCount != descriptor.colorAttachments[descriptor.presentAttachmentIndex].sampleCount) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.depthStencilAttachment.sampleCount must be equal to the sample count of the color attachment specified as present attachment by descriptor.presentAttachmentIndex ([]).", descriptor.presentAttachmentIndex);
			}

			if (descriptor.depthStencilAttachment.loadOperation > LoadOperation::UNDEFINED) {
				errorCount += 1;

				ENGINE_ERROR("descriptor.depthStencilAttachment.loadOperation must be a valid LoadOperation value.");
			}
		} */

		ENGINE_ASSERT(false, "TODO");
	}

	void validateDescriptor(const SwapchainDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.vSync != true && descriptor.vSync != false) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.vSync must be either true or false.");
		}

		ENGINE_ASSERT(errorCount == 0, "Swapchain descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}
}