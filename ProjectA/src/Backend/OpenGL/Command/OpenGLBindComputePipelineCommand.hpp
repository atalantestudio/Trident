#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLComputePipeline.hpp"

namespace ProjectA {
	class OpenGLBindComputePipelineCommand : public OpenGLCommand {
		public:
			OpenGLBindComputePipelineCommand(const ref<OpenGLComputePipeline>& computePipeline) :
				program(computePipeline->program)
			{}

			void execute() const final override {
				glUseProgram(program);
			}

		private:
			const GLuint program;
	};
}