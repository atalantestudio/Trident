#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLSetImageMemoryBarrierCommand.hpp"

namespace ProjectA {
	OpenGLSetImageMemoryBarrierCommand::OpenGLSetImageMemoryBarrierCommand(GLbitfield barrier) :
		barrier(barrier)
	{}

	void OpenGLSetImageMemoryBarrierCommand::execute() const {
		glMemoryBarrier(barrier);
	}
}