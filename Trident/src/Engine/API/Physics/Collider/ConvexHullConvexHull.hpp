// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Physics/Collider/clipEdge.hpp"
#include "Engine/API/Physics/Collider/findIncidentEdge.hpp"
#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/Feature2D.hpp"

namespace Trident {
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

		const ref<ConvexHullGeometry2D> referenceGeometry = std::static_pointer_cast<ConvexHullGeometry2D>(satOutput.referenceBody->geometry);
		const mat2x2<float32> referenceInverseRotation = mat2x2<float32>::rotation(-satOutput.referenceBody->rotation);

		const Edge referenceEdge = referenceGeometry->edges[satOutput.referenceEdgeIndex];
		const float32 referenceEdgeDistance = abs(signedDistanceLinePoint(referenceGeometry->vertices[referenceEdge.indexA] * satOutput.referenceBody->scale, referenceGeometry->vertices[referenceEdge.indexB] * satOutput.referenceBody->scale, {0.0f, 0.0f}));

		const uint32 referenceLeftEdgeIndex = (static_cast<sint32>(satOutput.referenceEdgeIndex) - 1) % referenceGeometry->edges.count();
		const Edge referenceLeftEdge = referenceGeometry->edges[referenceLeftEdgeIndex];
		const vec2<float32> referenceLeftEdgeNormal = edgeNormalClockwise(referenceGeometry->vertices[referenceLeftEdge.indexA], referenceGeometry->vertices[referenceLeftEdge.indexB]) * referenceInverseRotation;
		const float32 referenceLeftEdgeDistance = abs(signedDistanceLinePoint(referenceGeometry->vertices[referenceLeftEdge.indexA] * satOutput.referenceBody->scale, referenceGeometry->vertices[referenceLeftEdge.indexB] * satOutput.referenceBody->scale, {0.0f, 0.0f}));

		const uint32 referenceRightEdgeIndex = (satOutput.referenceEdgeIndex + 1) % referenceGeometry->edges.count();
		const Edge referenceRightEdge = referenceGeometry->edges[referenceRightEdgeIndex];
		const vec2<float32> referenceRightEdgeNormal = edgeNormalClockwise(referenceGeometry->vertices[referenceRightEdge.indexA], referenceGeometry->vertices[referenceRightEdge.indexB]) * referenceInverseRotation;
		const float32 referenceRightEdgeDistance = abs(signedDistanceLinePoint(referenceGeometry->vertices[referenceRightEdge.indexA] * satOutput.referenceBody->scale, referenceGeometry->vertices[referenceRightEdge.indexB] * satOutput.referenceBody->scale, {0.0f, 0.0f}));

		ClipFeature2D incidentEdge = findIncidentEdge(*satOutput.incidentBody, satOutput.normal * mat2x2<float32>::rotation(satOutput.incidentBody->rotation));
		float32 offset;

		// Clip against reference left edge.
		offset = dot(satOutput.referenceBody->position, referenceLeftEdgeNormal) + referenceLeftEdgeDistance;
		incidentEdge = clipEdge(incidentEdge, referenceLeftEdgeNormal, offset, referenceLeftEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		// Clip against reference right edge.
		offset = dot(satOutput.referenceBody->position, referenceRightEdgeNormal) + referenceRightEdgeDistance;
		incidentEdge = clipEdge(incidentEdge, referenceRightEdgeNormal, offset, referenceRightEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		contactManifold->normal = satOutput.normal;
		contactManifold->contactCount = 0;

		offset = dot(satOutput.referenceBody->position, satOutput.normal) + referenceEdgeDistance;

		uint32 vertexIndex = 0;

		// Discard vertices with a positive separation and put the rest on the reference edge.
		while (vertexIndex < 2) {
			const ClipVertex2D& clipVertex = incidentEdge.edge[vertexIndex];
			const float32 separation = dot(clipVertex.position, satOutput.normal) - offset;

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

		return contactManifold->contactCount > 0;
	}
}