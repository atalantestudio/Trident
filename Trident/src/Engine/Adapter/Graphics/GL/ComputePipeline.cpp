// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/GL/ShaderModule.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::ComputePipeline::ComputePipeline(const Trident::Device* device) :
		Trident::ComputePipeline(device)
	{}

	ref<Trident::ComputePipeline> GL::createComputePipeline(const Trident::Device* device, const ComputePipelineDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::ComputePipeline> computePipeline = allocateRef<GL::ComputePipeline>(device);
		computePipeline->program = glCreateProgram();

		const ref<GL::ShaderModule> computeShaderModule = static_pointer_cast<GL::ShaderModule>(descriptor.computeShaderModule);

		glAttachShader(computePipeline->program, computeShaderModule->shader);
		glLinkProgram(computePipeline->program);

		GLint linkStatus;

		glGetProgramiv(computePipeline->program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus != GL_TRUE) {
			GLint infoLogLength;

			glGetProgramiv(computePipeline->program, GL_INFO_LOG_LENGTH, &infoLogLength);

			sequence<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(computePipeline->program, infoLogLength, nullptr, &infoLog[0]);

			ADAPTER_ASSERT(false, "Program linking failed:\n[]", infoLog);

			return nullptr;
		}

		glDetachShader(computePipeline->program, computeShaderModule->shader);

		#if defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				glObjectLabel(GL_PROGRAM, computePipeline->program, static_cast<GLsizei>(descriptor.name.count()), &descriptor.name[0]);
			}
		#endif

		return computePipeline;
	}

	void GL::destroyComputePipeline(const Trident::Device* device, const Trident::ComputePipeline* computePipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::ComputePipeline* _computePipeline = static_cast<const GL::ComputePipeline*>(computePipeline);

		glDeleteProgram(_computePipeline->program);
	}
}