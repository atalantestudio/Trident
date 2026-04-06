// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/CullMode.hpp"
#include "Engine/API/Graphics/PolygonMode.hpp"

namespace Trident::GL {
	inline GLenum getPolygonMode(PolygonMode polygonMode) {
		static constexpr GLenum POLYGON_MODES[] = {
			GL_POINT,
			GL_LINE,
			GL_FILL,
		};

		return POLYGON_MODES[static_cast<uint32>(polygonMode)];
	}
}