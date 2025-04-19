#pragma once

#include "Engine/Renderer/PrimitiveTopology.hpp"

namespace ProjectA {
	static GLenum getGlPrimitiveTopology(PrimitiveTopology primitiveTopology) {
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