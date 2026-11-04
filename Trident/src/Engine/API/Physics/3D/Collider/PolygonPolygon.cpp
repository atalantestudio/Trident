// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/Collider.hpp"
#include "Engine/API/Physics/3D/SAT3D.hpp"
#include "Engine/API/Physics/3D/SutherlandHodgman3D.hpp"

namespace Trident {
	// See https://math.stackexchange.com/a/2217845.
	static void createEdgeContact(const EdgeQuery& edgeQuery, const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB, ContactManifold3D* contactManifold) {
		Transform3D transformA = rigidBodyA.getTransform();
		transformA.position += hullA.centroid;

		Transform3D transformB = rigidBodyB.getTransform();
		transformB.position += hullB.centroid;

		const vec3<float32> centroidA = hullA.centroid * rigidBodyA.world;

		const Edge3D edgeA = hullA.edges[edgeQuery.edgeIndexA];
		const Edge3D twinA = hullA.edges[(edgeQuery.edgeIndexA + 1) % hullA.edges.count()];

		const vec3<float32> a = hullA.vertices[edgeA.originVertexIndex] * transformA;
		const vec3<float32> ab = hullA.vertices[twinA.originVertexIndex] * transformA - a;

		const Edge3D edgeB = hullB.edges[edgeQuery.edgeIndexB];
		const Edge3D twinB = hullB.edges[(edgeQuery.edgeIndexB + 1) % hullB.edges.count()];

		const vec3<float32> c = hullB.vertices[edgeB.originVertexIndex] * transformB;
		const vec3<float32> cd = hullB.vertices[twinB.originVertexIndex] * transformB - c;

		const vec3<float32> ac = c - a;
		const vec3<float32> normal = cross(ab, cd);
		const float32 normalSquaredLength = lengthSquared(normal);

		const float32 tA = dot(cross(cd, normal), ac) / normalSquaredLength;
		const float32 tB = dot(cross(ab, normal), ac) / normalSquaredLength;

		const vec3<float32> closestA = a + ab * tA;
		const vec3<float32> closestB = c + cd * tB;

		contactManifold->normal = normal;

		// Ensure the normal points towards B.
		if (dot(contactManifold->normal, a - centroidA) < 0.0f) {
			contactManifold->normal *= -1.0f;
		}

		contactManifold->contactCount = 1;

		Contact3D& contact = contactManifold->contacts[0];
		contact.position = lerp(closestA, closestB, 0.5f);
		contact.separation = edgeQuery.separation;

		// TODO(physics/3d/edge-contact-id)
	}

	static Plane3D findIncidentFace(const Hull3D& incidentHull, vec3<float32> d) {
		float32 minAngle = max<float32>();
		index incidentFaceIndex = 0;
		index faceIndex = 0;

		while (faceIndex < incidentHull.faces.count()) {
			const float32 angle = dot(incidentHull.faces[faceIndex].normal, d);

			if (angle < minAngle) {
				minAngle = angle;
				incidentFaceIndex = faceIndex;
			}

			faceIndex += 1;
		}

		return incidentHull.faces[incidentFaceIndex];
	}

