// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Physics/Collider/clipEdge.hpp"
#include "Engine/API/Physics/Collider/findIncidentEdge.hpp"
#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/Feature2D.hpp"

namespace ProjectA {
	static bool ConvexHullConvexHullCollider(
		const RigidBody2D& A,
		const RigidBody2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) {
		const SAT2DOutput satOutput = SAT2D(A, B);

		#ifdef BUILD_MODE_DEBUG
			visualizationData->satOutput = satOutput;
		#endif

		if (satOutput.separation > 0.0f) {
			return false;
		}

		ClipFeature2D incidentEdge = findIncidentEdge(*satOutput.incidentBody, satOutput.normal);

		const vec2<float32> referenceRightEdgeNormal = normalized(satOutput.referenceEdge.edge[1] - satOutput.referenceEdge.edge[0]);
		const ref<ConvexHullGeometry2D> referenceGeometry = std::static_pointer_cast<ConvexHullGeometry2D>(satOutput.referenceBody->geometry);
		const uint32 leftEdgeIndex = (static_cast<sint32>(satOutput.referenceEdgeIndex) - 1) % referenceGeometry->edges.count();

		incidentEdge = clipEdge(incidentEdge, -referenceRightEdgeNormal, -dot(referenceRightEdgeNormal, satOutput.referenceEdge.edge[0]), leftEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		const uint32 rightEdgeIndex = (satOutput.referenceEdgeIndex + 1) % referenceGeometry->edges.count();

		incidentEdge = clipEdge(incidentEdge, referenceRightEdgeNormal, dot(referenceRightEdgeNormal, satOutput.referenceEdge.edge[1]), rightEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		contactManifold->normal = satOutput.normal;

		if (satOutput.referenceBody == &B) {
			contactManifold->normal *= -1.0f;
		}

		const float32 referenceNormalOffset = dot(satOutput.referenceEdge.edge[0], satOutput.normal);

		uint32 vertexIndex = 0;
		uint32 contactCount = 0;

		// Keep all vertices below the reference edge, then move them to the reference edge's line.
		while (vertexIndex < 2) {
			const ClipVertex2D& clipVertex = incidentEdge.edge[vertexIndex];
			const float32 separation = dot(clipVertex.position, satOutput.normal) - referenceNormalOffset;

			if (separation <= 0.0f) {
				Contact2D& contact = contactManifold->contacts[contactCount];
				contact.position = clipVertex.position - satOutput.normal * separation;
				contact.separation = separation;
				contact.id = clipVertex.id;

				if (satOutput.referenceBody == &B) {
					contact.id.flip();
				}

				contactCount += 1;
			}

			vertexIndex += 1;
		}

		contactManifold->contactCount = contactCount;

		return contactCount;
	}
}