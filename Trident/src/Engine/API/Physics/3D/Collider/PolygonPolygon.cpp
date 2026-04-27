// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/Collider.hpp"
#include "Engine/API/Physics/3D/SAT3D.hpp"
#include "Engine/API/Physics/3D/SutherlandHodgman3D.hpp"

namespace Trident {
	static void createEdgeContact(const EdgeQuery& edgeQuery, const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB, ContactManifold3D* contactManifold) {
		const vec3<float32> centroidA = multiplyTransform({0.0f, 0.0f, 0.0f}, rigidBodyA.world);

		const Edge3D edgeA = hullA.edges[edgeQuery.edgeIndexA + 0];
		const Edge3D twinA = hullA.edges[edgeQuery.edgeIndexA + 1];

		const vec3<float32> p1 = multiplyTransform(vec3<float32>(hullA.vertexPositions[edgeA.originVertexIndex]), rigidBodyA.world);
		const vec3<float32> q1 = multiplyTransform(vec3<float32>(hullA.vertexPositions[twinA.originVertexIndex]), rigidBodyA.world);
		const vec3<float32> e1 = normalized(q1 - p1);

		const Edge3D edgeB = hullB.edges[edgeQuery.edgeIndexB + 0];
		const Edge3D twinB = hullB.edges[edgeQuery.edgeIndexB + 1];

		const vec3<float32> p2 = multiplyTransform(vec3<float32>(hullB.vertexPositions[edgeB.originVertexIndex]), rigidBodyB.world);
		const vec3<float32> q2 = multiplyTransform(vec3<float32>(hullB.vertexPositions[twinB.originVertexIndex]), rigidBodyB.world);
		const vec3<float32> e2 = normalized(q2 - p2);

		vec3<float32> normal = normalized(cross(e1, e2));

		if (dot(normal, p1 - centroidA) < 0.0f) {
			// Ensure the normal points towards B.
			normal = -normal;
		}

		// See https://math.stackexchange.com/a/2217845.
		const vec3<float32> ab = p2 - p1;

		const float32 nn = dot(normal, normal);
		const float32 tA = dot(cross(p2, normal), ab) / nn;
		const float32 tB = dot(cross(p1, normal), ab) / nn;

		const vec3<float32> closestA = p1 + e1 * tA;
		const vec3<float32> closestB = p2 + e2 * tB;

		contactManifold->normal = normal;
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = (closestA + closestB) * 0.5f;
		contactManifold->contacts[0].separation = edgeQuery.separation;

		// TODO(physics/3d/edge-contact-id)
	}

