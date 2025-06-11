#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLCopyBufferToImageCommand : public OpenGLCommand {
		public:
			OpenGLCopyBufferToImageCommand(GLuint buffer, GLuint texture, GLuint target, GLenum format, const BufferImageCopyRegion& copyRegion) :
				buffer(buffer),
				texture(texture),
				target(target),
				format(format),
				copyRegion(&copyRegion)
			{}

			// TODO: Use glCopyTextureSubImage* instead of glTextureSubImage*?
			//       The description for glCopyTextureSubImage* is:
			//       Replace a rectangular portion of a texture image with pixels from the current GL_READ_BUFFER
			//       (rather than from main memory, as is the case for glTexSubImage2D).
			void execute() const override {
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);

				const void* pixels = (const void*) copyRegion->bufferOffset;

				switch (target) {
					case GL_TEXTURE_1D:
						// TODO: Test this path.
						DEBUG_ASSERT(false);

						glTextureSubImage1D(
							texture,
							copyRegion->imageMipLevel,
							copyRegion->imageOffsetX,
							copyRegion->imageExtent.width,
							format,
							GL_UNSIGNED_BYTE,
							pixels
						);

						break;
					case GL_TEXTURE_1D_ARRAY:
					case GL_TEXTURE_2D:
					case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
					case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
					case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
					case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
					case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
					case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
						// TODO: Test this path.
						DEBUG_ASSERT(false);

						glTextureSubImage2D(
							texture,
							copyRegion->imageMipLevel,
							copyRegion->imageOffsetX,
							copyRegion->imageOffsetY,
							copyRegion->imageExtent.width,
							copyRegion->imageExtent.height,
							format,
							GL_UNSIGNED_BYTE,
							pixels
						);

						break;
					case GL_TEXTURE_2D_ARRAY:
					case GL_TEXTURE_3D:
						std::vector<uint8> d(512 * 512 * 4, 150);

						glClearTexSubImage(texture, 0, 0, 0, 0, 512, 512, 1, GL_RGBA, GL_UNSIGNED_BYTE, d.data());
						glTextureSubImage3D(
							texture,
							0,
							0,
							0,
							0,
							512,
							512,
							1,
							GL_RGBA,
							GL_UNSIGNED_BYTE,
							(const void*) 0
						);

						/*glTextureSubImage3D(
							texture,
							copyRegion->imageMipLevel,
							copyRegion->imageOffsetX,
							copyRegion->imageOffsetY,
							copyRegion->imageBaseArrayLayer + copyRegion->imageOffsetZ,
							copyRegion->imageExtent.width,
							copyRegion->imageExtent.height,
							copyRegion->imageExtent.depth,
							format,
							GL_UNSIGNED_BYTE,
							pixels
						);*/

						break;
				}

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			}

		private:
			const GLuint buffer;
			const GLuint texture;
			const GLuint target;
			const GLenum format;
			const BufferImageCopyRegion* copyRegion;
	};
}