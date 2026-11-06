// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageSampleCount.hpp"

namespace Trident {
	struct PhysicsRendererBuildOptions2D {
		/// Total number of hull vertices.
		uint32 maxVertexCount;

		/// Total number of transforms.
		uint32 maxTransformCount;

		ImageSampleCount sampleCount;
	};
}