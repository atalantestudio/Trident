// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Physics/Collider/clipEdge.hpp"
#include "Engine/API/Physics/Collider/findIncidentEdge.hpp"
#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/Feature2D.hpp"

namespace Trident {
	static float32 calculateEdgeDistance(ref<ConvexHullGeometry2D> geometry, vec2<float32> scale, uint32 edgeIndex) {
		const Edge& edge = geometry->edges[edgeIndex];
		const vec2<float32> a = geometry->vertices[edge.indexA] * scale;
		const vec2<float32> b = geometry->vertices[edge.indexB] * scale;
		const float32 edgeDistance = abs(signedDistanceLinePoint(a, b, {0.0f, 0.0f}));

		return edgeDistance;
	}

	static bool ConvexHullConvexHullCollider(
		const RigidBody2D& A,
		const RigidBody2D& B,
		ContactManifold2D* contactManifold
		#if defined(BUILD_MODE_DEBUG)
			, VisualizationData2D* visualizationData
		#endif
	) {
		const SAT2DOutput satOutput = SAT2D(A, B);

		#if defined(BUILD_MODE_DEBUG)
			visualizationData->satOutput = satOutput;
		#endif

		if (satOutput.separation > 0.0f) {
			return false;
		}

		ClipFeature2D incidentEdge = findIncidentEdge(*satOutput.incidentBody, satOutput.normal);

		const ref<ConvexHullGeometry2D> referenceGeometry = std::static_pointer_cast<ConvexHullGeometry2D>(satOutput.referenceBody->geometry);
		const vec2<float32> referenceRightEdgeNormal = perpendicularRight(satOutput.normal);
		const uint32 leftEdgeIndex = (static_cast<sint32>(satOutput.referenceEdgeIndex) - 1) % referenceGeometry->edges.count();
		const uint32 rightEdgeIndex = (satOutput.referenceEdgeIndex + 1) % referenceGeometry->edges.count();
		const float32 side = dot(satOutput.referenceBody->position, referenceRightEdgeNormal);

		// TODO(stable-simulation): `edgeDistance` and `offset` do not correctly map to arbitrary polygons.
		const float32 edgeDistance = calculateEdgeDistance(referenceGeometry, satOutput.referenceBody->scale, rightEdgeIndex);

		float32 offset = -side + edgeDistance;

		incidentEdge = clipEdge(incidentEdge, -referenceRightEdgeNormal, offset, leftEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		offset = side + edgeDistance;

		incidentEdge = clipEdge(incidentEdge, referenceRightEdgeNormal, offset, rightEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		contactManifold->normal = satOutput.normal;
		contactManifold->contactCount = 0;

		const float32 referenceNormalOffset = dot(satOutput.referenceBody->position, satOutput.normal) + calculateEdgeDistance(referenceGeometry, satOutput.referenceBody->scale, satOutput.referenceEdgeIndex);

		uint32 vertexIndex = 0;

		// Keep all vertices below the reference edge, then move them to the reference edge's line.
		while (vertexIndex < 2) {
			const ClipVertex2D& clipVertex = incidentEdge.edge[vertexIndex];
			const float32 separation = dot(clipVertex.position, satOutput.normal) - referenceNormalOffset;

			if (separation <= 0.0f) {
				Contact2D& contact = contactManifold->contacts[contactManifold->contactCount];
				contact.position = clipVertex.position - satOutput.normal * separation;
				contact.separation = separation;
				contact.id = clipVertex.id;

				contactManifold->contactCount += 1;
			}

			vertexIndex += 1;
		}

		if (satOutput.referenceBody == &B) {
			contactManifold->normal *= -1.0f;

			uint8 contactIndex = 0;

			while (contactIndex < contactManifold->contactCount) {
				contactManifold->contacts[contactIndex].id.flip();

				contactIndex += 1;
			}
		}

		DEBUG_ASSERT(contactManifold->contactCount > 0);

		return contactManifold->contactCount > 0;
	}
}