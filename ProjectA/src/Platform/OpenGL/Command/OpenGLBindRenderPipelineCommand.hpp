#pragma once

#include "Platform/OpenGL/Command/OpenGLCommand.hpp"
#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"

namespace ProjectA {
	class OpenGLBindRenderPipelineCommand : public OpenGLCommand {
		public:
			OpenGLBindRenderPipelineCommand(const OpenGLRenderPipeline* renderPipeline);

			void execute() const override;

		private:
			const OpenGLRenderPipeline* renderPipeline;
	};
}