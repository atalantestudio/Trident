// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"
#include "Engine/API/Physics/Feature2D.hpp"
#include "Engine/API/Physics/SAT2D.hpp"

namespace Trident {
	/// Finds and returns the incident edge on the provided rigid body.
	static ClipFeature2D findIncidentEdge(
		/// The rigid body owning the incident edge.
		const RigidBody2DComponent& incidentBody,

		/// The hull used by the rigid body.
		const Hull2D& incidentHull,

		/// The reference edge normal, rotated by (Reference - Incident).
		vec2<float32> referenceEdgeNormal
	) {
		ClipFeature2D incidentEdge{};
		incidentEdge.vertexCount = 2;

		const uint16 edgeCount = static_cast<uint16>(incidentHull.edges.count());

		float32 minAngle = max<float32>();
		uint32 incidentEdgeIndex = 0;
		uint16 edgeIndex = 0;

		while (edgeIndex < edgeCount) {
			const Edge2D edge = incidentHull.edges[edgeIndex];
			const vec2<float32> a = incidentHull.vertices[edge.indexA];
			const vec2<float32> b = incidentHull.vertices[edge.indexB];
			const vec2<float32> d = perpendicularLeft(b - a);
			const float32 angle = dot(d, referenceEdgeNormal);

			if (angle < minAngle) {
				minAngle = angle;
				incidentEdgeIndex = edgeIndex;

				// TODO(physics/2d/incident-edge-vertex-order): Why is the simulation unstable when a and b are inverted?
				incidentEdge.edge[0].position = b;
				incidentEdge.edge[1].position = a;
			}

			edgeIndex += 1;
		}

		ClipVertex2D& a = incidentEdge.edge[0];
		a.position = a.position * incidentBody.world;
		a.id.referenceEdgeAIndex = Contact2DID::NULL_EDGE_INDEX;
		a.id.referenceEdgeBIndex = Contact2DID::NULL_EDGE_INDEX;
		a.id.incidentEdgeAIndex = (incidentEdgeIndex + 1) % edgeCount;
		a.id.incidentEdgeBIndex = incidentEdgeIndex;

		ClipVertex2D& b = incidentEdge.edge[1];
		b.position = b.position * incidentBody.world;
		b.id.referenceEdgeAIndex = Contact2DID::NULL_EDGE_INDEX;
		b.id.referenceEdgeBIndex = Contact2DID::NULL_EDGE_INDEX;
		b.id.incidentEdgeAIndex = incidentEdgeIndex;
		b.id.incidentEdgeBIndex = (static_cast<sint16>(incidentEdgeIndex) - 1) % edgeCount;

		return incidentEdge;
	}

	/// Clips an edge against a 2D plane.
	/// Returns a new edge containing the clipped vertices.
	static ClipFeature2D clipEdge(const ClipFeature2D& edge, Plane2D plane, uint32 edgeIndex) {
		const ClipVertex2D a = edge.edge[0];
		const ClipVertex2D b = edge.edge[1];

		const float32 distanceA = signedDistance(a.position, plane);
		const float32 distanceB = signedDistance(b.position, plane);

		ClipFeature2D clippedEdge{};

		if (distanceA <= 0.0f) {
			// Keep vertices that are below the line.
			clippedEdge.edge[clippedEdge.vertexCount] = a;
			clippedEdge.vertexCount += 1;
		}

		if (distanceB <= 0.0f) {
			// Keep vertices that are below the line.
			clippedEdge.edge[clippedEdge.vertexCount] = b;
			clippedEdge.vertexCount += 1;
		}

		if (distanceA * distanceB < 0.0f) {
			// If the vertices are on different sides of the plane, insert the intersection point.
			DEBUG_ASSERT(clippedEdge.vertexCount < 2);

			const float32 t = distanceA / (distanceA - distanceB);

			ClipVertex2D& c = clippedEdge.edge[clippedEdge.vertexCount];
			c.position = lerp(a.position, b.position, t);

			if (distanceA > 0.0f) {
				c.id = a.id;
				c.id.referenceEdgeAIndex = edgeIndex;
				c.id.referenceEdgeBIndex = Contact2DID::NULL_EDGE_INDEX;
			}
			else {
				c.id = b.id;
				c.id.incidentEdgeAIndex = edgeIndex;
				c.id.incidentEdgeBIndex = Contact2DID::NULL_EDGE_INDEX;
			}

			clippedEdge.vertexCount += 1;
		}

		DEBUG_ASSERT(clippedEdge.vertexCount <= 2);

		return clippedEdge;
	}

