// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/Collider.hpp"
#include "Engine/API/Physics/3D/SAT3D.hpp"

namespace Trident {
	inline static float32 signedDistanceFacePoint(vec3<float32> faceNormal, vec3<float32> faceOffset, vec3<float32> p) {
		return dot(faceNormal, p - faceOffset);
	}

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

	static std::vector<vec3<float32>> findFaceVertices(const Hull3D& referenceHull, uint32 referenceFaceIndex) {
		std::vector<vec3<float32>> vertices;
		uint32 edgeIndex = 0;

		while (edgeIndex < referenceHull.edges.count()) {
			const Edge3D edge = referenceHull.edges[edgeIndex];

			if (edge.faceIndex == referenceFaceIndex) {
				const vec3<float32> baseVertex = referenceHull.vertexPositions[edge.originVertexIndex];

				vertices.emplace_back(baseVertex);
			}

			edgeIndex += 1;
		}

		return vertices;
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
	std::vector<vec3<float32>> SutherlandHodgman(const std::vector<vec3<float32>>& incidentFaceVertices, const std::vector<Face3D>& referenceSideFaces) {
		std::vector<vec3<float32>> clippedVertices;

		for (const Face3D referenceFace : referenceSideFaces) {
			for (const vec3<float32> incidentFaceVertex : incidentFaceVertices) {
				// TODO: Transform the vertices and the faces before clipping.
				// BREAKPOINT();
			}
		}

		return clippedVertices;
	}

	bool PolygonPolygon(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB, ContactManifold3D* contactManifold) {
		const SATOutput3D sat = SAT3D(rigidBodyA, rigidBodyB, hullA, hullB);

		if (sat.separation > 0.0f) {
			return false;
		}

		const uint32 incidentFaceIndex = findIncidentFaceIndex(*sat.incidentHull, sat.axis);
		const std::vector<vec3<float32>> referenceFaceVertices = findFaceVertices(*sat.incidentHull, incidentFaceIndex);

		const std::vector<Face3D> referenceSideFaces = findReferenceSideFaces(*sat.referenceHull, sat.referenceFaceIndex);

		const std::vector<vec3<float32>> clippedVertices = SutherlandHodgman(referenceFaceVertices, referenceSideFaces);

		contactManifold->normal = sat.axis;

		return true;
	}
}