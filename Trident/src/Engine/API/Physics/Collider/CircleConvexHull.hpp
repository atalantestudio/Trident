// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/PointGeometry2D.hpp"
#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/SATCircleConvexHull2D.hpp"

namespace Trident {
	static bool CircleConvexHullCollider(
		const RigidBody2D& A,
		const RigidBody2D& B,
		ContactManifold2D* contactManifold
		#if defined(BUILD_MODE_DEBUG)
			, VisualizationData2D* visualizationData
		#endif
	) {
		RigidBody2D pointA = A;
		pointA.geometry = createRef<PointGeometry2D>();

		// NOTE: The Y component is ignored for circle scaling.
		const float32 scaleA = A.scale[0];

		const GJK2DOutput gjkOutput = GJK2D(pointA, B);

		#if defined(BUILD_MODE_DEBUG)
			visualizationData->gjkOutput = gjkOutput;
		#endif

		const float32 d = gjkOutput.distance - A.geometry->radius * scaleA;

		if (d > 0.0f) {
			return false;
		}

		if (d > -A.geometry->radius * scaleA) {
			contactManifold->normal = normalized(gjkOutput.closestB - gjkOutput.closestA);
			contactManifold->contactCount = 1;
			contactManifold->contacts[0].position = gjkOutput.closestB;
			contactManifold->contacts[0].separation = d;

			return true;
		}

		const SAT2DOutput satOutput = SATCircleConvexHull2D(A, B);

		#if defined(BUILD_MODE_DEBUG)
			visualizationData->satOutput = satOutput;
		#endif

		contactManifold->normal = -satOutput.normal;
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = satOutput.incidentSupportPoint - satOutput.normal * satOutput.separation;
		contactManifold->contacts[0].separation = satOutput.separation - A.geometry->radius * scaleA;

		return true;
	}
}