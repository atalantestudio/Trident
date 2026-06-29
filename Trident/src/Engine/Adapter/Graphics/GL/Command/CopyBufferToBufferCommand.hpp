// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"
#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/API/Graphics/BufferCopyRegion.hpp"

namespace Trident {
	class GL::CopyBufferToBufferCommand final : public GL::Command {
		public:
			explicit CopyBufferToBufferCommand(ref<GL::BaseBuffer> sourceBuffer, ref<GL::BaseBuffer> destinationBuffer, view<BufferCopyRegion> copyRegions) :
				sourceBuffer(sourceBuffer),
				destinationBuffer(destinationBuffer),
				copyRegions(copyRegions.count())
			{
				std::copy(copyRegions.begin(), copyRegions.end(), &this->copyRegions[0]);
			}

			void execute() const final override {
				glBindBuffer(GL_COPY_READ_BUFFER, sourceBuffer->buffer);
				glBindBuffer(GL_COPY_WRITE_BUFFER, destinationBuffer->buffer);

				for (const BufferCopyRegion& copyRegion : copyRegions) {
					#if OPENGL_VERSION >= OPENGL_VERSION_3_1
						glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, copyRegion.sourceOffset, copyRegion.destinationOffset, copyRegion.size);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}
			}

		private:
			ref<GL::BaseBuffer> sourceBuffer;
			ref<GL::BaseBuffer> destinationBuffer;
			sequence<BufferCopyRegion> copyRegions;
	};
}