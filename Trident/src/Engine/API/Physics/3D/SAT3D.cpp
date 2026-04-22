// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/SAT3D.hpp"

namespace Trident {
	static vec3<float32> support(const Hull3D& hull, vec3<float32> d) {
		float32 distance;
		float32 maxDistance = dot(vec3<float32>(hull.vertexPositions[0]), d);
		uint32 maxDistanceVertexIndex = 0;
		uint32 vertexIndex = 1;

		while (vertexIndex < hull.vertexPositions.count()) {
			distance = dot(vec3<float32>(hull.vertexPositions[vertexIndex]), d);

			if (distance > maxDistance) {
				maxDistance = distance;
				maxDistanceVertexIndex = vertexIndex;
			}

			vertexIndex += 1;
		}

		return vec3<float32>(hull.vertexPositions[maxDistanceVertexIndex]);
	}

	/// See https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf.
	static bool isMinkowskiFace(
		/// Face normal of the base edge on A.
		vec3<float32> a,

		/// Face normal of the twin edge on A.
		vec3<float32> b,

		/// Face normal of the base edge on B.
		vec3<float32> c,

		/// Face normal of the twin edge on B.
		vec3<float32> d
	) {
		const vec3<float32> crossBA = cross(b, a);
		const vec3<float32> crossDC = cross(d, c);

		const float32 cba = dot(c, crossBA);
		const float32 dba = dot(d, crossBA);
		const float32 adc = dot(a, crossDC);
		const float32 bdc = dot(b, crossDC);

		return cba * dba < 0.0f && adc * bdc < 0.0f && cba * bdc > 0.0f;
	}

	static float32 calculateEdgeDistance(vec3<float32> hullCentroidA, vec3<float32> edgeOriginA, vec3<float32> edgeDirectionA, vec3<float32> edgeOriginB, vec3<float32> edgeDirectionB) {
		if (distanceSquared(edgeDirectionA, edgeDirectionB) < 0.001f) {
			// Edges are parallel or nearly parallel.
			return min<float32>();
		}

		vec3<float32> normal = normalized(cross(edgeDirectionA, edgeDirectionB));

		if (dot(normal, edgeOriginA - hullCentroidA) > 0.0f) {
			// Ensure the normal points towards B.
			normal = -normal;
		}

		return dot(normal, edgeOriginB - edgeOriginA);
	}

	static EdgeQuery queryEdgeDirections(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		EdgeQuery query{};
		query.separation = min<float32>();

		// TODO(debug)
		const vec3<float32> hullCentroidA = vec3<float32>(hullA.centroid); // + rigidBodyA.position;

		// TODO(debug)
		// A:
		// - [vertexID: 0, faceID: 4] // top (16)
		// - [vertexID: 5, faceID: 3] // right (17)
		// 
		// B:
		// - [vertexID: 6, faceID: 1] // back (14)
		// - [vertexID: 4, faceID: 2] // left (15)

		uint16 edgeIndexA = 0;

		while (edgeIndexA < hullA.edges.count()) {
			const Edge3D edgeA = hullA.edges[edgeIndexA + 0];
			const Edge3D twinEdgeA = hullA.edges[edgeIndexA + 1];
			const vec3<float32> a = rigidBodyA.facePlanes[edgeA.faceIndex].worldNormal;
			const vec3<float32> b = rigidBodyA.facePlanes[twinEdgeA.faceIndex].worldNormal;

			uint16 edgeIndexB = 0;

			while (edgeIndexB < hullB.edges.count()) {
				const Edge3D edgeB = hullB.edges[edgeIndexB + 0];
				const Edge3D twinEdgeB = hullB.edges[edgeIndexB + 1];
				const vec3<float32> c = rigidBodyB.facePlanes[edgeB.faceIndex].worldNormal;
				const vec3<float32> d = rigidBodyB.facePlanes[twinEdgeB.faceIndex].worldNormal;

				// TODO(debug)
				// if (edgeIndexA == 16 && edgeIndexB == 14) BREAKPOINT();

				if (isMinkowskiFace(a, b, -c, -d)) {
					const vec3<float32> edgeOriginA = multiplyTransform(vec3<float32>(hullA.vertexPositions[edgeA.originVertexIndex]), rigidBodyA.world);
					const vec3<float32> edgeEndA = multiplyTransform(vec3<float32>(hullA.vertexPositions[twinEdgeA.originVertexIndex]), rigidBodyA.world);
					const vec3<float32> edgeDirectionA = normalized(edgeEndA - edgeOriginA);
					const vec3<float32> edgeOriginB = multiplyTransform(vec3<float32>(hullB.vertexPositions[edgeB.originVertexIndex]), rigidBodyB.world);
					const vec3<float32> edgeEndB = multiplyTransform(vec3<float32>(hullB.vertexPositions[twinEdgeB.originVertexIndex]), rigidBodyB.world);
					const vec3<float32> edgeDirectionB = normalized(edgeEndB - edgeOriginB);

					const float32 separation = calculateEdgeDistance(hullCentroidA, edgeOriginA, edgeDirectionA, edgeOriginB, edgeDirectionB);

					if (separation > query.separation) {
						query.separation = separation;
						query.edgeA = edgeA;
						query.edgeB = edgeB;
					}

					if (separation > 0.0f) {
						break;
					}
				}

				edgeIndexB += 2;
			}

			edgeIndexA += 2;
		}

		return query;
	}

	static FaceQuery queryFaceDirections(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		FaceQuery query{};
		query.separation = min<float32>();

		// Relative rotation between A and B.
		const quat<float32> R = rigidBodyB.orientation * inverse(rigidBodyA.orientation);

		uint16 faceIndex = 0;

		while (faceIndex < hullA.faces.count()) {
			const Plane3D& P = rigidBodyA.facePlanes[faceIndex];
			const vec3<float32> s = multiplyTransform(support(hullB, -P.localNormal * R), rigidBodyB.world);
			const float32 separation = signedDistanceWorld(P, s);

			if (separation > query.separation) {
				query.separation = separation;
				query.axis = P.localNormal;
				query.referenceFaceIndex = faceIndex;
				query.referenceBody = &rigidBodyA;
				query.incidentBody = &rigidBodyB;
				query.referenceHull = &hullA;
				query.incidentHull = &hullB;
			}

			if (separation > 0.0f) {
				break;
			}

			faceIndex += 1;
		}

		return query;
	}

	SATOutput3D SAT3D(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		SATOutput3D output{};
		output.overlap = false;

		output.faceQueryA = queryFaceDirections(rigidBodyA, rigidBodyB, hullA, hullB);

		if (output.faceQueryA.separation > 0.0f) {
			return output;
		}

		output.faceQueryB = queryFaceDirections(rigidBodyB, rigidBodyA, hullB, hullA);

		if (output.faceQueryB.separation > 0.0f) {
			return output;
		}

		output.edgeQuery = queryEdgeDirections(rigidBodyA, rigidBodyB, hullA, hullB);

		if (output.edgeQuery.separation > 0.0f) {
			return output;
		}

		output.overlap = true;

		return output;
	}
}