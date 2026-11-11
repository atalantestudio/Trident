// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageSampleCount.hpp"
#include "Engine/API/Physics/2D/Hull/HullHeader.hpp"

namespace Trident {
	struct PhysicsRendererBuildHull2D {
		const HullHeader2D* hullHeader;
		uint16 maxInstanceCount;

		// Used internally by the renderer.
		uint16 instanceCount = 0;
	};

	struct PhysicsRendererBuildOptions2D {
		ImageSampleCount sampleCount;
		vec2<float32> cameraPosition;
		float32 zoomLevel;
		sequence<PhysicsRendererBuildHull2D> hulls;
		uint16 maxCircleInstanceCount;
		uint32 maxContactCount;
	};
}