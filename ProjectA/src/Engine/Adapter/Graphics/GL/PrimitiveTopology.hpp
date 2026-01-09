// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PrimitiveTopology.hpp"

namespace ProjectA::GL {
	inline GLenum getPrimitiveTopology(PrimitiveTopology primitiveTopology) {
		static constexpr GLenum glPrimitiveTopologies[] = {
			GL_POINTS,
			GL_LINES,
			GL_LINE_STRIP,
			GL_TRIANGLES,
			GL_TRIANGLE_STRIP,
		};

		return glPrimitiveTopologies[static_cast<uint8>(primitiveTopology)];
	}
}