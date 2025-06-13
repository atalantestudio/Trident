#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetImageMemoryBarrierCommand : public OpenGLCommand {
		public:
			OpenGLSetImageMemoryBarrierCommand(GLbitfield barrier) :
				barrier(barrier)
			{}

			void execute() const final override {
				glMemoryBarrier(barrier);
			}

		private:
			const GLbitfield barrier;
	};
}