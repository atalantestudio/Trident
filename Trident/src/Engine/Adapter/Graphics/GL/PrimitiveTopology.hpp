// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/PrimitiveTopology.hpp"

namespace Trident::GL {
	inline GLenum getPrimitiveTopology(PrimitiveTopology primitiveTopology) {
		static constexpr GLenum PRIMITIVE_TOPOLOGIES[] = {
			GL_POINTS,
			GL_LINES,
			GL_LINE_STRIP,
			GL_TRIANGLES,
			GL_TRIANGLE_STRIP,
		};

		return PRIMITIVE_TOPOLOGIES[static_cast<uint32>(primitiveTopology)];
	}
}