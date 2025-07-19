#pragma once

#include "Engine/API/Geometry/PointGeometry2D.hpp"
#include "Engine/API/Object/PhysicalObject.hpp"
#include "Engine/API/Physics/ContactManifold.hpp"
#include "Engine/API/Physics/VisualizationData.hpp"

namespace ProjectA {
	typedef bool (*Collider2D)(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, ContactManifold2D* contactManifold, VisualizationData2D* visualizationData);

	static bool CircleCircleCollider(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, ContactManifold2D* contactManifold, VisualizationData2D* visualizationData) {
		if (abs(lengthSq(B.position) - lengthSq(A.position)) < 0.001f) {
			// Degenerate case: The circles are coincident.
			return false;
		}

		const float32 d = lengthSq(B.position - A.position) - lyah::pow(A.geometry->radius + B.geometry->radius, 2.0f);

		if (d > 0.0f) {
			return false;
		}

		const vec2<float32> sA = A.position + contactManifold->contactNormal * A.geometry->radius;
		const vec2<float32> sB = B.position - contactManifold->contactNormal * B.geometry->radius;

		contactManifold->contactNormal = normalized(B.position - A.position);
		contactManifold->penetration = d;
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = (sA + sB) * 0.5f;

		return true;
	}

	static bool CircleConvexHullCollider(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, ContactManifold2D* contactManifold, VisualizationData2D* visualizationData) {
		PhysicalObjectCopy2D pointA = A;
		pointA.geometry = createRef<PointGeometry2D>();

		const GJK2DOutput gjkOutput = GJK2D(pointA, B);
		const float32 d = gjkOutput.distance - A.geometry->radius;

		if (d > 0.0f) {
			return false;
		}

		if (d > -A.geometry->radius) {
			contactManifold->contactNormal = normalized(gjkOutput.closestB - gjkOutput.closestA);
			contactManifold->penetration = d;
			contactManifold->contactCount = 1;
			contactManifold->contacts[0].position = gjkOutput.closestB;

			return true;
		}

		// TODO: Handle deep contact with SAT.
		BREAKPOINT();

		return true;
	}

	static bool ConvexHullCircleCollider(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, ContactManifold2D* contactManifold, VisualizationData2D* visualizationData) {
		return CircleConvexHullCollider(B, A, contactManifold, visualizationData);
	}

	static bool ConvexHullConvexHullCollider(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, ContactManifold2D* contactManifold, VisualizationData2D* visualizationData) {
		const SAT2DOutput satOutput = SAT2D(A, B);

		if (visualizationData != nullptr) {
			visualizationData->satOutput = satOutput;
		}

		if (satOutput.separation > 0.0f) {
			return false;
		}

		// TODO

		return true;
	}

	// https://media.steampowered.com/apps/valve/2015/DirkGregorius_Contacts.pdf
	static constexpr Collider2D COLLIDER_2D_TABLE[static_cast<uint32>(Geometry2DType::MAX_ENUM)][static_cast<uint32>(Geometry2DType::MAX_ENUM)] = {
		{CircleCircleCollider, CircleConvexHullCollider},
		{ConvexHullCircleCollider, ConvexHullConvexHullCollider},
	};
}