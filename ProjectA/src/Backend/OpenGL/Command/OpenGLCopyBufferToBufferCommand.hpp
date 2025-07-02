#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

#include "Engine/API/Rendering/BufferCopyRegion.hpp"

namespace ProjectA {
	class OpenGLCopyBufferToBufferCommand final : public OpenGLCommand {
		public:
			explicit OpenGLCopyBufferToBufferCommand(GLuint sourceBuffer, GLuint destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) :
				sourceBuffer(sourceBuffer),
				destinationBuffer(destinationBuffer),
				copyRegions(&copyRegions)
			{}

			void execute() const final override {
				#ifndef OPENGL_USE_DSA
					glBindBuffer(GL_COPY_READ_BUFFER, sourceBuffer);
					glBindBuffer(GL_COPY_WRITE_BUFFER, destinationBuffer);
				#endif

				for (const BufferCopyRegion& copyRegion : *copyRegions) {
					#ifdef OPENGL_USE_DSA
						glCopyNamedBufferSubData(sourceBuffer, destinationBuffer, copyRegion.sourceOffset, copyRegion.destinationOffset, copyRegion.size);
					#else
						glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, copyRegion.sourceOffset, copyRegion.destinationOffset, copyRegion.size);
					#endif
				}

				#ifndef OPENGL_USE_DSA
					glBindBuffer(GL_COPY_READ_BUFFER, 0);
					glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
				#endif
			}

		private:
			const GLuint sourceBuffer;
			const GLuint destinationBuffer;
			const sequence<BufferCopyRegion>* copyRegions;
	};
}