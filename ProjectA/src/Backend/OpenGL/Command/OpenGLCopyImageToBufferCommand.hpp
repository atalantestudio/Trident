#pragma once

#include "Engine/API/Rendering/BufferImageCopyRegion.hpp"

namespace ProjectA {
	class OpenGLCopyImageToBufferCommand final : public OpenGLCommand {
		public:
			explicit OpenGLCopyImageToBufferCommand(const OpenGLImage* image, const OpenGLBuffer* buffer, const sequence<BufferImageCopyRegion>& copyRegions) :
				image(image),
				buffer(buffer),
				copyRegions(&copyRegions)
			{}

			void execute() const final override {
				glBindBuffer(GL_PIXEL_PACK_BUFFER, buffer->object);

				for (const BufferImageCopyRegion& copyRegion : *copyRegions) {
					// TODO: Move that switch out of the loop. The function to call must be known beforehand (maybe even when creating the image).
					switch (image->target) {
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
									image->object,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageOffsetY,
									copyRegion.imageOffsetZ + copyRegion.imageLayerOffset,
									copyRegion.imageExtent.width,
									copyRegion.imageExtent.height,
									copyRegion.imageExtent.depth * copyRegion.imageLayerCount,
									image->format,
									GL_UNSIGNED_BYTE,
									static_cast<GLsizei>(buffer->size),
									(GLvoid*) copyRegion.bufferOffset
								);
							#else
								ENGINE_ERROR_UNSUPPORTED();
							#endif

							break;
						default:
							ENGINE_ERROR("Unhandled image target ([]).", image->target);
							DEBUG_ASSERT(false);

							break;
					}
				}

				#ifdef OPENGL_UNBIND_AFTER_USE
					glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
				#endif
			}

		private:
			const OpenGLImage* image;
			const OpenGLBuffer* buffer;
			const sequence<BufferImageCopyRegion>* copyRegions;
	};
}