#pragma once

#include <glad/glad.h>

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetImageMemoryBarrierCommand : public OpenGLCommand {
		public:
			OpenGLSetImageMemoryBarrierCommand(GLbitfield barrier);

			void execute() const override;

		private:
			GLbitfield barrier;
	};
}