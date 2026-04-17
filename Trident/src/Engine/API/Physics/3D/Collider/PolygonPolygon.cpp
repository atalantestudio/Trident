// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/Collider.hpp"
#include "Engine/API/Physics/3D/FaceFeature3D.hpp"
#include "Engine/API/Physics/3D/SAT3D.hpp"

namespace Trident {
	static uint32 findIncidentFaceIndex(const Hull3D& incidentHull, vec3<float32> referenceFaceNormal) {
		float32 minValue = max<float32>();
		uint32 minFaceIndex = 0;
		uint32 faceIndex = 0;

		while (faceIndex < incidentHull.faces.count()) {
			const Face3D face = incidentHull.faces[faceIndex];
			const float32 value = dot(referenceFaceNormal, vec3<float32>(face.normal));

			if (value < minValue) {
				minValue = value;
				minFaceIndex = faceIndex;
			}

			faceIndex += 1;
		}

		return minFaceIndex;
	}

	static std::vector<vec3<float32>> findFaceVertices(const Hull3D& hull, uint32 faceIndex) {
		std::vector<vec3<float32>> vertices;
		uint32 edgeIndex = 0;

		while (edgeIndex < hull.edges.count()) {
			const Edge3D edge = hull.edges[edgeIndex];

			if (edge.faceIndex == faceIndex) {
				const vec3<float32> baseVertex = hull.vertexPositions[edge.originVertexIndex];

				vertices.emplace_back(baseVertex);
			}

			edgeIndex += 1;
		}

		return vertices;
	}

	// NOTE: The twin edges of the edges belonging to the face are not included.
	std::vector<uint32> findFaceSideEdgeIndices(const Hull3D& hull, uint32 faceIndex) {
		std::vector<uint32> edgeIndices;
		uint32 edgeIndex = 0;

		while (edgeIndex < hull.edges.count()) {
			const Edge3D edge = hull.edges[edgeIndex];

			if (edge.faceIndex == faceIndex) {
				edgeIndices.emplace_back(edgeIndex);

				/* uint32 otherEdgeIndex = edgeIndex;

				if (~edgeIndex & 1) {
					otherEdgeIndex += 1;
				}
				else {
					otherEdgeIndex -= 1;
				}

				edgeIndices.emplace_back(otherEdgeIndex); */
			}

			edgeIndex += 1;
		}

		return edgeIndices;
	}

	static std::vector<Face3D> findReferenceSideFaces(const Hull3D& referenceHull, uint32 referenceFaceIndex) {
		// This list SHOULD contain unique faces.
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

				const Face3D sideFace = referenceHull.faces[referenceHull.edges[otherEdgeIndex].faceIndex];

				sideFaces.emplace_back(sideFace);
			}

