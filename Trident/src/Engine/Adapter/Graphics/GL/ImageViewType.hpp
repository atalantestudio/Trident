// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/ImageType.hpp"

namespace Trident::GL {
	inline GLenum getImageViewType(GLenum target, uint32 layerCount, uint32 viewLayerCount) {
		if (layerCount > 1 && viewLayerCount == 1) {
			// Demote to a non-array target.
			switch (target) {
				case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: {
					return GL_TEXTURE_2D_MULTISAMPLE;
				}
				case GL_TEXTURE_2D_ARRAY: {
					return GL_TEXTURE_2D;
				}
				default: {
					ADAPTER_ASSERT(false, "Unsupported image type.");

					break;
				}
			}

			return GL_NONE;
		}

		return target;
	}
}