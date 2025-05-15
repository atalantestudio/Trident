#pragma once

namespace ProjectA {
	static GLenum getGlImageType(ImageType imageType) {
		static constexpr GLenum glImageTypes[] = {
			GL_TEXTURE_2D,
		};

		return glImageTypes[static_cast<uint8>(imageType)];
	}
}