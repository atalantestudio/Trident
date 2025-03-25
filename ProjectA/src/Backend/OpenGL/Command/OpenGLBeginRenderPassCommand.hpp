#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

#include "Engine/API/CommandEncoder.hpp"

namespace ProjectA {
	class OpenGLBeginRenderPassCommand : public OpenGLCommand {
		public:
			OpenGLBeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor);

			void execute() const override;

		private:
			const BeginRenderPassDescriptor descriptor;
	};
}