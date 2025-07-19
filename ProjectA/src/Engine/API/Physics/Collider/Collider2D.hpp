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
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = (sA + sB) * 0.5f;
		contactManifold->contacts[0].separation = d;

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
			contactManifold->contactCount = 1;
			contactManifold->contacts[0].position = gjkOutput.closestB;
			contactManifold->contacts[0].separation = d;

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

		Feature2D referenceEdge;
		referenceEdge.edge[0] = satOutput.edgeA1;
		referenceEdge.edge[1] = satOutput.edgeA2;

		const Feature2D incidentEdge = findIncidentEdge(*satOutput.incidentObject, satOutput.normal);
		const vec2<float32> referenceSideEdgeNormal = normalized(referenceEdge.edge[1] - referenceEdge.edge[0]);

		Feature2D clippedIncidentEdge = incidentEdge;

		clippedIncidentEdge = clipEdge(clippedIncidentEdge, -referenceSideEdgeNormal, -dot(referenceSideEdgeNormal, referenceEdge.edge[0]));

		if (clippedIncidentEdge.getVertexCount() < 2) {
			return false;
		}

		clippedIncidentEdge = clipEdge(clippedIncidentEdge, referenceSideEdgeNormal, dot(referenceSideEdgeNormal, referenceEdge.edge[1]));

		if (clippedIncidentEdge.getVertexCount() < 2) {
			return false;
		}

		contactManifold->contactCount = 0;

		const float32 referenceNormalOffset = dot(satOutput.normal, referenceEdge.edge[0]);

		// Keep all vertices below the reference edge, then move them to the reference edge's line.
		for (uint32 vertexIndex = 0; vertexIndex < 2; vertexIndex += 1) {
			const float32 separation = dot(clippedIncidentEdge.edge[vertexIndex], satOutput.normal) - referenceNormalOffset;

			if (separation > 0.0f) {
				continue;
			}

			contactManifold->contacts[contactManifold->contactCount].position = clippedIncidentEdge.edge[vertexIndex] - satOutput.normal * separation;
			contactManifold->contacts[contactManifold->contactCount].separation = separation;
			contactManifold->contactCount += 1;
		}

		return true;
	}

	// https://media.steampowered.com/apps/valve/2015/DirkGregorius_Contacts.pdf
	static constexpr Collider2D COLLIDER_2D_TABLE[static_cast<uint32>(Geometry2DType::MAX_ENUM)][static_cast<uint32>(Geometry2DType::MAX_ENUM)] = {
		{CircleCircleCollider, CircleConvexHullCollider},
		{ConvexHullCircleCollider, ConvexHullConvexHullCollider},
	};
}