// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Collider/clipEdge.hpp"
#include "Engine/API/Physics/Collider/findIncidentEdge.hpp"
#include "Engine/API/Physics/Collider/Collider2D.hpp"

namespace Trident {
	static bool ConvexHullConvexHullCollider(const RigidBody2DComponent& A, const RigidBody2DComponent& B, const Hull2D& hullA, const Hull2D& hullB, ContactManifold2D* contactManifold) {
		const SAT2DOutput sat = SAT2D(A, B, hullA, hullB);

		if (sat.separation > 0.0f) {
			return false;
		}

		const vec2<float32> referenceCentroid = sat.referenceHull->centroid;
		const mat2x2<float32> referenceRotation = rotationAngle<mat2x2<float32>>(sat.referenceBody->rotation);

		const uint32 referenceLeftEdgeIndex = (static_cast<sint32>(sat.referenceEdgeIndex) - 1) % sat.referenceHull->edges.count();
		const uint32 referenceRightEdgeIndex = (sat.referenceEdgeIndex + 1) % sat.referenceHull->edges.count();

		const Edge2D referenceEdge = sat.referenceHull->edges[sat.referenceEdgeIndex];
		const Edge2D referenceLeftEdge = sat.referenceHull->edges[referenceLeftEdgeIndex];
		const Edge2D referenceRightEdge = sat.referenceHull->edges[referenceRightEdgeIndex];

		const vec2<float32> referenceEdgeA = sat.referenceHull->vertices[referenceEdge.indexA];
		const vec2<float32> referenceEdgeB = sat.referenceHull->vertices[referenceEdge.indexB];
		const vec2<float32> referenceLeftEdgeA = sat.referenceHull->vertices[referenceLeftEdge.indexA];
		const vec2<float32> referenceRightEdgeB = sat.referenceHull->vertices[referenceRightEdge.indexB];

		const vec2<float32> referenceLeftEdgeNormal = edgeNormalClockwise(referenceLeftEdgeA, referenceEdgeA) * referenceRotation;
		const vec2<float32> referenceRightEdgeNormal = edgeNormalClockwise(referenceEdgeB, referenceRightEdgeB) * referenceRotation;

		const float32 referenceEdgeDistance = abs(signedDistanceLPAB(referenceEdgeA * sat.referenceBody->scale, referenceEdgeB * sat.referenceBody->scale, referenceCentroid));
		const float32 referenceLeftEdgeDistance = abs(signedDistanceLPAB(referenceLeftEdgeA * sat.referenceBody->scale, referenceEdgeA * sat.referenceBody->scale, referenceCentroid));
		const float32 referenceRightEdgeDistance = abs(signedDistanceLPAB(referenceEdgeB * sat.referenceBody->scale, referenceRightEdgeB * sat.referenceBody->scale, referenceCentroid));

		ClipFeature2D incidentEdge = findIncidentEdge(*sat.incidentBody, *sat.incidentHull, sat.axis * rotationAngle<mat2x2<float32>>(-sat.incidentBody->rotation));
		Plane2D plane;

		// Clip against reference left edge.
		plane.normal = referenceLeftEdgeNormal;
		plane.offset = dot(sat.referenceBody->position, referenceLeftEdgeNormal) + referenceLeftEdgeDistance;
		incidentEdge = clipEdge(incidentEdge, plane, referenceLeftEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		// Clip against reference right edge.
		plane.normal = referenceRightEdgeNormal;
		plane.offset = dot(sat.referenceBody->position, referenceRightEdgeNormal) + referenceRightEdgeDistance;
		incidentEdge = clipEdge(incidentEdge, plane, referenceRightEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		contactManifold->normal = sat.axis;
		contactManifold->contactCount = 0;

		plane.normal = sat.axis;
		plane.offset = dot(sat.referenceBody->position, sat.axis) + referenceEdgeDistance;

		uint32 vertexIndex = 0;

		while (vertexIndex < 2) {
			const ClipVertex2D& clipVertex = incidentEdge.edge[vertexIndex];
			const float32 separation = signedDistance(clipVertex.position, plane);

			// Keep vertices that are below the reference plane.
			if (separation <= 0.0f) {
				// Move the contact point onto the reference plane.
				Contact2D& contact = contactManifold->contacts[contactManifold->contactCount];
				contact.position = clipVertex.position - sat.axis * separation;
				contact.separation = separation;
				contact.id = clipVertex.id;

				contactManifold->contactCount += 1;
			}

			vertexIndex += 1;
		}

		if (sat.referenceBody == &B) {
			contactManifold->normal *= -1.0f;

			uint8 contactIndex = 0;

			while (contactIndex < contactManifold->contactCount) {
				contactManifold->contacts[contactIndex].id.flip();

				contactIndex += 1;
			}
		}

		return contactManifold->contactCount > 0;
	}
}