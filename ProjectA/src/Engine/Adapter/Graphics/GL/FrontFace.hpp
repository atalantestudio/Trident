// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/FrontFace.hpp"

namespace ProjectA::GL {
	inline GLenum getFrontFace(FrontFace frontFace) {
		static constexpr GLenum glFrontFaces[] = {
			GL_CW,
			GL_CCW,
		};

		return glFrontFaces[static_cast<uint8>(frontFace)];
	}
}