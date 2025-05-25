#pragma once

#include "Engine/API/Rendering/Descriptor/BeginRenderPassDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/RenderPipelineDescriptor.hpp"

// API descriptor validators for debug mode.
namespace ProjectA {
	static void validateDescriptor(const RenderPipelineDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.renderPass == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.renderPass must be a reference to a valid render pass.");
		}

		if (descriptor.pipelineLayout == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.pipelineLayout must be a reference to a valid pipeline layout.");
		}

		if (descriptor.vertexShaderModule == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.vertexShaderModule must be a reference to a valid vertex shader module.");
		}

		if (descriptor.fragmentShaderModule == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.fragmentShaderModule must be a reference to a valid fragment shader module.");
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

		if (descriptor.colorBlendState.attachments.size() != descriptor.renderPass->getColorAttachmentCount()) {
			errorCount += 1;

			ENGINE_ERROR("The number of color blend attachments in descriptor.colorBlendState.attachments must be equal to the color attachment count of descriptor.renderPass.");
		}

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

		ENGINE_ASSERT(errorCount == 0, "Render pipeline descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}

	static void validateDescriptor(const BeginRenderPassDescriptor& descriptor) {
		uint32 warningCount = 0;
		uint32 errorCount = 0;

		if (descriptor.renderPass == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.renderPass must be a reference to a valid render pass.");
		}

		if (descriptor.swapchain == nullptr) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.swapchain must be a reference to a valid swapchain.");
		}

		if (descriptor.clearValues.size() < descriptor.renderPass->getClearValueCount()) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.clearValues must have a number of clear values greater than or equal to the last attachment index in descriptor.renderPass having a load operation of LoadOperation::CLEAR.");
		}

		if (descriptor.imageIndex >= descriptor.swapchain->getImageCount()) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.imageIndex must be less than the number of swapchain images.");
		}

		ENGINE_ASSERT(errorCount == 0, "Render pass begin descriptor is invalid ([] warnings and [] errors).", warningCount, errorCount);
	}
}