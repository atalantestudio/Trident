#pragma once

#include "Engine/API/Geometry/Geometry2DType.hpp"
#include "Engine/API/Object/PhysicalObject.hpp"
#include "Engine/API/Physics/ContactManifold.hpp"

namespace ProjectA {
	typedef float32 (*ColliderPenetration2D)(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B);
	typedef ContactManifold2D (*Collider2D)(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, float32 penetration);

	static float32 CircleCircleColliderPenetration(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) {
		if (abs(lengthSq(B.position) - lengthSq(A.position)) < 0.001f) {
			// Degenerate case: The circles are coincident.
			// Return a positive penetration to avoid generating contact points.
			return max<float32>();
		}

		return lengthSq(B.position - A.position) - lyah::pow(A.geometry->radius + B.geometry->radius, 2.0f);
	}

	static constexpr ColliderPenetration2D COLLIDER_PENETRATION_2D_TABLE[Geometry2DType::MAX_ENUM][Geometry2DType::MAX_ENUM] = {
		{CircleCircleColliderPenetration},
	};

	// https://media.steampowered.com/apps/valve/2015/DirkGregorius_Contacts.pdf
	static ContactManifold2D CircleCircleCollider(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, float32 penetration) {
		ContactManifold2D contactManifold;
		contactManifold.contactNormal = normalized(B.position - A.position);
		contactManifold.penetration = penetration;
		contactManifold.contactPointCount = 1;

		const vec2<float32> sA = A.position + contactManifold.contactNormal * A.geometry->radius;
		const vec2<float32> sB = B.position - contactManifold.contactNormal * B.geometry->radius;

		contactManifold.contactPoints[0].position = (sA + sB) * 0.5f;

		return contactManifold;
	}

	static constexpr Collider2D COLLIDER_2D_TABLE[Geometry2DType::MAX_ENUM][Geometry2DType::MAX_ENUM] = {
		{CircleCircleCollider},
	};
}