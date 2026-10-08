// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Material/MaterialReference.hpp"

#include "Engine/API/Physics/MomentOfInertia.hpp"

namespace Trident {
	struct EntityDescriptor {
		uint32 geometryIndex;
		MaterialReference material;
		vec3<float32> position;
		vec3<float32> rotation;
		float32 uniformScale;
		float32 mass;
		MomentOfInertia3D momentOfInertia;
		vec3<float32> inertiaScale;
		float32 friction;
		float32 restitution;
	};
}