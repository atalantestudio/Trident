// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Collider/Collider2D.hpp"

namespace Trident {
	static bool CircleCircleCollider(
		const RigidBody2D& A,
		const RigidBody2D& B,
		ContactManifold2D* contactManifold
		#if defined(BUILD_MODE_DEBUG)
			, VisualizationData2D* visualizationData
		#endif
	) {
		const vec2<float32> AB = B.position - A.position;
		const float32 lengthAB = lengthSquared(AB);

		if (abs(lengthAB) < 0.001f) {
			// Degenerate case: The circles are coincident.
			return false;
		}

		// NOTE: The Y component is ignored for circle scaling.
		const float32 scaleA = A.scale[0];
		const float32 scaleB = B.scale[0];

		const float32 d = lengthAB - pow(A.geometry->radius * scaleA + B.geometry->radius * scaleB, 2.0f);

		if (d > 0.0f) {
			return false;
		}

		const vec2<float32> sA = A.position + A.geometry->support(AB) * scaleA;
		const vec2<float32> sB = B.position + B.geometry->support(-AB) * scaleB;

		contactManifold->normal = normalized(AB);
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = (sA + sB) * 0.5f;
		contactManifold->contacts[0].separation = d;

		return true;
	}
}