// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageSampleCount.hpp"
#include "Engine/API/Physics/2D/Hull/HullHeader.hpp"

namespace Trident {
	struct PhysicsRendererBuildHull2D {
		const HullHeader2D* hullHeader;
		uint16 maxTransformCount;
		uint16 transformCount = 0;
	};

	struct PhysicsRendererBuildOptions2D {
		ImageSampleCount sampleCount;
		sequence<PhysicsRendererBuildHull2D> hulls;
		uint16 maxCircleTransformCount;
	};
}