// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider2D.hpp"
#include "Engine/API/Physics/2D/Feature2D.hpp"
#include "Engine/API/Physics/2D/SAT2D.hpp"

namespace Trident {
	// Returns the farthest edge along `d` in world-space.
	static ClipFeature2D incidentEdgeSupport(const RigidBody2D& rigidBody, const Shape2D& shape, vec2<float32> d) {
		const uint16 vertexCount = shape.hull->vertexCount;
		const vec2<float32>* vertices = shape.hull->getVertices();

		ClipFeature2D edge{};
		edge.vertexCount = 2;

		float32 maxAngle = min<float32>();
		uint32 edgeIndexA = 0;
		uint32 edgeIndexB = 0;
		uint16 indexA = 0;

		while (indexA < vertexCount) {
			const uint32 indexB = (indexA + 1) % vertexCount;
			const vec2<float32> a = vertices[indexA];
			const vec2<float32> b = vertices[indexB];
			const vec2<float32> normal = perpendicularLeft(b - a);
			const float32 angle = dot(normal, d);

			if (angle > maxAngle) {
				edge.edge[0].position = a;
				edge.edge[1].position = b;

				maxAngle = angle;
				edgeIndexA = indexA;
				edgeIndexB = indexB;
			}

			indexA += 1;
		}

		ClipVertex2D& a = edge.edge[0];
		a.position *= rigidBody.transform;
		a.id.referenceEdgeAIndex = ContactID2D::NULL_EDGE_INDEX;
		a.id.referenceEdgeBIndex = ContactID2D::NULL_EDGE_INDEX;
		a.id.incidentEdgeAIndex = (static_cast<sint32>(edgeIndexA) - 1) % vertexCount;
		a.id.incidentEdgeBIndex = edgeIndexA;

		ClipVertex2D& b = edge.edge[1];
		b.position *= rigidBody.transform;
		b.id.referenceEdgeAIndex = ContactID2D::NULL_EDGE_INDEX;
		b.id.referenceEdgeBIndex = ContactID2D::NULL_EDGE_INDEX;
		b.id.incidentEdgeAIndex = edgeIndexA;
		b.id.incidentEdgeBIndex = edgeIndexB;

		return edge;
	}

	// Clips an edge against a 2D plane and returns a new edge formed by the clipped vertices.
	static ClipFeature2D clipEdge(const ClipFeature2D& edge, Plane2D plane, uint32 edgeIndex) {
		const ClipVertex2D a = edge.edge[0];
		const ClipVertex2D b = edge.edge[1];

		const float32 dA = signedDistance(a.position, plane);
		const float32 dB = signedDistance(b.position, plane);

		ClipFeature2D clippedEdge{};

		if (dA <= 0.0f) {
			// Keep vertices that are below the line.
			clippedEdge.edge[clippedEdge.vertexCount] = a;
			clippedEdge.vertexCount += 1;
		}

		if (dB <= 0.0f) {
			// Keep vertices that are below the line.
			clippedEdge.edge[clippedEdge.vertexCount] = b;
			clippedEdge.vertexCount += 1;
		}

		if (dA * dB < 0.0f) {
			// If the vertices are on different sides of the plane, insert the intersection point.
			DEBUG_ASSERT(clippedEdge.vertexCount < 2);

			const float32 t = dA / (dA - dB);

			ClipVertex2D& c = clippedEdge.edge[clippedEdge.vertexCount];
			c.position = lerp(a.position, b.position, t);

			if (dA > 0.0f) {
				c.id = a.id;
				c.id.referenceEdgeAIndex = edgeIndex;
				c.id.referenceEdgeBIndex = ContactID2D::NULL_EDGE_INDEX;
			}
			else {
				c.id = b.id;
				c.id.incidentEdgeAIndex = edgeIndex;
				c.id.incidentEdgeBIndex = ContactID2D::NULL_EDGE_INDEX;
			}

			clippedEdge.vertexCount += 1;
		}

		DEBUG_ASSERT(clippedEdge.vertexCount <= 2);

		return clippedEdge;
	}

	bool createEdgeContact(const SATEdgeQuery2D& query, const Shape2D& referenceShape, const Shape2D& incidentShape, const RigidBody2D& referenceBody, const RigidBody2D& incidentBody, ContactManifold2D* contactManifold) {
		contactManifold->contactCount = 0;

		const vec2<float32>* vertices = referenceShape.hull->getVertices();

		const vec2<float32> a = vertices[query.referenceEdgeIndexA] * referenceBody.transform;
		const vec2<float32> b = vertices[query.referenceEdgeIndexB] * referenceBody.transform;

		Plane2D plane;
		plane.normal = edgeNormalClockwise(a, b);
		plane.offset = dot(plane.normal, a);

		ClipFeature2D incidentEdge = incidentEdgeSupport(incidentBody, incidentShape, -query.referencePlane.normal);

		Plane2D leftSidePlane;
		leftSidePlane.normal = perpendicularLeft(plane.normal);
		leftSidePlane.offset = dot(leftSidePlane.normal, a);

		incidentEdge = clipEdge(incidentEdge, leftSidePlane, (static_cast<sint32>(query.referenceEdgeIndexA) - 1) % referenceShape.hull->vertexCount);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		Plane2D rightSidePlane;
		rightSidePlane.normal = perpendicularRight(plane.normal);
		rightSidePlane.offset = dot(rightSidePlane.normal, b);

		incidentEdge = clipEdge(incidentEdge, rightSidePlane, query.referenceEdgeIndexB);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		contactManifold->normal = plane.normal;

		uint32 vertexIndex = 0;

		while (vertexIndex < incidentEdge.vertexCount) {
			const ClipVertex2D& clipVertex = incidentEdge.edge[vertexIndex];
			const float32 separation = signedDistance(clipVertex.position, plane);

			// Keep vertices that are below the reference plane.
			if (separation <= 0.0f) {
				Contact2D& contact = contactManifold->contacts[contactManifold->contactCount];

				// TODO(physics/2d): Moving a contact onto the reference plane might push it outside of the incident body.
				contact.position = clipVertex.position - plane.normal * separation;

				contact.separation = separation;
				contact.id = clipVertex.id;

				contactManifold->contactCount += 1;
			}

			vertexIndex += 1;
		}

		DEBUG_ASSERT(contactManifold->contactCount > 0);

		return true;
	}

	bool ConvexHullConvexHull2D(const Shape2D& shapeA, const Shape2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* contactManifold) {
		DEBUG_ASSERT(shapeA.type == ShapeType2D::CONVEX_HULL);
		DEBUG_ASSERT(shapeB.type == ShapeType2D::CONVEX_HULL);

		const SATOutput2D sat = SAT2D(rigidBodyA, rigidBodyB, shapeA, shapeB);

		if (!sat.overlap) {
			return false;
		}

		if (sat.edgeQueryA.separation >= sat.edgeQueryB.separation) {
			return createEdgeContact(sat.edgeQueryA, shapeA, shapeB, rigidBodyA, rigidBodyB, contactManifold);
		}

		if (!createEdgeContact(sat.edgeQueryB, shapeB, shapeA, rigidBodyB, rigidBodyA, contactManifold)) {
			return false;
		}

		// TODO(physics/2d/convex-hull-convex-hull): Keep normal inversion?
		contactManifold->normal *= -1.0f;

		uint16 contactIndex = 0;

		while (contactIndex < contactManifold->contactCount) {
			contactManifold->contacts[contactIndex].id.flip();

			contactIndex += 1;
		}

		return true;
	}
}