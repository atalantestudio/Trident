#pragma once

#include "Engine/Renderer/CommandEncoder.hpp"

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLBeginRenderPassCommand : public OpenGLCommand {
		public:
			OpenGLBeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor);

			void execute() const override;

		private:
			const BeginRenderPassDescriptor descriptor;
	};
}