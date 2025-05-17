#pragma once

namespace ProjectA {
	static GLenum getGlImageType(ImageType imageType, uint8 sampleCount) {
		if (imageType == ImageType::TEXTURE_2D) {
			if (sampleCount > 1) {
				return GL_TEXTURE_2D_MULTISAMPLE;
			}

			return GL_TEXTURE_2D;
		}

		DEBUG_ASSERT(false);

		return 0;
	}
}