// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/Collider.hpp"
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
	std::vector<vec3<float32>> SutherlandHodgman(const SATOutput3D& sat, const std::vector<Face3D>& referenceSideFaces, const std::vector<uint32>& incidentEdgeIndices) {
		std::vector<vec3<float32>> clippedVertices;

		for (const Face3D referenceFace : referenceSideFaces) {
			// Transform face.
			const vec3<float32> transformedFaceNormal = vec3<float32>(referenceFace.normal) * sat.referenceBody->orientation;
			const vec3<float32> transformedFaceOffset = multiplyTransform(referenceFace.offset * vec3<float32>(transformedFaceNormal), sat.referenceBody->world);

			for (uint32 incidentEdgeIndex : incidentEdgeIndices) {
				uint32 otherEdgeIndex;

				if (incidentEdgeIndex & 1) {
					otherEdgeIndex = incidentEdgeIndex - 1;
				}
				else {
					otherEdgeIndex = incidentEdgeIndex + 1;
				}

				const Edge3D edge = sat.incidentHull->edges[incidentEdgeIndex];
				const Edge3D otherEdge = sat.incidentHull->edges[otherEdgeIndex];

				const vec3<float32> a = multiplyTransform(vec3<float32>(sat.incidentHull->vertexPositions[edge.originVertexIndex]), sat.incidentBody->world);
				const vec3<float32> b = multiplyTransform(vec3<float32>(sat.incidentHull->vertexPositions[otherEdge.originVertexIndex]), sat.incidentBody->world);

				const float32 distanceA = signedDistanceFP(transformedFaceNormal, transformedFaceOffset, a);
				const float32 distanceB = signedDistanceFP(transformedFaceNormal, transformedFaceOffset, b);

				uint32 newVertexCount = 0;

				if (distanceA <= 0.0f) {
					// Keep vertices that are below the face.
					clippedVertices.emplace_back(a);

					newVertexCount += 1;
				}

				if (distanceB <= 0.0f) {
					// Keep vertices that are below the face.
					clippedVertices.emplace_back(b);

					newVertexCount += 1;
				}

				if (distanceA * distanceB < 0.0f) {
					// If the vertices are on different sides of the faces,
					// generate a new vertex by interpolating between the computed distances.
					DEBUG_ASSERT(newVertexCount < 2);

					const float32 t = distanceA / (distanceA - distanceB);
					const vec3<float32> c = lerp(a, b, t);

					clippedVertices.emplace_back(c);
				}
			}

			BREAKPOINT();
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
		const std::vector<uint32> incidentFaceSideEdgeIndices = findFaceSideEdgeIndices(*sat.incidentHull, incidentFaceIndex);

		const std::vector<Face3D> referenceSideFaces = findReferenceSideFaces(*sat.referenceHull, sat.referenceFaceIndex);

		const std::vector<vec3<float32>> clippedVertices = SutherlandHodgman(sat, referenceSideFaces, incidentFaceSideEdgeIndices);

		contactManifold->normal = sat.axis;

		return true;
	}
}