	static void createFaceContact(const FaceQuery& faceQuery, ContactManifold3D* contactManifold) {
		// TODO(lyah/scalar-math)
		// Last component of the SIMD register is not 0, which causes issues when solving.
		// Replace with scalar math.
		contactManifold->normal = {faceQuery.axis[0], faceQuery.axis[1], faceQuery.axis[2]};

		const Plane3D& referencePlane = faceQuery.referenceBody->facePlanes[faceQuery.referenceFaceIndex];

		pool<uint16, 8> referenceSidePlaneIndices;

		{
			uint32 edgeIndex = 0;

			while (edgeIndex < faceQuery.referenceHull->edges.count()) {
				const Edge3D edge = faceQuery.referenceHull->edges[edgeIndex];

				if (edge.faceIndex == faceQuery.referenceFaceIndex) {
					uint32 otherEdgeIndex = edgeIndex;

					if (edgeIndex & 1) {
						// If it's a twin edge, add the edge preceeding it (base).
						otherEdgeIndex -= 1;
					}
					else {
						// If it's a base edge, add the edge following it (twin).
						otherEdgeIndex += 1;
					}

					referenceSidePlaneIndices.add(faceQuery.referenceHull->edges[otherEdgeIndex].faceIndex);
				}

				edgeIndex += 1;
			}
		}

		// Rotate by the inverse of the incident body's orientation to get a normal that can be used as a support direction.
		// TODO: Invert the orientation before multiplying?
		const vec3<float32> referenceFaceDirection = referencePlane.worldNormal * faceQuery.incidentBody->orientation;

		index incidentFaceIndex = max<index>();

		// Find the index of the incident face.
		{
			float32 angle;
			float32 minAngle = max<float32>();
			index faceIndex = 0;

			while (faceIndex < faceQuery.incidentBody->facePlanes.count()) {
				angle = dot(faceQuery.incidentBody->facePlanes[faceIndex].localNormal, referenceFaceDirection);

				if (angle < minAngle) {
					minAngle = angle;
					incidentFaceIndex = faceIndex;
				}

				faceIndex += 1;
			}
		}

		const Face3D& incidentFace = faceQuery.incidentHull->faces[incidentFaceIndex];

		pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> incidentFaceVertices{};

		uint16 edgeIndex = 0;

		while (edgeIndex < incidentFace.edgeIndexRange.count) {
			const Edge3D edge = faceQuery.incidentHull->edges[faceQuery.incidentHull->faceEdgeIndices[incidentFace.edgeIndexRange.offset + edgeIndex]];

			ClipVertex3D clipVertex{};
			clipVertex.position = multiplyTransform(vec3<float32>(faceQuery.incidentHull->vertexPositions[edge.originVertexIndex]), faceQuery.incidentBody->world);
			clipVertex.separation = 0.0f;
			clipVertex.id.referenceEdgeAIndex = max<uint16>();
			clipVertex.id.referenceEdgeBIndex = max<uint16>();
			clipVertex.id.referenceFaceAIndex = static_cast<sint16>(edge.faceIndex - 1) % static_cast<uint16>(faceQuery.incidentHull->faces.count());
			clipVertex.id.referenceFaceBIndex = edge.faceIndex;
			clipVertex.id.incidentEdgeAIndex = max<uint16>();
			clipVertex.id.incidentEdgeBIndex = max<uint16>();
			clipVertex.id.incidentFaceAIndex = edge.faceIndex;
			clipVertex.id.incidentFaceBIndex = (edge.faceIndex + 1) % static_cast<uint16>(faceQuery.incidentHull->faces.count());

			incidentFaceVertices.add(clipVertex);

			edgeIndex += 1;
		}

		pool<ClipVertex3D, 8> contactPool = SutherlandHodgman3D(faceQuery, faceQuery.referenceFaceIndex, incidentFaceVertices, referenceSidePlaneIndices);

		// Discard contacts that are above the reference plane, and move the rest onto the plane.
		// This helps with coherence when reducing the number of contacts.
		{
			sint8 contactIndex = static_cast<sint8>(contactPool.count()) - 1;

			while (contactIndex >= 0) {
				ClipVertex3D& contact = contactPool[contactIndex];

				const float32 separation = signedDistanceWorld(referencePlane, contact.position);

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
				const ClipVertex3D& contact = contactPool[contactIndex];

				contactManifold->contacts[contactIndex].position = contact.position;
				contactManifold->contacts[contactIndex].separation = contact.separation;
				contactManifold->contacts[contactIndex].id = contact.id;

				contactIndex += 1;
			}

			return;
		}

		contactManifold->contactCount = 4;

		// Select the first contact as the support point towards a fixed direction to avoid jitter.
		{
			// TODO(constexpr)
			static const vec3<float32> n = {1.0f, 1.0f, 1.0f};

			index contactIndex = 0;
			index maxAngleContactIndex = max<index>();
			float32 angle;
			float32 maxAngle = min<float32>();

			while (contactIndex < contactPool.count()) {
				angle = dot(contactPool[contactIndex].position, n);

				if (angle > maxAngle) {
					maxAngle = angle;
					maxAngleContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			contactManifold->contacts[0].position = contactPool[maxAngleContactIndex].position;
			contactManifold->contacts[0].separation = contactPool[maxAngleContactIndex].separation;
			contactManifold->contacts[0].id = contactPool[maxAngleContactIndex].id;

			contactPool.remove(maxAngleContactIndex);
		}

		// The second contact is the farthest one from the first contact.
		{
			const vec3<float32> a = contactManifold->contacts[0].position;

			index contactIndex = 0;
			index maxDistanceContactIndex = max<index>();
			float32 distance;
			float32 maxDistance = min<float32>();

			while (contactIndex < contactPool.count()) {
				distance = distanceSquared(a, contactPool[contactIndex].position);

				if (distance > maxDistance) {
					maxDistance = distance;
					maxDistanceContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			contactManifold->contacts[1].position = contactPool[maxDistanceContactIndex].position;
			contactManifold->contacts[1].separation = contactPool[maxDistanceContactIndex].separation;
			contactManifold->contacts[1].id = contactPool[maxDistanceContactIndex].id;

			contactPool.remove(maxDistanceContactIndex);
		}

		// The third contact maximizes the area of the current manifold.
		{
			const vec3<float32> a = contactManifold->contacts[0].position;
			const vec3<float32> b = contactManifold->contacts[1].position;

			index contactIndex = 0;
			index maxAreaContactIndex = max<index>();
			float32 area;
			float32 maxArea = min<float32>();
			vec3<float32> c;

			while (contactIndex < contactPool.count()) {
				c = contactPool[contactIndex].position;

				area = dot(cross(a - c, b - c) * 0.5f, contactManifold->normal);

				if (area > maxArea) {
					maxArea = area;
					maxAreaContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			contactManifold->contacts[2].position = contactPool[maxAreaContactIndex].position;
			contactManifold->contacts[2].separation = contactPool[maxAreaContactIndex].separation;
			contactManifold->contacts[2].id = contactPool[maxAreaContactIndex].id;

			contactPool.remove(maxAreaContactIndex);
		}

		// The fourth contact adds the largest area to the current manifold.
		{
			const vec3<float32> a = contactManifold->contacts[0].position;
			const vec3<float32> b = contactManifold->contacts[1].position;
			const vec3<float32> c = contactManifold->contacts[2].position;

			index contactIndex = 0;
			index minAreaContactIndex = max<index>();
			float32 area;
			float32 minArea = max<float32>();
			vec3<float32> d;

			while (contactIndex < contactPool.count()) {
				d = contactPool[contactIndex].position;

				area = dot(cross(a - d, b - d) * 0.5f, contactManifold->normal);

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

			contactManifold->contacts[3].position = contactPool[minAreaContactIndex].position;
			contactManifold->contacts[3].separation = contactPool[minAreaContactIndex].separation;
			contactManifold->contacts[3].id = contactPool[minAreaContactIndex].id;

			contactPool.remove(minAreaContactIndex);
		}

		// TODO(debug): Swap contacts for LINE_STRIP visualization.
		std::swap(contactManifold->contacts[1], contactManifold->contacts[2]);
	}

	bool PolygonPolygon(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB, ContactManifold3D* contactManifold) {
		SATOutput3D sat = SAT3D(rigidBodyA, rigidBodyB, hullA, hullB);

		if (!sat.overlap) {
			return false;
		}

		if (sat.edgeQuery.separation > sat.faceQueryA.separation && sat.edgeQuery.separation > sat.faceQueryB.separation) {
			createEdgeContact(sat.edgeQuery, rigidBodyA, rigidBodyB, hullA, hullB, contactManifold);

			return true;
		}

		if (sat.faceQueryA.separation >= sat.faceQueryB.separation) {
			// TODO(physics/3d/separating-axis-orientation): The axis should be rotated by the base orientation. Why is the inverse needed?
			sat.faceQueryA.axis *= inverse(sat.faceQueryA.referenceBody->orientation);

			createFaceContact(sat.faceQueryA, contactManifold);

			return true;
		}

		// TODO(physics/3d/separating-axis-orientation): The axis should be rotated by the base orientation. Why is the inverse needed?
		sat.faceQueryB.axis *= inverse(sat.faceQueryB.referenceBody->orientation);

		createFaceContact(sat.faceQueryB, contactManifold);

		contactManifold->normal *= -1.0f;

		index contactIndex = 0;

		while (contactIndex < contactManifold->contactCount) {
			contactManifold->contacts[contactIndex].id.flip();

			contactIndex += 1;
		}

		return true;
	}
}