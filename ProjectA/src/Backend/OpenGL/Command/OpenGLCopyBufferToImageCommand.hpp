#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLCopyBufferToImageCommand : public OpenGLCommand {
		public:
			OpenGLCopyBufferToImageCommand(GLuint buffer, const OpenGLImage* image, const sequence<BufferImageCopyRegion>& copyRegions) :
				buffer(buffer),
				image(image),
				copyRegions(&copyRegions)
			{}

			// TODO: Use glCopyTextureSubImage* instead of glTextureSubImage*?
			//       The description for glCopyTextureSubImage* is:
			//       Replace a rectangular portion of a texture image with pixels from the current GL_READ_BUFFER
			//       (rather than from main memory, as is the case for glTexSubImage2D).
			void execute() const final override {
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);

				#ifndef OPENGL_USE_DSA
					glBindTexture(image->target, image->object);
				#endif

				switch (image->format) {
					case GL_RGB:
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

						break;
					case GL_RGBA:
						glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

						break;
					default:
						ENGINE_WARN("Image target is not RGB or RGBA. Unpack alignment unchanged.");

						break;
				}

				for (const BufferImageCopyRegion& copyRegion : *copyRegions) {
					const void* pixels = (const void*) copyRegion.bufferOffset;

					// TODO: Move that switch out of the loop. The function to call must be known beforehand (maybe even when creating the image).
					switch (image->target) {
						case GL_TEXTURE_1D:
							// TODO: Test this path.
							DEBUG_ASSERT(false);

							#ifdef OPENGL_USE_DSA
								glTextureSubImage1D(
									image->object,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageExtent.width,
									image->format,
									GL_UNSIGNED_BYTE,
									pixels
								);
							#else
								glTexSubImage1D(
									image->target,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageExtent.width,
									image->format,
									GL_UNSIGNED_BYTE,
									pixels
								);
							#endif

							break;
						case GL_TEXTURE_1D_ARRAY:
						case GL_TEXTURE_2D:
						case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
						case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
						case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
						case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
						case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
						case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
							#ifdef OPENGL_USE_DSA
								glTextureSubImage2D(
									image->object,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageOffsetY + copyRegion.imageLayerOffset,
									copyRegion.imageExtent.width,
									copyRegion.imageExtent.height * copyRegion.imageLayerCount,
									image->format,
									GL_UNSIGNED_BYTE,
									pixels
								);
							#else
								glTexSubImage2D(
									image->target,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageOffsetY + copyRegion.imageLayerOffset,
									copyRegion.imageExtent.width,
									copyRegion.imageExtent.height * copyRegion.imageLayerCount,
									image->format,
									GL_UNSIGNED_BYTE,
									pixels
								);
							#endif

							break;
						case GL_TEXTURE_2D_ARRAY:
						case GL_TEXTURE_3D:
							#ifdef OPENGL_USE_DSA
								glTextureSubImage3D(
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
									pixels
								);
							#else
								glTexSubImage3D(
									image->target,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageOffsetY,
									copyRegion.imageOffsetZ + copyRegion.imageLayerOffset,
									copyRegion.imageExtent.width,
									copyRegion.imageExtent.height,
									copyRegion.imageExtent.depth * copyRegion.imageLayerCount,
									image->format,
									GL_UNSIGNED_BYTE,
									pixels
								);
							#endif

							break;
						default:
							ENGINE_ERROR("Unhandled image target ([]).", image->target);
							DEBUG_ASSERT(false);

							break;
					}
				}

				#ifndef OPENGL_USE_DSA
					glBindTexture(image->target, 0);
				#endif

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			}

		private:
			const GLuint buffer;
			const OpenGLImage* image;
			const sequence<BufferImageCopyRegion>* copyRegions;
	};
}