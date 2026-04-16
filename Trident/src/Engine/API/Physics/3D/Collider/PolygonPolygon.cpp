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

	static vec3<float32> support(const std::vector<EdgeFeature3D>& edges, vec3<float32> d) {
		float32 maxValue = min<float32>();
		vec3<float32> s;

		for (const EdgeFeature3D& edge : edges) {
			DEBUG_ASSERT(edge.vertexCount == 2);

			float32 value = dot(edge.edge[0].position, d);

			if (value > maxValue) {
				maxValue = value;
				s = edge.edge[0].position;
			}

			value = dot(edge.edge[1].position, d);

			if (value > maxValue) {
				maxValue = value;
				s = edge.edge[1].position;
			}
		}

		return s;
	}

	static vec3<float32> farthestFrom(const std::vector<EdgeFeature3D>& edges, vec3<float32> a) {
		float32 maxDistance = min<float32>();
		vec3<float32> b;

		for (const EdgeFeature3D& edge : edges) {
			DEBUG_ASSERT(edge.vertexCount == 2);

			float32 _distance = distanceSquared(edge.edge[0].position, b);

			if (_distance > maxDistance) {
				maxDistance = _distance;
				b = edge.edge[0].position;
			}

			_distance = distanceSquared(edge.edge[0].position, b);

			if (_distance > maxDistance) {
				maxDistance = _distance;
				b = edge.edge[1].position;
			}
		}

		return b;
	}

	static vec3<float32> areaMaximized(const std::vector<EdgeFeature3D>& edges, vec3<float32> a, vec3<float32> b, vec3<float32> faceNormal) {
		float32 maxArea = min<float32>();
		float32 area;
		vec3<float32> c;
		vec3<float32> ca;
		vec3<float32> cb;

		for (const EdgeFeature3D& edge : edges) {
			DEBUG_ASSERT(edge.vertexCount == 2);

			ca = a - edge.edge[0].position;
			cb = b - edge.edge[0].position;

			area = dot((0.5f * cross(ca, cb)), faceNormal);

			if (area > maxArea) {
				maxArea = area;
				c = edge.edge[0].position;
			}

			ca = a - edge.edge[1].position;
			cb = b - edge.edge[1].position;

			area = dot((0.5f * cross(ca, cb)), faceNormal);

			if (area > maxArea) {
				maxArea = area;
				c = edge.edge[1].position;
			}
		}

		return c;
	}

	static vec3<float32> areaMaximizedNegative(const std::vector<EdgeFeature3D>& edges, vec3<float32> a, vec3<float32> b, vec3<float32> c, vec3<float32> faceNormal) {
		float32 area;
		float32 minArea = max<float32>();
		const vec3<float32> ab = b - a;
		vec3<float32> bc;
		vec3<float32> ca;
		vec3<float32> d;

		// TODO
		/* for (const EdgeFeature3D& edge : edges) {
			DEBUG_ASSERT(edge.vertexCount == 2);

			d0 = edge.edge[0].position - b;
			d1 = a - edge.edge[0].position;

			area = dot((0.5f * cross(ca, cb)), faceNormal);

			if (area < minArea) {
				minArea = area;
				c = edge.edge[0].position;
			}

			bc = edge.edge[0].position - b;
			ca = a - edge.edge[0].position;

			area = dot((0.5f * cross(ca, cb)), faceNormal);

			if (area < minArea) {
				minArea = area;
				c = edge.edge[1].position;
			}
		} */

		return c;
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

		/// Contact point reduction.
		/// See https://media.steampowered.com/apps/valve/2015/DirkGregorius_Contacts.pdf.

		// The first contact is selected in a fixed direction to avoid jitter.
		contactManifold->contacts[contactManifold->contactCount].position = support(clippedEdges, {1.0f, 1.0f, 1.0f});
		contactManifold->contactCount += 1;

		// The second contact is the farthest one from the first contact.
		contactManifold->contacts[contactManifold->contactCount].position = farthestFrom(clippedEdges, contactManifold->contacts[0].position);
		contactManifold->contactCount += 1;

		// The third contact maximizes the area of the current manifold.
		contactManifold->contacts[contactManifold->contactCount].position = areaMaximized(clippedEdges, contactManifold->contacts[0].position, contactManifold->contacts[1].position, sat.axis);
		contactManifold->contactCount += 1;

		// The fourth contact adds the largest area of the current manifold.
		contactManifold->contacts[contactManifold->contactCount].position = areaMaximizedNegative(clippedEdges, contactManifold->contacts[0].position, contactManifold->contacts[1].position, contactManifold->contacts[2].position, sat.axis);
		contactManifold->contactCount += 1;

		return true;
	}
}