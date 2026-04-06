// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/FrontFace.hpp"

namespace Trident::GL {
	inline GLenum getFrontFace(FrontFace frontFace) {
		static constexpr GLenum FRONT_FACES[] = {
			GL_CW,
			GL_CCW,
		};

		ADAPTER_ASSERT(frontFace < FrontFace::count, "Invalid front face.");

		return FRONT_FACES[static_cast<uint32>(frontFace)];
	}
}