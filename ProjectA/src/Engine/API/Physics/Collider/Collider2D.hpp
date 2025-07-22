#pragma once

#include "Engine/API/Geometry/PointGeometry2D.hpp"
#include "Engine/API/Object/PhysicalObject.hpp"
#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/SAT2D.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/API/Physics/VisualizationData.hpp"
#endif

namespace ProjectA {
	typedef bool (*Collider2D)(
		const PhysicalObjectCopy2D& A,
		const PhysicalObjectCopy2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	);

	// TODO: Write to visualizationData in debug-mode.
	static bool CircleCircleCollider(
		const PhysicalObjectCopy2D& A,
		const PhysicalObjectCopy2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) {
		if (abs(lengthSq(B.position) - lengthSq(A.position)) < 0.001f) {
			// Degenerate case: The circles are coincident.
			return false;
		}

		const float32 d = lengthSq(B.position - A.position) - lyah::pow(A.geometry->radius + B.geometry->radius, 2.0f);

		if (d > 0.0f) {
			return false;
		}

		const vec2<float32> sA = A.position + contactManifold->normal * A.geometry->radius;
		const vec2<float32> sB = B.position - contactManifold->normal * B.geometry->radius;

		contactManifold->normal = normalized(B.position - A.position);
		contactManifold->setContactCount(1);
		contactManifold->contacts[0].position = (sA + sB) * 0.5f;
		contactManifold->contacts[0].separation = d;

		return true;
	}

	// TODO: Write to visualizationData in debug-mode.
	static bool CircleConvexHullCollider(
		const PhysicalObjectCopy2D& A,
		const PhysicalObjectCopy2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) {
		PhysicalObjectCopy2D pointA = A;
		pointA.geometry = createRef<PointGeometry2D>();

		const GJK2DOutput gjkOutput = GJK2D(pointA, B);
		const float32 d = gjkOutput.distance - A.geometry->radius;

		if (d > 0.0f) {
			return false;
		}

		if (d > -A.geometry->radius) {
			contactManifold->normal = normalized(gjkOutput.closestB - gjkOutput.closestA);
			contactManifold->setContactCount(1);
			contactManifold->contacts[0].position = gjkOutput.closestB;
			contactManifold->contacts[0].separation = d;

			return true;
		}

		// TODO: Handle deep contact with SAT.
		BREAKPOINT();

		return true;
	}

	static bool ConvexHullCircleCollider(
		const PhysicalObjectCopy2D& A,
		const PhysicalObjectCopy2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) {
		return CircleConvexHullCollider(
			B,
			A,
			contactManifold
			#ifdef BUILD_MODE_DEBUG
				, visualizationData
			#endif
		);
	}

	static bool ConvexHullConvexHullCollider(
		const PhysicalObjectCopy2D& A,
		const PhysicalObjectCopy2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) {
		const SAT2DOutput satOutput = SAT2D(A, B);

		#ifdef BUILD_MODE_DEBUG
			if (visualizationData != nullptr) {
				visualizationData->satOutput = satOutput;
			}
		#endif

		if (satOutput.separation > 0.0f) {
			return false;
		}

		const Feature2D incidentEdge = findIncidentEdge(*satOutput.incidentObject, satOutput.normal);
		const vec2<float32> referenceRightEdgeNormal = normalized(satOutput.referenceEdge.edge[1] - satOutput.referenceEdge.edge[0]);

		Feature2D clippedIncidentEdge = incidentEdge;
		clippedIncidentEdge = clipEdge(clippedIncidentEdge, -referenceRightEdgeNormal, -dot(referenceRightEdgeNormal, satOutput.referenceEdge.edge[0]));

		if (clippedIncidentEdge.getVertexCount() < 2) {
			return false;
		}

		clippedIncidentEdge = clipEdge(clippedIncidentEdge, referenceRightEdgeNormal, dot(referenceRightEdgeNormal, satOutput.referenceEdge.edge[1]));

		if (clippedIncidentEdge.getVertexCount() < 2) {
			return false;
		}

		contactManifold->normal = satOutput.normal;

		if (satOutput.referenceObject == &B) {
			contactManifold->normal = -contactManifold->normal;
		}

		const float32 referenceNormalOffset = dot(satOutput.referenceEdge.edge[0], satOutput.normal);
		uint32 contactCount = 0;

		// Keep all vertices below the reference edge, then move them to the reference edge's line.
		for (uint32 vertexIndex = 0; vertexIndex < 2; vertexIndex += 1) {
			const float32 separation = dot(clippedIncidentEdge.edge[vertexIndex], satOutput.normal) - referenceNormalOffset;

			if (separation > 0.0f) {
				continue;
			}

			contactManifold->contacts[contactCount].position = clippedIncidentEdge.edge[vertexIndex] - contactManifold->normal * separation;
			contactManifold->contacts[contactCount].separation = separation;

			contactCount += 1;
		}

		contactManifold->setContactCount(contactCount);

		return contactCount;
	}

	// https://media.steampowered.com/apps/valve/2015/DirkGregorius_Contacts.pdf
	static constexpr Collider2D COLLIDER_2D_TABLE[static_cast<uint32>(Geometry2DType::MAX_ENUM)][static_cast<uint32>(Geometry2DType::MAX_ENUM)] = {
		{CircleCircleCollider, CircleConvexHullCollider},
		{ConvexHullCircleCollider, ConvexHullConvexHullCollider},
	};
}