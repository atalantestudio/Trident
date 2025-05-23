#pragma once

namespace ProjectA {
	static GLenum getGlDepthFunc(CompareOperation compareOperation) {
		return GL_NEVER + static_cast<GLenum>(compareOperation);
	}
}