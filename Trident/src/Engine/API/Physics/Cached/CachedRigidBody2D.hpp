// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace Trident {
	struct CachedRigidBody2DDescriptor {
		svec2<float32> position;
		float32 rotation;
		svec2<float32> scale;
		float32 mass;
		float32 friction;
		uint32 geometryIndex;
	};

	class CachedRigidBody2D {
		public:
			CachedRigidBody2D() = default;

			explicit CachedRigidBody2D(const CachedRigidBody2DDescriptor& descriptor) :
				position(descriptor.position),
				rotation(descriptor.rotation),
				scale(descriptor.scale),
				friction(descriptor.friction),
				geometryIndex(descriptor.geometryIndex)
			{
				const float32 m = descriptor.mass;

				DEBUG_ASSERT(m > 0.0_kg);

				if (m < max<float32>()) {
					// See https://github.com/erincatto/box2d-lite/blob/227b71b6974ea57ab7e96d40f6374287bd6a0e77/src/Body.cpp#L47.
					const float32 area = descriptor.scale[0] * descriptor.scale[0] + descriptor.scale[1] * descriptor.scale[1];
					const float32 I = m * area / 12.0f;

					inverseMass = 1.0f / m;
					inverseInertia = 1.0f / I;
				}
			}

		public:
			svec2<float32> position;
			float32 rotation;
			svec2<float32> scale;
			float32 inverseMass = 0.0_kg;
			float32 inverseInertia = 0.0_kg;
			float32 friction;
			uint32 geometryIndex;
	};
}