#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"

namespace ProjectA {
	class OpenGLBindRenderPipelineCommand : public OpenGLCommand {
		public:
			OpenGLBindRenderPipelineCommand(const OpenGLRenderPipeline* renderPipeline);

			void execute() const override;

		private:
			const OpenGLRenderPipeline* renderPipeline;
	};
}