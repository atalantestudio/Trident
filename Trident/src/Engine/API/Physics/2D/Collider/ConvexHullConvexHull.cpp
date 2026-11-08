// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider2D.hpp"
#include "Engine/API/Physics/2D/Feature2D.hpp"
#include "Engine/API/Physics/2D/SAT2D.hpp"

namespace Trident {
	static mat3x3<float32> world(const RigidBody2D& rigidBody) {
		return (
			translation<mat3x3<float32>>(-rigidBody.centroid) *
			rotationAngle<mat3x3<float32>>(rigidBody.rotation) *
			translation<mat3x3<float32>>(rigidBody.position)
		);
	}

	/// Finds and returns the incident edge on the provided rigid body.
	static ClipFeature2D findIncidentEdge(
		/// The rigid body owning the incident edge.
		const RigidBody2D& incidentBody,

		/// The hull used by the rigid body.
		const Shape2D& incidentShape,

		/// The reference edge normal, rotated by (Reference - Incident).
		vec2<float32> referenceEdgeNormal
	) {
		const vec2<float32>* vertices = pointer_cast<const vec2<float32>*>(pointer_cast<const uint8*>(incidentShape.hull) + incidentShape.hull->vertexOffset);

		ClipFeature2D incidentEdge{};
		incidentEdge.vertexCount = 2;

		float32 minAngle = max<float32>();
		uint32 incidentEdgeIndex = 0;
		uint16 vertexIndex = 0;

		while (vertexIndex < incidentShape.hull->vertexCount) {
			const vec2<float32> a = vertices[vertexIndex];
			const vec2<float32> b = vertices[(vertexIndex + 1) % incidentShape.hull->vertexCount];
			const vec2<float32> d = perpendicularLeft(b - a);
			const float32 angle = dot(d, referenceEdgeNormal);

			if (angle < minAngle) {
				minAngle = angle;
				incidentEdgeIndex = vertexIndex;

				// TODO(physics/2d/incident-edge-vertex-order): Why is the simulation unstable when a and b are inverted?
				incidentEdge.edge[0].position = b;
				incidentEdge.edge[1].position = a;
			}

			vertexIndex += 1;
		}

		ClipVertex2D& a = incidentEdge.edge[0];
		a.position = a.position * world(incidentBody);
		a.id.referenceEdgeAIndex = Contact2DID::NULL_EDGE_INDEX;
		a.id.referenceEdgeBIndex = Contact2DID::NULL_EDGE_INDEX;
		a.id.incidentEdgeAIndex = (incidentEdgeIndex + 1) % incidentShape.hull->vertexCount;
		a.id.incidentEdgeBIndex = incidentEdgeIndex;

		ClipVertex2D& b = incidentEdge.edge[1];
		b.position = b.position * world(incidentBody);
		b.id.referenceEdgeAIndex = Contact2DID::NULL_EDGE_INDEX;
		b.id.referenceEdgeBIndex = Contact2DID::NULL_EDGE_INDEX;
		b.id.incidentEdgeAIndex = incidentEdgeIndex;
		b.id.incidentEdgeBIndex = (static_cast<sint16>(incidentEdgeIndex) - 1) % incidentShape.hull->vertexCount;

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

	bool ConvexHullConvexHull2D(const Shape2D& shapeA, const Shape2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* contactManifold) {
		const SAT2DOutput sat = SAT2D(rigidBodyA, rigidBodyB, shapeA, shapeB);

		if (sat.separation > 0.0f) {
			return false;
		}

		const vec2<float32> referenceCentroid = sat.referenceBody->centroid;
		const mat2x2<float32> referenceRotation = rotationAngle<mat2x2<float32>>(sat.referenceBody->rotation);

		const uint32 referenceLeftEdgeIndex = (static_cast<sint32>(sat.referenceEdgeIndex) - 1) % sat.referenceShape->hull->vertexCount;
		const uint32 referenceRightEdgeIndex = (sat.referenceEdgeIndex + 1) % sat.referenceShape->hull->vertexCount;

		const vec2<float32>* referenceVertices = pointer_cast<const vec2<float32>*>(pointer_cast<const uint8*>(sat.referenceShape->hull) + sat.referenceShape->hull->vertexOffset);

		const vec2<float32> referenceLeftEdgeA = referenceVertices[(static_cast<sint32>(sat.referenceEdgeIndex) - 1) % sat.referenceShape->hull->vertexCount];
		const vec2<float32> referenceEdgeA = referenceVertices[sat.referenceEdgeIndex];
		const vec2<float32> referenceEdgeB = referenceVertices[(sat.referenceEdgeIndex + 1) % sat.referenceShape->hull->vertexCount];
		const vec2<float32> referenceRightEdgeB = referenceVertices[(sat.referenceEdgeIndex + 2) % sat.referenceShape->hull->vertexCount];

		const vec2<float32> referenceLeftEdgeNormal = edgeNormalClockwise(referenceLeftEdgeA, referenceEdgeA) * referenceRotation;
		const vec2<float32> referenceRightEdgeNormal = edgeNormalClockwise(referenceEdgeB, referenceRightEdgeB) * referenceRotation;

		const float32 referenceEdgeDistanceToCentroid = abs(signedDistanceLPAB(referenceEdgeA, referenceEdgeB, referenceCentroid));
		const float32 referenceLeftEdgeDistanceToCentroid = abs(signedDistanceLPAB(referenceLeftEdgeA, referenceEdgeA, referenceCentroid));
		const float32 referenceRightEdgeDistanceToCentroid = abs(signedDistanceLPAB(referenceEdgeB, referenceRightEdgeB, referenceCentroid));

		ClipFeature2D incidentEdge = findIncidentEdge(*sat.incidentBody, *sat.incidentShape, sat.axis * rotationAngle<mat2x2<float32>>(-sat.incidentBody->rotation));
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

		contactManifold->normal = sat.axis;
		contactManifold->contactCount = 0;

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
				Contact2D& contact = contactManifold->contacts[contactManifold->contactCount];
				contact.position = clipVertex.position - sat.axis * separation;
				contact.separation = separation;
				contact.id = clipVertex.id;

				contactManifold->contactCount += 1;
			}

			vertexIndex += 1;
		}

		if (sat.referenceBody == &rigidBodyB) {
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