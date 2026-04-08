// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/Entity.hpp"

namespace Trident {
	struct Joint2DDescriptor {
		Entity A;
		Entity B;
		vec2<float32> anchor;
		float32 biasFactor;
		float32 softness;
	};

	struct Joint2D {
		Entity A;
		Entity B;

		vec2<float32> localAnchorA;
		vec2<float32> localAnchorB;
		vec2<float32> accumulatedImpulse = {0.0f, 0.0f};
		mat2x2<float32> M;

		vec2<float32> bias = {0.0f, 0.0f};
		float32 biasFactor = 0.2f;
		float32 softness = 0.0f;

		vec2<float32> rA;
		vec2<float32> rB;
	};
}