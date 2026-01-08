#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/ComputePipeline.hpp"

namespace ProjectA {
	class OpenGLBindComputePipelineCommand final : public OpenGLCommand {
		public:
			explicit OpenGLBindComputePipelineCommand(ref<OpenGLComputePipeline> computePipeline) :
				program(computePipeline->program)
			{}

			void execute() const final override {
				glUseProgram(program);
			}

		private:
			const GLuint program;
	};
}