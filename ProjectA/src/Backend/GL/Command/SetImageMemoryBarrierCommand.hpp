#pragma once

#include "Backend/GL/Command/Command.hpp"

namespace ProjectA {
	class OpenGLSetImageMemoryBarrierCommand final : public OpenGLCommand {
		public:
			explicit OpenGLSetImageMemoryBarrierCommand(GLbitfield barrier) :
				barrier(barrier)
			{}

			void execute() const final override {
				#if OPENGL_VERSION >= OPENGL_VERSION_4_2
					glMemoryBarrier(barrier);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			const GLbitfield barrier;
	};
}