#pragma once

#include "Backend/GL/Command/Command.hpp"
#include "Backend/GL/Buffer.hpp"

#include "Engine/API/Rendering/BufferImageCopyRegion.hpp"

namespace ProjectA {
	class OpenGLCopyBufferToImageCommand final : public OpenGLCommand {
		public:
			explicit OpenGLCopyBufferToImageCommand(GLuint buffer, const OpenGLImage* image, const sequence<BufferImageCopyRegion>& copyRegions) :
				buffer(buffer),
				image(image),
				copyRegions(copyRegions)
			{}

			void execute() const final override {
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);

				#if OPENGL_VERSION < OPENGL_VERSION_4_5
					glBindTexture(image->target, image->object);
				#endif

				switch (image->format) {
					case GL_RGB:
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

						break;
					case GL_RGBA:
						glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

						break;
				}

				for (const BufferImageCopyRegion& copyRegion : copyRegions) {
					const GLvoid* pixels = (const GLvoid*) copyRegion.bufferOffset;

					// TODO: Move that switch out of the loop. The function to call must be known beforehand (maybe even when creating the image).
					switch (image->target) {
						case GL_TEXTURE_1D:
							// TODO: Implement 1D textures.
							DEBUG_ASSERT(false);

							#if OPENGL_VERSION >= OPENGL_VERSION_4_5
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
						case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
						case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
						case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
						case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
						case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
						case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
							#if OPENGL_VERSION >= OPENGL_VERSION_4_5
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
							#if OPENGL_VERSION >= OPENGL_VERSION_4_5
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

				#ifdef OPENGL_UNBIND_AFTER_USE
					glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

					#if OPENGL_VERSION < OPENGL_VERSION_4_5
						glBindTexture(image->target, 0);
					#endif
				#endif
			}

		private:
			const GLuint buffer;
			const OpenGLImage* image;
			const sequence<BufferImageCopyRegion> copyRegions;
	};
}