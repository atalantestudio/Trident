// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/CullMode.hpp"

namespace Trident::GL {
	inline GLenum getCullMode(CullMode cullMode) {
		static constexpr GLenum CULL_MODES[] = {
			0,
			GL_FRONT,
			GL_BACK,
		};

		ADAPTER_ASSERT(cullMode < CullMode::count, "Invalid cull mode.");

		return CULL_MODES[static_cast<uint8>(cullMode)];
	}

	// Returns the visible face using the cull mode value.
	inline GLenum getPolygonModeFace(CullMode cullMode) {
		static constexpr GLenum FACES[] = {
			GL_FRONT_AND_BACK,
			GL_BACK,
			GL_FRONT,
		};

		ADAPTER_ASSERT(cullMode < CullMode::count, "Invalid cull mode.");

		return FACES[static_cast<uint8>(cullMode)];
	}
}