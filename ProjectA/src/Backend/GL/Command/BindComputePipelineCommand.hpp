#pragma once

#include "Backend/GL/Command/Command.hpp"
#include "Backend/GL/ComputePipeline.hpp"

namespace ProjectA {
	class OpenGLBindComputePipelineCommand final : public OpenGLCommand {
		public:
			explicit OpenGLBindComputePipelineCommand(const ref<OpenGLComputePipeline>& computePipeline) :
				program(computePipeline->program)
			{}

			void execute() const final override {
				glUseProgram(program);
			}

		private:
			const GLuint program;
	};
}