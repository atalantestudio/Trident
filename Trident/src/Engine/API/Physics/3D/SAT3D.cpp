// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/SAT3D.hpp"

namespace Trident {
	inline static bool isMinkowskiFace(
		/// The face normal of the base edge on A.
		vec3<float32> a,

		/// The face normal of the twin edge on A.
		vec3<float32> b,

		/// The cross product of B and A.
		vec3<float32> crossBA,

		/// The face normal of the base edge on B.
		vec3<float32> c,

		/// The face normal of the twin edge on B.
		vec3<float32> d,

		/// The cross product of D and C.
		vec3<float32> crossDC
	) {
		const float32 cba = dot(c, crossBA);
		const float32 dba = dot(d, crossBA);
		const float32 adc = dot(a, crossDC);
		const float32 bdc = dot(b, crossDC);

		return cba * dba < 0.0f && adc * bdc < 0.0f && cba * bdc > 0.0f;
	}

	static float32 calculateEdgeDistance(vec3<float32> edgeOriginA, vec3<float32> edgeDirectionA, vec3<float32> edgeOriginB, vec3<float32> edgeDirectionB, vec3<float32> centroidA) {
		static constexpr float32 PARALLEL_EDGE_TOLERANCE = 0.005f;

		const vec3<float32> crossAB = cross(edgeDirectionA, edgeDirectionB);
		const float32 lengthCrossAB = length(crossAB);

		// Stop if the edges are quasi-parallel.
		if (lengthCrossAB < PARALLEL_EDGE_TOLERANCE * sqrt(lengthSquared(edgeDirectionA) * lengthSquared(edgeDirectionB))) {
			return min<float32>();
		}

		vec3<float32> normal = crossAB / lengthCrossAB;

		// Ensure the normal points towards B.
		if (dot(normal, edgeOriginA - centroidA) < 0.0f) {
			normal = -normal;
		}

		return dot(normal, edgeOriginB - edgeOriginA);
	}

	static EdgeQuery queryEdgeDirections(const Transform3D& transformA, const Transform3D& transformB, const Hull3D& hullA, const Hull3D& hullB) {
		EdgeQuery query{};

		// All computations are done in the local space of B.
		const Transform3D transform = transformA * inverse(transformB);

		const vec3<float32> centroidA = vec3<float32>(hullA.centroid) + transform.position;

		uint16 edgeIndexA = 0;

		while (edgeIndexA < hullA.edges.count()) {
			const Edge3D edgeA = hullA.edges[edgeIndexA + 0];
			const Edge3D twinA = hullA.edges[edgeIndexA + 1];

			const vec3<float32> a = vec3<float32>(hullA.vertexPositions[edgeA.originVertexIndex]) + transform.position;
			const vec3<float32> b = vec3<float32>(hullA.vertexPositions[twinA.originVertexIndex]) + transform.position;
			const vec3<float32> ab = b - a;

			// Face normals are vertices on the Gauss map.
			const vec3<float32> gmA = vec3<float32>(hullA.faces[edgeA.faceIndex].normal) * transform.orientation;
			const vec3<float32> gmB = vec3<float32>(hullA.faces[twinA.faceIndex].normal) * transform.orientation;

			uint16 edgeIndexB = 0;

			while (edgeIndexB < hullB.edges.count()) {
				const Edge3D edgeB = hullB.edges[edgeIndexB + 0];
				const Edge3D twinB = hullB.edges[edgeIndexB + 1];

				const vec3<float32> c = hullB.vertexPositions[edgeB.originVertexIndex];
				const vec3<float32> d = hullB.vertexPositions[twinB.originVertexIndex];
				const vec3<float32> cd = d - c;

				// Face normals are vertices on the Gauss map.
				const vec3<float32> gmC = hullB.faces[edgeB.faceIndex].normal;
				const vec3<float32> gmD = hullB.faces[twinB.faceIndex].normal;

				if (isMinkowskiFace(gmA, gmB, -ab, -gmC, -gmD, -cd)) {
					const float32 separation = calculateEdgeDistance(a, ab, c, cd, centroidA);

					if (separation > query.separation) {
						query.separation = separation;
						query.edgeIndexA = edgeIndexA;
						query.edgeIndexB = edgeIndexB;
					}

					if (separation > 0.0f) {
						return query;
					}
				}

				edgeIndexB += 2;
			}

			edgeIndexA += 2;
		}

		return query;
	}

	static FaceQuery queryFaceDirections(const Transform3D& transformA, const Transform3D& transformB, const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		FaceQuery query{};

		// TODO(physics/3d/sat): Move computations in the local space of B.
		// // All computations are done in the local space of B.
		// const Transform3D transform = transformA * inverse(transformB);

		const quat<float32> R = rigidBodyB.orientation * inverse(rigidBodyA.orientation);

		uint16 faceIndex = 0;

		while (faceIndex < hullA.faces.count()) {
			const Plane3D face = hullA.faces[faceIndex];

			// TODO(physics/3d/sat): Move computations in the local space of B.
			const Plane3D plane = rigidBodyA.facePlanes[faceIndex];
			const vec3<float32> support = multiplyTransform(hullB.support(-vec3<float32>(face.normal) * R), rigidBodyB.world);

			/* Plane3D plane = face * transform;

			if (bToA) {
				plane.offset = abs(plane.offset);
			}

			const vec3<float32> support = hullB.support(-vec3<float32>(plane.normal)); */

			const float32 separation = signedDistance(plane, support);

			if (separation > query.separation) {
				query.separation = separation;
				query.axis = face.normal;
				query.referenceFaceIndex = faceIndex;
			}

			if (separation > 0.0f) {
				break;
			}

			faceIndex += 1;
		}

		return query;
	}

	SATOutput3D SAT3D(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		SATOutput3D output;
		output.overlap = false;
		output.faceQueryA = queryFaceDirections(rigidBodyA.getTransform(), rigidBodyB.getTransform(), rigidBodyA, rigidBodyB, hullA, hullB);

		if (output.faceQueryA.separation > 0.0f) {
			return output;
		}

		output.faceQueryB = queryFaceDirections(rigidBodyB.getTransform(), rigidBodyA.getTransform(), rigidBodyB, rigidBodyA, hullB, hullA);

		if (output.faceQueryB.separation > 0.0f) {
			return output;
		}

		output.edgeQuery = queryEdgeDirections(rigidBodyA.getTransform(), rigidBodyB.getTransform(), hullA, hullB);

		if (output.edgeQuery.separation > 0.0f) {
			return output;
		}

		output.overlap = true;

		return output;
	}
}