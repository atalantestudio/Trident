// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CullMode.hpp"

namespace ProjectA::GL {
	inline GLenum getCullMode(CullMode cullMode) {
		static constexpr GLenum glCullModes[] = {
			0,
			GL_FRONT,
			GL_BACK,
			GL_FRONT_AND_BACK,
		};

		return glCullModes[static_cast<uint8>(cullMode)];
	}

	// Returns the visible face using the cull mode value.
	inline GLenum getPolygonModeFace(CullMode cullMode) {
		static constexpr GLenum glFaces[] = {
			GL_FRONT_AND_BACK,
			GL_BACK,
			GL_FRONT,
			GL_NONE,
		};

		return glFaces[static_cast<uint8>(cullMode)];
	}
}