	static void createFaceContact(const FaceQuery& faceQuery, const RigidBody3DComponent& referenceBody, const RigidBody3DComponent& incidentBody, const Hull3D& referenceHull, const Hull3D& incidentHull, ContactManifold3D* contactManifold) {
		contactManifold->normal = faceQuery.referenceFace.normal;

		Transform3D incidentTransform = incidentBody.getTransform();
		incidentTransform.position += incidentHull.centroid;

		// Rotate by the inverse of the incident body's orientation to get a normal that can be used as a support direction.
		const Plane3D& incidentFace = findIncidentFace(incidentHull, faceQuery.referenceFace.normal);

		pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> incidentFaceVertices{};
		uint16 edgeIndex = 0;

		while (edgeIndex < incidentFace.edgeIndexRange.count) {
			const Edge3D edge = incidentHull.edges[incidentHull.faceEdgeIndices[incidentFace.edgeIndexRange.offset + edgeIndex]];

			ClipVertex3D clipVertex;
			clipVertex.position = incidentHull.vertices[edge.originVertexIndex];
			clipVertex.separation = 0.0f;
			clipVertex.id.referenceEdgeAIndex = max<uint16>();
			clipVertex.id.referenceEdgeBIndex = max<uint16>();
			clipVertex.id.referenceFaceAIndex = static_cast<sint16>(edge.faceIndex - 1) % static_cast<uint16>(incidentHull.faces.count());
			clipVertex.id.referenceFaceBIndex = edge.faceIndex;
			clipVertex.id.incidentEdgeAIndex = max<uint16>();
			clipVertex.id.incidentEdgeBIndex = max<uint16>();
			clipVertex.id.incidentFaceAIndex = edge.faceIndex;
			clipVertex.id.incidentFaceBIndex = (edge.faceIndex + 1) % static_cast<uint16>(incidentHull.faces.count());

			incidentFaceVertices.add(clipVertex);

			edgeIndex += 1;
		}

		pool<ClipVertex3D, 8> contactPool = SutherlandHodgman3D(faceQuery.transform, referenceHull, faceQuery.referenceFace, faceQuery.referenceFaceIndex, incidentFaceVertices);

		// Discard contacts that are above the reference plane, and move the rest onto the plane.
		// This helps with coherence when reducing the number of contacts.
		{
			sint8 contactIndex = static_cast<sint8>(contactPool.count()) - 1;

			while (contactIndex >= 0) {
				ClipVertex3D& contact = contactPool[contactIndex];

				const float32 separation = signedDistance(faceQuery.referenceFace, contact.position);

				if (separation > 0.0f) {
					contactPool.remove(contactIndex);

					contactIndex -= 1;

					continue;
				}

				contact.position -= contactManifold->normal * separation;
				contact.separation = separation;

				contactIndex -= 1;
			}
		}

		if (contactPool.count() <= 4) {
			// No need to reduce the contact pool.
			// Just copy the clipped vertices to the contact manifold.

			contactManifold->contactCount = static_cast<uint8>(contactPool.count());

			index contactIndex = 0;

			while (contactIndex < contactPool.count()) {
				const ClipVertex3D& vertex = contactPool[contactIndex];

				Contact3D& contact = contactManifold->contacts[contactIndex];
				contact.position = vertex.position * incidentTransform;
				contact.separation = vertex.separation;
				contact.id = vertex.id;

				contactIndex += 1;
			}

			return;
		}

		contactManifold->contactCount = 4;

		index contactIndex = 0;

		// Select the first contact as the support point towards a fixed direction to avoid jitter.
		{
			static constexpr vec3<float32> d = {1.0f, 1.0f, 1.0f};

			index maxAngleContactIndex = max<index>();
			float32 maxAngle = min<float32>();

			while (contactIndex < contactPool.count()) {
				const float32 angle = dot(contactPool[contactIndex].position, d);

				if (angle > maxAngle) {
					maxAngle = angle;
					maxAngleContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			const ClipVertex3D& vertex = contactPool[maxAngleContactIndex];

			Contact3D& contact = contactManifold->contacts[0];
			contact.position = vertex.position;
			contact.separation = vertex.separation;
			contact.id = vertex.id;

			contactPool.remove(maxAngleContactIndex);
		}

		// The second contact is the farthest one from the first contact.
		{
			const vec3<float32> a = contactManifold->contacts[0].position;

			index maxDistanceContactIndex = max<index>();
			float32 maxDistance = min<float32>();

			contactIndex = 0;

			while (contactIndex < contactPool.count()) {
				const float32 distance = distanceSquared(a, contactPool[contactIndex].position);

				if (distance > maxDistance) {
					maxDistance = distance;
					maxDistanceContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			const ClipVertex3D& vertex = contactPool[maxDistanceContactIndex];

			Contact3D& contact = contactManifold->contacts[1];
			contact.position = vertex.position;
			contact.separation = vertex.separation;
			contact.id = vertex.id;

			contactPool.remove(maxDistanceContactIndex);
		}

		// The third contact maximizes the area of the current manifold.
		{
			const vec3<float32> a = contactManifold->contacts[0].position;
			const vec3<float32> b = contactManifold->contacts[1].position;

			index maxAreaContactIndex = max<index>();
			float32 maxArea = min<float32>();

			contactIndex = 0;

			while (contactIndex < contactPool.count()) {
				const vec3<float32> c = contactPool[contactIndex].position;
				const float32 area = dot(cross(a - c, b - c) * 0.5f, contactManifold->normal);

				if (area > maxArea) {
					maxArea = area;
					maxAreaContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			const ClipVertex3D& vertex = contactPool[maxAreaContactIndex];

			Contact3D& contact = contactManifold->contacts[2];
			contact.position = vertex.position;
			contact.separation = vertex.separation;
			contact.id = vertex.id;

			contactPool.remove(maxAreaContactIndex);
		}

		// The fourth contact adds the largest area to the current manifold.
		{
			const vec3<float32> a = contactManifold->contacts[0].position;
			const vec3<float32> b = contactManifold->contacts[1].position;
			const vec3<float32> c = contactManifold->contacts[2].position;

			index minAreaContactIndex = max<index>();
			float32 minArea = max<float32>();

			contactIndex = 0;

			while (contactIndex < contactPool.count()) {
				const vec3<float32> d = contactPool[contactIndex].position;

				float32 area = dot(cross(a - d, b - d) * 0.5f, contactManifold->normal);

				if (area < minArea) {
					minArea = area;
					minAreaContactIndex = contactIndex;
				}

				area = dot(cross(b - d, c - d) * 0.5f, contactManifold->normal);

				if (area < minArea) {
					minArea = area;
					minAreaContactIndex = contactIndex;
				}

				area = dot(cross(c - d, a - d) * 0.5f, contactManifold->normal);

				if (area < minArea) {
					minArea = area;
					minAreaContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			const ClipVertex3D& vertex = contactPool[minAreaContactIndex];

			Contact3D& contact = contactManifold->contacts[3];
			contact.position = vertex.position;
			contact.separation = vertex.separation;
			contact.id = vertex.id;

			contactPool.remove(minAreaContactIndex);
		}

		contactIndex = 0;

		while (contactIndex < contactManifold->contactCount) {
			contactManifold->contacts[contactIndex].position *= incidentTransform;

			contactIndex += 1;
		}
	}

	bool PolygonPolygon(const RigidBodyPair3D& pair, ContactManifold3D* contactManifold) {
		DEBUG_ASSERT(pair.hullA->type == Hull3DType::POLYGON);
		DEBUG_ASSERT(pair.hullB->type == Hull3DType::POLYGON);

		const SATOutput3D sat = SAT3D(*pair.rigidBodyA, *pair.rigidBodyB, *pair.hullA, *pair.hullB);

		if (!sat.overlap) {
			return false;
		}

		if (sat.edgeQuery.separation >= sat.faceQueryA.separation && sat.edgeQuery.separation >= sat.faceQueryB.separation) {
			createEdgeContact(sat.edgeQuery, *pair.rigidBodyA, *pair.rigidBodyB, *pair.hullA, *pair.hullB, contactManifold);

			return true;
		}

		if (sat.faceQueryA.separation >= sat.faceQueryB.separation) {
			createFaceContact(sat.faceQueryA, *pair.rigidBodyA, *pair.rigidBodyB, *pair.hullA, *pair.hullB, contactManifold);

			return true;
		}

		createFaceContact(sat.faceQueryB, *pair.rigidBodyB, *pair.rigidBodyA, *pair.hullB, *pair.hullA, contactManifold);

		// TODO(physics/3d/polygon-polygon): Keep normal inversion?
		contactManifold->normal *= -1.0f;

		index contactIndex = 0;

		while (contactIndex < contactManifold->contactCount) {
			contactManifold->contacts[contactIndex].id.flip();

			contactIndex += 1;
		}

		return true;
	}
}