#pragma once

#include "Engine/API/Rendering/CullMode.hpp"
#include "Engine/API/Rendering/PolygonMode.hpp"

namespace ProjectA {
	static GLenum getGlPolygonMode(PolygonMode polygonMode) {
		static constexpr GLenum glPolygonModes[] = {
			GL_POINT,
			GL_LINE,
			GL_FILL,
		};

		return glPolygonModes[static_cast<uint8>(polygonMode)];
	}

	// Returns the visible face using the cull mode value.
	static GLenum getGlPolygonModeFace(CullMode cullMode) {
		static constexpr GLenum glFaces[] = {
			GL_FRONT_AND_BACK,
			GL_BACK,
			GL_FRONT,
			GL_NONE,
		};

		return glFaces[static_cast<uint8>(cullMode)];
	}
}