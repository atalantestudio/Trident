// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Contact/Contact.hpp"

namespace Trident {
	struct ContactManifold2D {
		/// The normal of the manifold's plane.
		/// It points away from the reference body and towards the incident body.
		vec2<float32> normal;

		/// The tangent of the manifold's plane.
		vec2<float32> tangent;

		Contact2D contacts[2]{};
		uint8 contactCount;
	};
}