// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"
#include "Engine/API/Physics/Feature2D.hpp"
#include "Engine/API/Physics/2D/SAT/SATConvexHullConvexHull.hpp"

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

	static bool createEdgeContact(const SATConvexHullConvexHullOutput2D& query, const Hull2D& referenceShape, const Hull2D& incidentShape, const RigidBody2DComponent& referenceBody, const RigidBody2DComponent& incidentBody, ContactManifold2D* manifold) {
		const uint32 referenceLeftEdgeIndex = (static_cast<sint32>(query.referenceEdgeIndex) - 1) % referenceShape.edges.count();
		const uint32 referenceRightEdgeIndex = (query.referenceEdgeIndex + 1) % referenceShape.edges.count();

		const vec2<float32> localA = referenceShape.vertices[query.referenceEdgeIndex];
		const vec2<float32> localB = referenceShape.vertices[referenceRightEdgeIndex];
		const vec2<float32> a = localA * referenceBody.world;
		const vec2<float32> b = localB * referenceBody.world;

		Plane2D referencePlane;
		referencePlane.normal = query.axis;
		referencePlane.offset = (
			signedDistanceLPAB(localB * referenceBody.scale, localA * referenceBody.scale, referenceShape.centroid) +
			dot(referenceBody.position, referencePlane.normal)
		);

		Plane2D leftSidePlane;
		leftSidePlane.normal = perpendicularLeft(query.axis);
		leftSidePlane.offset = dot(a, leftSidePlane.normal);

		Plane2D rightSidePlane;
		rightSidePlane.normal = perpendicularRight(query.axis);
		rightSidePlane.offset = dot(b, rightSidePlane.normal);

		ClipFeature2D incidentEdge = findIncidentEdge(incidentBody, incidentShape, query.axis * rotationAngle<mat2x2<float32>>(-incidentBody.rotation));

		incidentEdge = clipEdge(incidentEdge, leftSidePlane, referenceLeftEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		incidentEdge = clipEdge(incidentEdge, rightSidePlane, referenceRightEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		manifold->normal = referencePlane.normal;

		uint8 vertexIndex = 0;

		while (vertexIndex < 2) {
			const ClipVertex2D& clipVertex = incidentEdge.edge[vertexIndex];
			const float32 separation = signedDistance(clipVertex.position, referencePlane);

			// Keep vertices that are below the reference plane.
			if (separation <= 0.0f) {
				// Move the contact point onto the reference plane.
				Contact2D& contact = manifold->contacts[manifold->contactCount];
				contact.position = clipVertex.position - referencePlane.normal * separation;
				contact.separation = separation;
				contact.id = clipVertex.id;

				manifold->contactCount += 1;
			}

			vertexIndex += 1;
		}

		return manifold->contactCount > 0;
	}

	bool ConvexHullConvexHull2D(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2DComponent& rigidBodyA, const RigidBody2DComponent& rigidBodyB, ContactManifold2D* manifold) {
		manifold->contactCount = 0;

		SATConvexHullConvexHullOutput2D output = SATConvexHullConvexHull2D(rigidBodyA, rigidBodyB, shapeA, shapeB);

		if (output.separation > 0.0f) {
			return false;
		}

		if (output.referenceIsA) {
			output.axis *= rotationAngle<mat2x2<float32>>(rigidBodyA.rotation);

			return createEdgeContact(output, shapeA, shapeB, rigidBodyA, rigidBodyB, manifold);
		}

		output.axis *= rotationAngle<mat2x2<float32>>(rigidBodyB.rotation);

		const bool result = createEdgeContact(output, shapeB, shapeA, rigidBodyB, rigidBodyA, manifold);

		manifold->normal *= -1.0f;

		uint8 contactIndex = 0;

		while (contactIndex < manifold->contactCount) {
			manifold->contacts[contactIndex].id.flip();

			contactIndex += 1;
		}

		return result;
	}
}