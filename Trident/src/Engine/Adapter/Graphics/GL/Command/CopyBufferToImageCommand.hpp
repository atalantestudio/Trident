// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"
#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/ImageFormat.hpp"
#include "Engine/API/Graphics/BufferImageCopyRegion.hpp"

namespace Trident {
	class GL::CopyBufferToImageCommand final : public GL::Command {
		public:
			explicit CopyBufferToImageCommand(ref<GL::Buffer> sourceBuffer, ref<GL::Image> destinationImage, view<BufferImageCopyRegion> copyRegions) :
				sourceBuffer(sourceBuffer),
				destinationImage(destinationImage),
				copyRegions(copyRegions.count())
			{
				std::copy(copyRegions.begin(), copyRegions.end(), &this->copyRegions[0]);
			}

			void execute() const final override {
				const GLenum type = getType(destinationImage->getFormat());

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, sourceBuffer->buffer);

				#if OPENGL_VERSION < OPENGL_VERSION_4_5
					glBindTexture(destinationImage->target, destinationImage->texture);
				#endif

				switch (destinationImage->format) {
					case GL_RGB: {
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

						break;
					}
					case GL_RGBA: {
						glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

						break;
					}
				}

				for (const BufferImageCopyRegion& copyRegion : copyRegions) {
					const GLvoid* pixels = (const GLvoid*) copyRegion.bufferOffset;

					// TODO(graphics/gl/logs) (tested on HP Victus)
					// The buffer image copy may generate the following warning:
					// "Pixel-path performance warning: Pixel transfer is synchronized with 3D rendering."
					// See https://stackoverflow.com/a/49369531.

					// TODO(graphics/gl/texture): Move that switch out of the loop. The function to call should be known beforehand (maybe even before creating the image).
					switch (destinationImage->target) {
						case GL_TEXTURE_1D: {
							#if OPENGL_VERSION >= OPENGL_VERSION_4_5
								glTextureSubImage1D(
									destinationImage->texture,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageExtent.width,
									destinationImage->format,
									type,
									pixels
								);
							#else
								glTexSubImage1D(
									destinationImage->target,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageExtent.width,
									destinationImage->format,
									type,
									pixels
								);
							#endif

							break;
						}
						case GL_TEXTURE_1D_ARRAY:
						case GL_TEXTURE_2D:
						case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
						case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
						case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
						case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
						case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
						case GL_TEXTURE_CUBE_MAP_POSITIVE_Z: {
							#if OPENGL_VERSION >= OPENGL_VERSION_4_5
								glTextureSubImage2D(
									destinationImage->texture,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageOffsetY + copyRegion.imageLayerOffset,
									copyRegion.imageExtent.width,
									copyRegion.imageExtent.height * copyRegion.imageLayerCount,
									destinationImage->format,
									type,
									pixels
								);
							#else
								glTexSubImage2D(
									destinationImage->target,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageOffsetY + copyRegion.imageLayerOffset,
									copyRegion.imageExtent.width,
									copyRegion.imageExtent.height * copyRegion.imageLayerCount,
									destinationImage->format,
									type,
									pixels
								);
							#endif

							break;
						}
						case GL_TEXTURE_2D_ARRAY:
						case GL_TEXTURE_3D: {
							#if OPENGL_VERSION >= OPENGL_VERSION_4_5
								glTextureSubImage3D(
									destinationImage->texture,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageOffsetY,
									copyRegion.imageOffsetZ + copyRegion.imageLayerOffset,
									copyRegion.imageExtent.width,
									copyRegion.imageExtent.height,
									copyRegion.imageExtent.depth * copyRegion.imageLayerCount,
									destinationImage->format,
									type,
									pixels
								);
							#else
								glTexSubImage3D(
									destinationImage->target,
									copyRegion.imageMipLevel,
									copyRegion.imageOffsetX,
									copyRegion.imageOffsetY,
									copyRegion.imageOffsetZ + copyRegion.imageLayerOffset,
									copyRegion.imageExtent.width,
									copyRegion.imageExtent.height,
									copyRegion.imageExtent.depth * copyRegion.imageLayerCount,
									destinationImage->format,
									type,
									pixels
								);
							#endif

							break;
						}
						default: {
							ADAPTER_ASSERT(false, "Unhandled image target ([]).", destinationImage->target);

							break;
						}
					}
				}
			}

		private:
			ref<GL::Buffer> sourceBuffer;
			ref<GL::Image> destinationImage;
			sequence<BufferImageCopyRegion> copyRegions;
	};
}