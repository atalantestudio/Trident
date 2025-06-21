#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetImageMemoryBarrierCommand final : public OpenGLCommand {
		public:
			explicit OpenGLSetImageMemoryBarrierCommand(GLbitfield barrier) :
				barrier(barrier)
			{}

			void execute() const final override {
				glMemoryBarrier(barrier);
			}

		private:
			const GLbitfield barrier;
	};
}