// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageType.hpp"

namespace Trident::GL {
	inline GLenum getImageType(ImageType imageType, uint8 sampleCount, uint32 layerCount) {
		if (imageType == ImageType::TYPE_2D) {
			if (sampleCount > 1) {
				return GL_TEXTURE_2D_MULTISAMPLE;

				if (layerCount > 1) {
					// TODO: Implement multisampled 2D texture arrays.
					ENGINE_ERROR_UNIMPLEMENTED();

					return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
				}
			}

			if (layerCount > 1) {
				return GL_TEXTURE_2D_ARRAY;
			}

			return GL_TEXTURE_2D;
		}

		ENGINE_ERROR_UNSUPPORTED();

		return 0;
	}
}