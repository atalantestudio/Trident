// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageFlags.hpp"
#include "Engine/API/Graphics/ImageViewType.hpp"

namespace Trident::GL {
	inline GLenum getImageViewType(GLenum target, ImageViewType type, ImageFlags flags, uint32 viewLayerCount) {
		if (type == ImageViewType::TYPE_CUBE) {
			if (viewLayerCount > 6) {
				return GL_TEXTURE_CUBE_MAP_ARRAY;
			}

			return GL_TEXTURE_CUBE_MAP;
		}

		if (viewLayerCount == 1) {
			if (target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY) {
				return GL_TEXTURE_2D_MULTISAMPLE;
			}

			if (target == GL_TEXTURE_2D_ARRAY) {
				return GL_TEXTURE_2D;
			}
		}

		return target;
	}
}