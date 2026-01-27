// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CompareOperation.hpp"

namespace Trident::GL {
	inline GLenum getDepthFunc(CompareOperation compareOperation) {
		return GL_NEVER + static_cast<GLenum>(compareOperation);
	}
}