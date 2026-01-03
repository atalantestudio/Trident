#pragma once

#include "Engine/API/Rendering/CullMode.hpp"

namespace ProjectA {
	static GLenum getGlCullMode(CullMode cullMode) {
		static constexpr GLenum glCullModes[] = {
			0,
			GL_FRONT,
			GL_BACK,
			GL_FRONT_AND_BACK,
		};

		return glCullModes[static_cast<uint8>(cullMode)];
	}
}