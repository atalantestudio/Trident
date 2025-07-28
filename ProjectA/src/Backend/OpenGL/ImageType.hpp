#pragma once

namespace ProjectA {
	static GLenum getGlImageType(ImageType imageType, uint8 sampleCount, uint32 layerCount) {
		if (imageType == ImageType::TEXTURE_2D) {
			if (sampleCount > 1) {
				return GL_TEXTURE_2D_MULTISAMPLE;

				if (layerCount > 1) {
					// TODO: Test this path.
					ENGINE_ASSERT(false, "Multisampled 2D texture arrays are not implemented.");

					return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
				}
			}

			if (layerCount > 1) {
				return GL_TEXTURE_2D_ARRAY;
			}

			return GL_TEXTURE_2D;
		}

		DEBUG_ASSERT(false);

		return 0;
	}
}