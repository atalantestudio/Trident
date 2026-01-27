// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"

namespace Trident::GL {
	inline GLbitfield getBarrier(ImageLayout imageLayout) {
		ADAPTER_ASSERT(imageLayout != ImageLayout::PRESENT_SRC, "PRESENT_SRC cannot be used with access flags.");

		static constexpr GLbitfield glBarriers[] = {
			0,
			0,
			0,
			0,
			0,
			GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
			0,
			GL_TEXTURE_UPDATE_BARRIER_BIT,
			0,
		};

		return glBarriers[static_cast<uint8>(imageLayout)];
	}
}