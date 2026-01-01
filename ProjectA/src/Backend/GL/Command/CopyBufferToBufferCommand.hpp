#pragma once

#include "Backend/GL/Command/Command.hpp"
#include "Backend/GL/Buffer.hpp"

#include "Engine/API/Rendering/BufferCopyRegion.hpp"

namespace ProjectA {
	class OpenGLCopyBufferToBufferCommand final : public OpenGLCommand {
		public:
			explicit OpenGLCopyBufferToBufferCommand(GLuint sourceBuffer, GLuint destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) :
				sourceBuffer(sourceBuffer),
				destinationBuffer(destinationBuffer),
				copyRegions(copyRegions)
			{}

			void execute() const final override {
				#if OPENGL_VERSION < OPENGL_VERSION_4_5
					glBindBuffer(GL_COPY_READ_BUFFER, sourceBuffer);
					glBindBuffer(GL_COPY_WRITE_BUFFER, destinationBuffer);
				#endif

				for (const BufferCopyRegion& copyRegion : copyRegions) {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glCopyNamedBufferSubData(sourceBuffer, destinationBuffer, copyRegion.sourceOffset, copyRegion.destinationOffset, copyRegion.size);
					#elif OPENGL_VERSION >= OPENGL_VERSION_3_1
						glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, copyRegion.sourceOffset, copyRegion.destinationOffset, copyRegion.size);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}

				#if OPENGL_VERSION < OPENGL_VERSION_4_5 && defined(OPENGL_UNBIND_AFTER_USE)
					glBindBuffer(GL_COPY_READ_BUFFER, 0);
					glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
				#endif
			}

		private:
			const GLuint sourceBuffer;
			const GLuint destinationBuffer;
			const sequence<BufferCopyRegion> copyRegions;
	};
}