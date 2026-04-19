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

	bool PolygonPolygon(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB, ContactManifold3D* contactManifold) {
		const SATOutput3D sat = SAT3D(rigidBodyA, rigidBodyB, hullA, hullB);

		if (sat.separation > 0.0f) {
			return false;
		}

		// TODO(physics/3d/edge-contact): Face contacts are assumed for now. Add support for edge contacts.

		contactManifold->normal = sat.axis;

		// Find the index of the incident face.
		uint32 incidentFaceIndex = max<uint32>();

		const std::vector<Face3D> referenceSideFaces = findReferenceSideFaces(*sat.referenceHull, sat.referenceFaceIndex);
		const vec3<float32> incidentFaceDirection = sat.axis * inverse(sat.incidentBody->orientation);

		{
			float32 angle;
			float32 minAngle = max<float32>();
			uint32 faceIndex = 0;

			while (faceIndex < sat.incidentHull->faces.count()) {
				angle = dot(vec3<float32>(sat.incidentHull->faces[faceIndex].normal), incidentFaceDirection);

				if (angle < minAngle) {
					minAngle = angle;
					incidentFaceIndex = faceIndex;
				}

				faceIndex += 1;
			}
		}

		const Face3D& incidentFace = sat.incidentHull->faces[incidentFaceIndex];

		pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> incidentFaceVertices{};

		uint16 edgeIndex = 0;

		while (edgeIndex < incidentFace.edgeIndexRange.count) {
			const Edge3D edge = sat.incidentHull->edges[incidentFace.edgeIndexRange.offset + edgeIndex];

			ClipVertex3D clipVertex{};
			clipVertex.position = multiplyTransform(vec3<float32>(sat.incidentHull->vertexPositions[edge.originVertexIndex]), sat.incidentBody->world);

			incidentFaceVertices.add(clipVertex);

			edgeIndex += 1;
		}

		pool<ClipVertex3D, 8> contactPool = SutherlandHodgman3D(sat, incidentFaceVertices, referenceSideFaces);

		// Move the contacts onto the reference face.
		{
			uint8 contactIndex = 0;

			while (contactIndex < contactPool.count()) {
				contactPool[contactIndex].position -= sat.axis * sat.separation;

				contactIndex += 1;
			}
		}

		if (contactPool.count() <= 4) {
			// No need to reduce the contact pool.

			index contactIndex = 0;

			while (contactIndex < contactPool.count()) {
				contactManifold->contacts[contactIndex].position = contactPool[contactIndex].position;
				contactManifold->contacts[contactIndex].separation = sat.separation;

				contactIndex += 1;
			}

			contactManifold->contactCount = static_cast<uint8>(contactPool.count());

			return true;
		}

		contactManifold->contactCount = 4;

		/// Contact point reduction.
		/// See https://media.steampowered.com/apps/valve/2015/DirkGregorius_Contacts.pdf.

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
			contactManifold->contacts[0].separation = sat.separation;

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
			contactManifold->contacts[1].separation = sat.separation;

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

				area = dot(cross(a - c, b - c) * 0.5f, sat.axis);

				if (area > maxArea) {
					maxArea = area;
					maxAreaContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			contactManifold->contacts[2].position = contactPool[maxAreaContactIndex].position;
			contactManifold->contacts[2].separation = sat.separation;

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

				area = dot(cross(a - d, b - d) * 0.5f, sat.axis);

				if (area < minArea) {
					minArea = area;
					minAreaContactIndex = contactIndex;
				}

				area = dot(cross(b - d, c - d) * 0.5f, sat.axis);

				if (area < minArea) {
					minArea = area;
					minAreaContactIndex = contactIndex;
				}

				area = dot(cross(c - d, a - d) * 0.5f, sat.axis);

				if (area < minArea) {
					minArea = area;
					minAreaContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			contactManifold->contacts[3].position = contactPool[minAreaContactIndex].position;
			contactManifold->contacts[3].separation = sat.separation;

			contactPool.remove(minAreaContactIndex);
		}

		// TODO(debug): Swap contacts for LINE_STRIP visualization.
		std::swap(contactManifold->contacts[1], contactManifold->contacts[2]);

		return true;
	}
}