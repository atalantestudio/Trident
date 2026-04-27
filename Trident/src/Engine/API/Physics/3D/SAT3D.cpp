// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/SAT3D.hpp"

namespace Trident {
	/// See https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf.
	inline static bool isMinkowskiFace(
		/// Face normal of the base edge on A.
		vec3<float32> a,

		/// Face normal of the twin edge on A.
		vec3<float32> b,

		/// The cross product of B and A.
		vec3<float32> crossBA,

		/// Face normal of the base edge on B.
		vec3<float32> c,

		/// Face normal of the twin edge on B.
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

		if (lengthCrossAB < PARALLEL_EDGE_TOLERANCE * sqrt(lengthSquared(edgeDirectionA) * lengthSquared(edgeDirectionB))) {
			// Edges are parallel or nearly parallel.
			return min<float32>();
		}

		vec3<float32> normal = crossAB / lengthCrossAB;

		if (dot(normal, edgeOriginA - centroidA) < 0.0f) {
			// Ensure the normal points towards B.
			normal = -normal;
		}

		return dot(normal, edgeOriginB - edgeOriginA);
	}

	static EdgeQuery queryEdgeDirections(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		EdgeQuery query{};
		query.separation = min<float32>();

		// TODO(physics/3d/transform): Cleanup transform step.
		const mat4x4<float32> relativeTransform = inverse(rigidBodyB.world) * rigidBodyA.world;
		// const quat<float32> relativeOrientation = rigidBodyB.orientation * conjugate(rigidBodyA.orientation);
		const mat4x4<float32> relativeOrientation = inverse(mat4x4<float32>(rigidBodyB.orientation)) * mat4x4<float32>(rigidBodyA.orientation);

		const vec3<float32> centroidA = multiplyTransform(vec3<float32>(hullA.centroid), relativeTransform);

		uint16 edgeIndexA = 0;

		while (edgeIndexA < hullA.edges.count()) {
			const Edge3D edgeA = hullA.edges[edgeIndexA + 0];
			const Edge3D twinA = hullA.edges[edgeIndexA + 1];

			const vec3<float32> p1 = multiplyTransform(vec3<float32>(hullA.vertexPositions[edgeA.originVertexIndex]), relativeTransform);
			const vec3<float32> q1 = multiplyTransform(vec3<float32>(hullA.vertexPositions[twinA.originVertexIndex]), relativeTransform);
			const vec3<float32> e1 = q1 - p1;

			const vec3<float32> u1 = multiplyTransform(rigidBodyA.facePlanes[edgeA.faceIndex].localNormal, relativeOrientation);
			const vec3<float32> v1 = multiplyTransform(rigidBodyA.facePlanes[twinA.faceIndex].localNormal, relativeOrientation);

			uint16 edgeIndexB = 0;

			while (edgeIndexB < hullB.edges.count()) {
				const Edge3D edgeB = hullB.edges[edgeIndexB + 0];
				const Edge3D twinB = hullB.edges[edgeIndexB + 1];

				const vec3<float32> p2 = hullB.vertexPositions[edgeB.originVertexIndex];
				const vec3<float32> q2 = hullB.vertexPositions[twinB.originVertexIndex];
				const vec3<float32> e2 = q2 - p2;

				const vec3<float32> u2 = rigidBodyB.facePlanes[edgeB.faceIndex].localNormal;
				const vec3<float32> v2 = rigidBodyB.facePlanes[twinB.faceIndex].localNormal;

				if (isMinkowskiFace(u1, v1, -e1, -u2, -v2, -e2)) {
					const float32 separation = calculateEdgeDistance(p1, e1, p2, e2, centroidA);

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

	static FaceQuery queryFaceDirections(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		FaceQuery query{};
		query.separation = min<float32>();

		// Relative rotation between A and B.
		const quat<float32> R = rigidBodyB.orientation * inverse(rigidBodyA.orientation);

		uint16 faceIndex = 0;

		while (faceIndex < hullA.faces.count()) {
			const Plane3D& P = rigidBodyA.facePlanes[faceIndex];
			const vec3<float32> s = multiplyTransform(hullB.support(-P.localNormal * R), rigidBodyB.world);
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