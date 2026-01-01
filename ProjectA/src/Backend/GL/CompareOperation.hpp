#pragma once

#include "Engine/API/Rendering/CompareOperation.hpp"

namespace ProjectA {
	static GLenum getGlDepthFunc(CompareOperation compareOperation) {
		return GL_NEVER + static_cast<GLenum>(compareOperation);
	}
}