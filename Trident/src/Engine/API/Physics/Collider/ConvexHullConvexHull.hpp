// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Collider/clipEdge.hpp"
#include "Engine/API/Physics/Collider/findIncidentEdge.hpp"
#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/Feature2D.hpp"

namespace Trident {
	static bool ConvexHullConvexHullCollider(const RigidBody2DComponent& A, const RigidBody2DComponent& B, const Hull2D& hullA, const Hull2D& hullB, ContactManifold2D* contactManifold) {
		const SAT2DOutput satOutput = SAT2D(A, B, hullA, hullB);

		if (satOutput.separation > 0.0f) {
			return false;
		}

		const mat2x2<float32> referenceInverseRotation = mat2x2<float32>::rotation(-satOutput.referenceBody->rotation);

		const Edge referenceEdge = satOutput.referenceHull->getEdges()[satOutput.referenceEdgeIndex];
		const float32 referenceEdgeDistance = abs(signedDistanceLinePoint(vec2<float32>(satOutput.referenceHull->getVertices()[referenceEdge.indexA]) * satOutput.referenceBody->scale, vec2<float32>(satOutput.referenceHull->getVertices()[referenceEdge.indexB]) * satOutput.referenceBody->scale, {0.0f, 0.0f}));

		const uint32 referenceLeftEdgeIndex = (static_cast<sint32>(satOutput.referenceEdgeIndex) - 1) % satOutput.referenceHull->getEdges().count();
		const Edge referenceLeftEdge = satOutput.referenceHull->getEdges()[referenceLeftEdgeIndex];
		const vec2<float32> referenceLeftEdgeNormal = edgeNormalClockwise(satOutput.referenceHull->getVertices()[referenceLeftEdge.indexA], satOutput.referenceHull->getVertices()[referenceLeftEdge.indexB]) * referenceInverseRotation;
		const float32 referenceLeftEdgeDistance = abs(signedDistanceLinePoint(vec2<float32>(satOutput.referenceHull->getVertices()[referenceLeftEdge.indexA]) * satOutput.referenceBody->scale, vec2<float32>(satOutput.referenceHull->getVertices()[referenceLeftEdge.indexB]) * satOutput.referenceBody->scale, {0.0f, 0.0f}));

		const uint32 referenceRightEdgeIndex = (satOutput.referenceEdgeIndex + 1) % satOutput.referenceHull->getEdges().count();
		const Edge referenceRightEdge = satOutput.referenceHull->getEdges()[referenceRightEdgeIndex];
		const vec2<float32> referenceRightEdgeNormal = edgeNormalClockwise(satOutput.referenceHull->getVertices()[referenceRightEdge.indexA], satOutput.referenceHull->getVertices()[referenceRightEdge.indexB]) * referenceInverseRotation;
		const float32 referenceRightEdgeDistance = abs(signedDistanceLinePoint(vec2<float32>(satOutput.referenceHull->getVertices()[referenceRightEdge.indexA]) * satOutput.referenceBody->scale, vec2<float32>(satOutput.referenceHull->getVertices()[referenceRightEdge.indexB]) * satOutput.referenceBody->scale, {0.0f, 0.0f}));

		ClipFeature2D incidentEdge = findIncidentEdge(*satOutput.incidentBody, *satOutput.incidentHull, satOutput.normal * mat2x2<float32>::rotation(satOutput.incidentBody->rotation));
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