#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLCopyBufferToImageCommand : public OpenGLCommand {
		public:
			OpenGLCopyBufferToImageCommand(GLuint buffer, GLuint texture, GLenum format, const BufferImageCopyRegion& copyRegion) :
				buffer(buffer),
				texture(texture),
				format(format),
				copyRegion(&copyRegion)
			{}

			// TODO: Use glCopyTextureSubImage* instead of glTextureSubImage*?
			//       The description for glCopyTextureSubImage* is:
			//       Replace a rectangular portion of a texture image with pixels from the current GL_READ_BUFFER
			//       (rather than from main memory, as is the case for glTexSubImage2D).
			void execute() const override {
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);

				constexpr GLenum type = GL_UNSIGNED_BYTE;
				const void* pixels = (const void*) copyRegion->bufferOffset;

				switch (texture) {
					case GL_TEXTURE_1D:
						glTextureSubImage1D(
							texture,
							copyRegion->imageBaseArrayLayer,
							copyRegion->imageOffsetX,
							copyRegion->imageExtent.width,
							format,
							type,
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
						glTextureSubImage2D(
							texture,
							copyRegion->imageBaseArrayLayer,
							copyRegion->imageOffsetX,
							copyRegion->imageOffsetY,
							copyRegion->imageExtent.width,
							copyRegion->imageExtent.height,
							format,
							type,
							pixels
						);

						break;
					case GL_TEXTURE_2D_ARRAY:
					case GL_TEXTURE_3D:
						glTextureSubImage3D(
							texture,
							copyRegion->imageBaseArrayLayer,
							copyRegion->imageOffsetX,
							copyRegion->imageOffsetY,
							copyRegion->imageOffsetZ,
							copyRegion->imageExtent.width,
							copyRegion->imageExtent.height,
							copyRegion->imageExtent.depth,
							format,
							type,
							pixels
						);

						break;
				}

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			}

		private:
			const GLuint buffer;
			const GLuint texture;
			const GLenum format;
			const BufferImageCopyRegion* copyRegion;
	};
}