	bool ConvexHullConvexHull2D(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2DComponent& rigidBodyA, const RigidBody2DComponent& rigidBodyB, ContactManifold2D* manifold) {
		const SAT2DOutput sat = SAT2D(rigidBodyA, rigidBodyB, shapeA, shapeB);

		if (sat.separation > 0.0f) {
			return false;
		}

		const vec2<float32> referenceCentroid = sat.referenceHull->centroid;
		const mat2x2<float32> referenceRotation = rotationAngle<mat2x2<float32>>(sat.referenceBody->rotation);

		const uint32 referenceLeftEdgeIndex = (static_cast<sint32>(sat.referenceEdgeIndex) - 1) % sat.referenceHull->edges.count();
		const uint32 referenceRightEdgeIndex = (sat.referenceEdgeIndex + 1) % sat.referenceHull->edges.count();

		const Edge2D referenceLeftEdge = sat.referenceHull->edges[referenceLeftEdgeIndex];
		const Edge2D referenceEdge = sat.referenceHull->edges[sat.referenceEdgeIndex];
		const Edge2D referenceRightEdge = sat.referenceHull->edges[referenceRightEdgeIndex];

		const vec2<float32> referenceLeftEdgeA = sat.referenceHull->vertices[referenceLeftEdge.indexA];
		const vec2<float32> referenceEdgeA = sat.referenceHull->vertices[referenceEdge.indexA];
		const vec2<float32> referenceEdgeB = sat.referenceHull->vertices[referenceEdge.indexB];
		const vec2<float32> referenceRightEdgeB = sat.referenceHull->vertices[referenceRightEdge.indexB];

		const vec2<float32> referenceLeftEdgeNormal = edgeNormalClockwise(referenceLeftEdgeA, referenceEdgeA) * referenceRotation;
		const vec2<float32> referenceRightEdgeNormal = edgeNormalClockwise(referenceEdgeB, referenceRightEdgeB) * referenceRotation;

		const float32 referenceEdgeDistanceToCentroid = abs(signedDistanceLPAB(referenceEdgeA * sat.referenceBody->scale, referenceEdgeB * sat.referenceBody->scale, referenceCentroid));
		const float32 referenceLeftEdgeDistanceToCentroid = abs(signedDistanceLPAB(referenceLeftEdgeA * sat.referenceBody->scale, referenceEdgeA * sat.referenceBody->scale, referenceCentroid));
		const float32 referenceRightEdgeDistanceToCentroid = abs(signedDistanceLPAB(referenceEdgeB * sat.referenceBody->scale, referenceRightEdgeB * sat.referenceBody->scale, referenceCentroid));

		ClipFeature2D incidentEdge = findIncidentEdge(*sat.incidentBody, *sat.incidentHull, sat.axis * rotationAngle<mat2x2<float32>>(-sat.incidentBody->rotation));
		Plane2D plane;

		// Clip against reference left edge.
		plane.normal = referenceLeftEdgeNormal;
		plane.offset = referenceLeftEdgeDistanceToCentroid + dot(sat.referenceBody->position, referenceLeftEdgeNormal);
		incidentEdge = clipEdge(incidentEdge, plane, referenceLeftEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		// Clip against reference right edge.
		plane.normal = referenceRightEdgeNormal;
		plane.offset = referenceRightEdgeDistanceToCentroid + dot(sat.referenceBody->position, referenceRightEdgeNormal);
		incidentEdge = clipEdge(incidentEdge, plane, referenceRightEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		manifold->normal = sat.axis;
		manifold->contactCount = 0;

		// Build a plane for the reference edge.
		Plane2D referencePlane{};
		referencePlane.normal = sat.axis;
		referencePlane.offset = referenceEdgeDistanceToCentroid + dot(sat.referenceBody->position, sat.axis);

		uint32 vertexIndex = 0;

		while (vertexIndex < 2) {
			const ClipVertex2D& clipVertex = incidentEdge.edge[vertexIndex];
			const float32 separation = signedDistance(clipVertex.position, referencePlane);

			// Keep vertices that are below the reference plane.
			if (separation <= 0.0f) {
				// Move the contact point onto the reference plane.
				Contact2D& contact = manifold->contacts[manifold->contactCount];
				contact.position = clipVertex.position - sat.axis * separation;
				contact.separation = separation;
				contact.id = clipVertex.id;

				manifold->contactCount += 1;
			}

			vertexIndex += 1;
		}

		if (sat.referenceBody == &rigidBodyB) {
			manifold->normal *= -1.0f;

			uint8 contactIndex = 0;

			while (contactIndex < manifold->contactCount) {
				manifold->contacts[contactIndex].id.flip();

				contactIndex += 1;
			}
		}

		return manifold->contactCount > 0;
	}
}