			edgeIndex += 1;
		}

		return sideFaces;
	}

	/// See https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm.
	std::vector<EdgeFeature3D> SutherlandHodgman(const SATOutput3D& sat, std::vector<EdgeFeature3D> incidentEdges, const std::vector<Face3D>& referenceSideFaces) {
		uint8 clippedEdgeCount = 0;
		uint8 previousEdgeCount = (uint8) incidentEdges.size();

		for (const Face3D referenceFace : referenceSideFaces) {
			const vec3<float32> transformedFaceNormal = vec3<float32>(referenceFace.normal) * sat.referenceBody->orientation;

			// TODO: Fix face offset transformation.
			// const vec3<float32> transformedFaceOffset = multiplyTransform(referenceFace.offset * vec3<float32>(transformedFaceNormal), sat.referenceBody->world);
			const vec3<float32> transformedFaceOffset = referenceFace.offset * vec3<float32>(transformedFaceNormal);

			std::vector<EdgeFeature3D> clippedEdges(8);
			uint8 incidentEdgeIndex = 0;

			clippedEdgeCount = 0;

			while (incidentEdgeIndex < previousEdgeCount) {
				const EdgeFeature3D& incidentEdge = incidentEdges[incidentEdgeIndex];
				const ClipVertex3D& a = incidentEdge.edge[0];
				const ClipVertex3D& b = incidentEdge.edge[1];

				const float32 distanceA = signedDistanceFP(transformedFaceNormal, transformedFaceOffset, a.position);
				const float32 distanceB = signedDistanceFP(transformedFaceNormal, transformedFaceOffset, b.position);

				EdgeFeature3D& clippedEdge = clippedEdges[clippedEdgeCount];
				clippedEdge.vertexCount = 0;

				if (distanceA <= 0.0f) {
					// Keep vertices that are below the face.
					clippedEdge.edge[clippedEdge.vertexCount] = a;
					clippedEdge.vertexCount += 1;
				}

				if (distanceB <= 0.0f) {
					// Keep vertices that are below the face.
					clippedEdge.edge[clippedEdge.vertexCount] = b;
					clippedEdge.vertexCount += 1;
				}

				if (distanceA * distanceB < 0.0f) {
					// If the vertices are on different sides of the faces,
					// generate a new vertex by interpolating between the computed distances.
					DEBUG_ASSERT(clippedEdge.vertexCount < 2);

					const float32 t = distanceA / (distanceA - distanceB);

					ClipVertex3D& c = clippedEdge.edge[clippedEdge.vertexCount];
					c.position = lerp(a.position, b.position, t);

					clippedEdge.vertexCount += 1;
				}

				incidentEdgeIndex += 1;
				clippedEdgeCount += 1;
			}

			incidentEdges = clippedEdges;

			previousEdgeCount = clippedEdgeCount;
		}

		incidentEdges.resize(clippedEdgeCount);

		return incidentEdges;
	}

	bool PolygonPolygon(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB, ContactManifold3D* contactManifold) {
		const SATOutput3D sat = SAT3D(rigidBodyA, rigidBodyB, hullA, hullB);

		if (sat.separation > 0.0f) {
			return false;
		}

		const uint32 incidentFaceIndex = findIncidentFaceIndex(*sat.incidentHull, sat.axis);
		const std::vector<vec3<float32>> referenceFaceVertices = findFaceVertices(*sat.incidentHull, incidentFaceIndex);
		const std::vector<uint32> incidentFaceSideEdgeIndices = findFaceSideEdgeIndices(*sat.incidentHull, incidentFaceIndex);

		const std::vector<Face3D> referenceSideFaces = findReferenceSideFaces(*sat.referenceHull, sat.referenceFaceIndex);

		std::vector<EdgeFeature3D> incidentEdges(incidentFaceSideEdgeIndices.size());
		uint32 edgeIndex = 0;

		while (edgeIndex < incidentFaceSideEdgeIndices.size()) {
			const uint32 firstEdgeIndex = incidentFaceSideEdgeIndices[edgeIndex];
			const Edge3D edge = sat.incidentHull->edges[firstEdgeIndex];

			uint16 indexA = edge.originVertexIndex;
			uint16 indexB;

			if (firstEdgeIndex & 1) {
				indexB = sat.incidentHull->edges[firstEdgeIndex - 1].originVertexIndex;
			}
			else {
				indexB = sat.incidentHull->edges[firstEdgeIndex + 1].originVertexIndex;
			}

			incidentEdges[edgeIndex].vertexCount = 2;
			incidentEdges[edgeIndex].edge[0].position = multiplyTransform(vec3<float32>(sat.incidentHull->vertexPositions[indexA]), sat.incidentBody->world);
			incidentEdges[edgeIndex].edge[1].position = multiplyTransform(vec3<float32>(sat.incidentHull->vertexPositions[indexB]), sat.incidentBody->world);

			edgeIndex += 1;
		}

		std::vector<EdgeFeature3D> clippedEdges = SutherlandHodgman(sat, incidentEdges, referenceSideFaces);

		// TODO: Discard points that are above the reference face.

		contactManifold->normal = sat.axis;
		contactManifold->contactCount = 0;

		// Move the contact points onto the reference face.
		for (EdgeFeature3D& clippedEdge : clippedEdges) {
			DEBUG_ASSERT(clippedEdge.vertexCount == 2);

			clippedEdge.edge[0].position -= sat.axis * sat.separation;
			clippedEdge.edge[1].position -= sat.axis * sat.separation;
		}

		int i = 0;
		for (auto e : clippedEdges) {
			DEBUG_ASSERT(e.vertexCount == 2);

			ENGINE_DEBUG("Edge []:\n- A = ([])\n- B = ([])", i, e.edge[0].position, e.edge[1].position);

			i += 1;
		}

		checklist<vec3<float32>, 8> contacts;

		// TODO(physics/3d/contact-checklist): This can be removed if we can get a vertex list from Sutherland-Hodgman.
		{
			for (const EdgeFeature3D& edge : clippedEdges) {
				DEBUG_ASSERT(edge.vertexCount == 2);

				contacts.add(edge.edge[0].position);
				contacts.add(edge.edge[1].position);
			}
		}

		DEBUG_ASSERT(contacts.count() > 0);

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

			while (contactIndex < contacts.count()) {
				angle = dot(contacts[contactIndex], n);

				if (angle > maxAngle) {
					maxAngle = angle;
					maxAngleContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			contactManifold->contacts[0].position = contacts[maxAngleContactIndex];
			contactManifold->contactCount += 1;

			contacts.remove(maxAngleContactIndex);
		}

		if (contacts.count() == 0) {
			return true;
		}

		// The second contact is the farthest one from the first contact.
		{
			const vec3<float32> a = contactManifold->contacts[0].position;

			index contactIndex = 0;
			index maxDistanceContactIndex = max<index>();
			float32 distance;
			float32 maxDistance = min<float32>();

			while (contactIndex < contacts.count()) {
				distance = distanceSquared(a, contacts[contactIndex]);

				if (distance > maxDistance) {
					maxDistance = distance;
					maxDistanceContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			contactManifold->contacts[1].position = contacts[maxDistanceContactIndex];
			contactManifold->contactCount += 1;

			contacts.remove(maxDistanceContactIndex);
		}

		if (contacts.count() == 0) {
			return true;
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

			while (contactIndex < contacts.count()) {
				c = contacts[contactIndex];

				area = dot(cross(a - c, b - c) * 0.5f, sat.axis);

				if (area > maxArea) {
					maxArea = area;
					maxAreaContactIndex = contactIndex;
				}

				contactIndex += 1;
			}

			contactManifold->contacts[2].position = contacts[maxAreaContactIndex];
			contactManifold->contactCount += 1;

			contacts.remove(maxAreaContactIndex);
		}

		if (contacts.count() == 0) {
			return true;
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

			while (contactIndex < contacts.count()) {
				d = contacts[contactIndex];

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

			contactManifold->contacts[3].position = contacts[minAreaContactIndex];
			contactManifold->contactCount += 1;

			contacts.remove(minAreaContactIndex);
		}

		return true;
	}
}