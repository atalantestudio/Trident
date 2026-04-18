// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/3D/Contact3D.hpp"

namespace Trident {
	struct ContactManifold3D {
		/// The normal of the manifold's plane.
		vec3<float32> normal;

		/// The tangents of the manifold's plane.
		vec3<float32> tangent0;
		vec3<float32> tangent1;

		Contact3D contacts[4]{};
		uint8 contactCount;
	};
}