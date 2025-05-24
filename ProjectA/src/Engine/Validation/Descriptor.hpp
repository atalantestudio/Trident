#pragma once

#include "Engine/API/Rendering/Descriptor/RenderPipelineDescriptor.hpp"

// API descriptor validators for debug mode.
namespace ProjectA {
	static void validateDescriptor(const RenderPipelineDescriptor& descriptor) {
		uint32 errorCount = 0;
		uint32 warningCount = 0;

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

		if (descriptor.lineWidth <= 0.0f) {
			warningCount += 1;

			ENGINE_WARN("descriptor.lineWidth should be > 0.");
		}

		if (descriptor.minSampleShading < 0.0f || descriptor.minSampleShading > 1.0f) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.minSampleShading must be in the range [0, 1].");
		}

		if (descriptor.depthStencilState.depthTest != true && descriptor.depthStencilState.depthTest != false) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.depthStencilState.depthTest must be either true or false.");
		}

		if (descriptor.depthStencilState.depthWrite != true && descriptor.depthStencilState.depthWrite != false) {
			errorCount += 1;

			ENGINE_ERROR("descriptor.depthStencilState.depthWrite must be either true or false.");
		}

		if (!(descriptor.dynamicState & DynamicState::SCISSOR)) {
			warningCount += 1;

			ENGINE_WARN("descriptor.dynamicState should include DynamicState::SCISSOR.");
		}

		if (!(descriptor.dynamicState & DynamicState::VIEWPORT)) {
			warningCount += 1;

			ENGINE_WARN("descriptor.dynamicState should include DynamicState::VIEWPORT.");
		}

		ENGINE_ASSERT(errorCount == 0, "Render pipeline descriptor is invalid ([] errors and [] warnings).", errorCount, warningCount);
	}
}