// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/Buffer.hpp"

#include "Engine/API/Rendering/BufferCopyRegion.hpp"

namespace ProjectA {
	class GL::CopyBufferToBufferCommand final : public GL::Command {
		public:
			explicit CopyBufferToBufferCommand(ref<GL::Buffer> sourceBuffer, ref<GL::Buffer> destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) :
				sourceBuffer(sourceBuffer),
				destinationBuffer(destinationBuffer),
				copyRegions(copyRegions)
			{}

			void execute() const final override {
				#if OPENGL_VERSION < OPENGL_VERSION_4_5
					glBindBuffer(GL_COPY_READ_BUFFER, sourceBuffer->buffer);
					glBindBuffer(GL_COPY_WRITE_BUFFER, destinationBuffer->buffer);
				#endif

				for (const BufferCopyRegion& copyRegion : copyRegions) {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glCopyNamedBufferSubData(sourceBuffer->buffer, destinationBuffer->buffer, copyRegion.sourceOffset, copyRegion.destinationOffset, copyRegion.size);
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
			ref<GL::Buffer> sourceBuffer;
			ref<GL::Buffer> destinationBuffer;
			sequence<BufferCopyRegion> copyRegions;
	};
}