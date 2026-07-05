// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/ImageFormat.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/BufferImageCopyRegion.hpp"

namespace Trident {
	class GL::CopyImageToBufferCommand final : public GL::Command {
		public:
			explicit CopyImageToBufferCommand(ref<GL::Image> sourceImage, ref<GL::Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) :
				sourceImage(sourceImage),
				destinationBuffer(destinationBuffer),
				copyRegions(copyRegions.count())
			{
				std::copy(copyRegions.begin(), copyRegions.end(), &this->copyRegions[0]);
			}

			void execute() const final override {
				const GLenum type = getType(sourceImage->getFormat());

				glBindBuffer(GL_PIXEL_PACK_BUFFER, destinationBuffer->buffer);

				for (const BufferImageCopyRegion& copyRegion : copyRegions) {
					// TODO(graphics/gl/texture): Move that switch out of the loop. The function to call should be known beforehand (maybe even before creating the image).
					switch (sourceImage->target) {
						case GL_TEXTURE_1D:
						case GL_TEXTURE_1D_ARRAY:
						case GL_TEXTURE_2D:
						case GL_TEXTURE_2D_ARRAY:
						case GL_TEXTURE_3D:
						case GL_TEXTURE_CUBE_MAP:
						case GL_TEXTURE_CUBE_MAP_ARRAY:
						case GL_TEXTURE_RECTANGLE: {
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
									type,
									static_cast<GLsizei>(destinationBuffer->getSize()),
									(GLvoid*) copyRegion.bufferOffset
								);
							#else
								ENGINE_ERROR_UNSUPPORTED();
							#endif

							break;
						}
						default: {
							ADAPTER_ASSERT(false, "Unhandled image target ([]).", sourceImage->target);

							break;
						}
					}
				}
			}

		private:
			ref<GL::Image> sourceImage;
			ref<GL::Buffer> destinationBuffer;
			sequence<BufferImageCopyRegion> copyRegions;
	};
}