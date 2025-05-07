#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLComputePipeline.hpp"

namespace ProjectA {
	class OpenGLBindComputePipelineCommand : public OpenGLCommand {
		public:
			OpenGLBindComputePipelineCommand(const ref<OpenGLComputePipeline>& computePipeline);

			void execute() const override;

		private:
			const GLuint program;
	};
}