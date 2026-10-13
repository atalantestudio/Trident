// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Experimental/GI2D/Renderer.hpp"

namespace Trident {
	void GI2DExperimentalRenderer::updateCameraProjection() const {
		const vec2<float32> size = vec2<float32>(getSwapchain()->getExtent());
		const vec2<float32> O = {0.0f, 0.0f};

		const mat3x3<float32> projection = {
			2.0f / size.x,  0.0f,           0.0f,
			0.0f,           2.0f / size.y,  0.0f,
			O[0],           O[1],           1.0f,
		};

		cameraMappedRange->projection = {{
			{projection[0].x, projection[0].y, projection[0].z, 1.0f},
			{projection[1].x, projection[1].y, projection[1].z, 1.0f},
			{projection[2].x, projection[2].y, projection[2].z, 1.0f},
		}};
	}
}