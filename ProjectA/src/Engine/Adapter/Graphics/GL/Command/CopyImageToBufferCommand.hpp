// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/BufferImageCopyRegion.hpp"

namespace ProjectA {
	class GL::CopyImageToBufferCommand final : public GL::Command {
		public:
			explicit CopyImageToBufferCommand(ref<GL::Image> sourceImage, ref<GL::Buffer> destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) :
				sourceImage(sourceImage),
				destinationBuffer(destinationBuffer),
				copyRegions(copyRegions)
			{}

			void execute() const final override {
				glBindBuffer(GL_PIXEL_PACK_BUFFER, destinationBuffer->buffer);

				for (const BufferImageCopyRegion& copyRegion : copyRegions) {
					// TODO: Move that switch out of the loop. The function to call must be known beforehand (maybe even when creating the image).
					switch (sourceImage->target) {
						case GL_TEXTURE_1D:
						case GL_TEXTURE_1D_ARRAY:
						case GL_TEXTURE_2D:
						case GL_TEXTURE_2D_ARRAY:
						case GL_TEXTURE_3D:
						case GL_TEXTURE_CUBE_MAP:
						case GL_TEXTURE_CUBE_MAP_ARRAY:
						case GL_TEXTURE_RECTANGLE:
							#if OPENGL_VERSION >= OPENGL_VERSION_4_5
								glGetTextureSubImage(
									sourceImage->texture,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageOffsetY,
									copyRegion.imageOffsetZ + copyRegion.imageLayerOffset,
									copyRegion.imageExtent.width,
									copyRegion.imageExtent.height,
									copyRegion.imageExtent.depth * copyRegion.imageLayerCount,
									sourceImage->format,
									GL_UNSIGNED_BYTE,
									static_cast<GLsizei>(destinationBuffer->size),
									(GLvoid*) copyRegion.bufferOffset
								);
							#else
								ENGINE_ERROR_UNSUPPORTED();
							#endif

							break;
						default:
							ENGINE_ERROR("Unhandled image target ([]).", sourceImage->target);
							DEBUG_ASSERT(false);

							break;
					}
				}

				#ifdef OPENGL_UNBIND_AFTER_USE
					glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
				#endif
			}

		private:
			ref<GL::Image> sourceImage;
			ref<GL::Buffer> destinationBuffer;
			sequence<BufferImageCopyRegion> copyRegions;
	};
}