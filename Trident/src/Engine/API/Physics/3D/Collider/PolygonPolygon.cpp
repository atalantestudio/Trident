// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/Collider.hpp"
#include "Engine/API/Physics/3D/FaceFeature3D.hpp"
#include "Engine/API/Physics/3D/SAT3D.hpp"
#include "Engine/API/Physics/3D/SutherlandHodgman3D.hpp"

namespace Trident {
	static std::vector<Face3D> findReferenceSideFaces(const Hull3D& referenceHull, uint32 referenceFaceIndex) {
		std::vector<Face3D> sideFaces;

		uint32 edgeIndex = 0;

		while (edgeIndex < referenceHull.edges.count()) {
			const Edge3D edge = referenceHull.edges[edgeIndex];

			if (edge.faceIndex == referenceFaceIndex) {
				uint32 otherEdgeIndex = edgeIndex;

				if (edgeIndex & 1) {
					// If it's a twin edge, add the edge preceeding it (base).
					otherEdgeIndex -= 1;
				}
				else {
					// If it's a base edge, add the edge following it (twin).
					otherEdgeIndex += 1;

					DEBUG_ASSERT(otherEdgeIndex < referenceHull.edges.count());
				}

				const Face3D& sideFace = referenceHull.faces[referenceHull.edges[otherEdgeIndex].faceIndex];

				sideFaces.emplace_back(sideFace);
			}

			edgeIndex += 1;
		}

		return sideFaces;
	}

	static void createEdgeContact(const EdgeQuery& edgeQuery, const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB, ContactManifold3D* contactManifold) {
		// TODO(physics/3d/edge-contact)
		BREAKPOINT();

		contactManifold->contactCount = 0;
	}

	static void createFaceContact(const FaceQuery& faceQuery, ContactManifold3D* contactManifold) {
		// TODO(lyah/scalar-math)
		// Last component of the SIMD register is not 0, which causes issues when solving.
		// Replace with scalar math.
		contactManifold->normal = {faceQuery.axis[0], faceQuery.axis[1], faceQuery.axis[2]};

		const Face3D& referenceFace = faceQuery.referenceHull->faces[faceQuery.referenceFaceIndex];
		const Face3D F = transformFace(referenceFace, faceQuery.referenceBody->world);

		const std::vector<Face3D> referenceSideFaces = findReferenceSideFaces(*faceQuery.referenceHull, faceQuery.referenceFaceIndex);

		// Rotate by the inverse of the incident body's orientation to get a normal that can be used as a support direction.
		const vec3<float32> referenceFaceDirection = vec3<float32>(F.normal) * inverse(faceQuery.incidentBody->orientation);

		index incidentFaceIndex = max<index>();

		// Find the index of the incident face.
		{
			float32 angle;
			float32 minAngle = max<float32>();
			index faceIndex = 0;

			while (faceIndex < faceQuery.incidentHull->faces.count()) {
				angle = dot(vec3<float32>(faceQuery.incidentHull->faces[faceIndex].normal), referenceFaceDirection);

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
			const Edge3D edge = faceQuery.incidentHull->edges[incidentFace.edgeIndexRange.offset + edgeIndex];

			ClipVertex3D clipVertex{};
			clipVertex.position = multiplyTransform(vec3<float32>(faceQuery.incidentHull->vertexPositions[edge.originVertexIndex]), faceQuery.incidentBody->world);

			incidentFaceVertices.add(clipVertex);

			edgeIndex += 1;
		}

		pool<ClipVertex3D, 8> contactPool = SutherlandHodgman3D(faceQuery, incidentFaceVertices, referenceSideFaces);

		// Discard contacts that are above the reference plane, and move the rest onto the plane.
		// This helps with coherence when reducing the number of contacts.
		{
			sint8 contactIndex = static_cast<sint8>(contactPool.count()) - 1;

			while (contactIndex >= 0) {
				ClipVertex3D& contact = contactPool[contactIndex];

				const float32 separation = signedDistanceFP(vec3<float32>(F.normal), F.offset, contact.position);

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

				contactIndex += 1;
			}

			return;
		}

		// Reduce contact points.
		// See https://media.steampowered.com/apps/valve/2015/DirkGregorius_Contacts.pdf.

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

		if (sat.faceQueryA.separation > sat.edgeQuery.separation && sat.faceQueryB.separation > sat.edgeQuery.separation) {
			if (sat.faceQueryA.separation >= sat.faceQueryB.separation) {
				// TODO(physics/3d/separating-axis-orientation): The axis should be rotated by the base orientation. Why is the inverse needed?
				sat.faceQueryA.axis *= inverse(sat.faceQueryA.referenceBody->orientation);

				createFaceContact(sat.faceQueryA, contactManifold);
			}
			else {
				// TODO(physics/3d/separating-axis-orientation): The axis should be rotated by the base orientation. Why is the inverse needed?
				sat.faceQueryB.axis *= inverse(sat.faceQueryB.referenceBody->orientation);

				createFaceContact(sat.faceQueryB, contactManifold);
			}
		}
		else {
			createEdgeContact(sat.edgeQuery, rigidBodyA, rigidBodyB, hullA, hullB, contactManifold);
		}

		return true;
	}
}