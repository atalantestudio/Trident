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
	static bool isMinkowskiFace(vec3<float32> a, vec3<float32> b, vec3<float32> c, vec3<float32> d) {
		const vec3<float32> crossBA = cross(b, a);
		const vec3<float32> crossDC = cross(d, c);

		const float32 cba = dot(c, crossBA);
		const float32 dba = dot(d, crossBA);
		const float32 adc = dot(a, crossDC);
		const float32 bdc = dot(b, crossDC);

		return cba * dba < 0.0f && adc * bdc < 0.0f && cba * bdc > 0.0f;
	}

	static float32 calculateEdgeDistance(vec3<float32> hullCentroidA, vec3<float32> edgeOriginA, vec3<float32> edgeDirectionA, vec3<float32> edgeOriginB, vec3<float32> edgeDirectionB) {
		// Edges are parallel or nearly parallel.
		if (distanceSquared(edgeDirectionA, edgeDirectionB) < 0.001f) {
			return min<float32>();
		}

		vec3<float32> normal = normalized(cross(edgeDirectionA, edgeDirectionB));

		if (dot(normal, edgeOriginA - hullCentroidA) > 0.0f) {
			// Ensure the normal points towards B.
			normal = -normal;
		}

		return dot(normal, edgeOriginB - edgeOriginA);
	}

	static FaceQuery queryFaceDirections(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		FaceQuery query{};
		query.separation = min<float32>();

		// Relative rotation between A and B.
		const quat<float32> R = rigidBodyB.orientation * inverse(rigidBodyA.orientation);

		uint16 faceIndex = 0;

		while (faceIndex < hullA.faces.count()) {
			const Face3D& faceA = hullA.faces[faceIndex];
			const vec3<float32> d = faceA.normal;
			const vec3<float32> s = support(hullB, -d * R);
			const vec3<float32> sT = multiplyTransform(s, rigidBodyB.world);
			const Face3D F = transformFace(faceA, rigidBodyA.world);
			const float32 separation = signedDistanceFP(vec3<float32>(F.normal), F.offset, sT);

			if (separation > query.separation) {
				query.separation = separation;
				query.axis = d;
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

	static EdgeQuery queryEdgeDirections(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		EdgeQuery query;
		query.separation = min<float32>();

		// Relative rotation between A and B.
		const quat<float32> R = rigidBodyB.orientation * inverse(rigidBodyA.orientation);

		uint16 edgeIndexA = 0;

		const vec3<float32> hullCentroidA = vec3<float32>(hullA.centroid) + rigidBodyA.position;

		while (edgeIndexA < hullA.edges.count()) {
			const Edge3D edgeA = hullA.edges[edgeIndexA];
			const Edge3D twinEdgeA = hullA.edges[edgeIndexA + 1];
			const vec3<float32> a = vec3<float32>(hullA.faces[edgeA.faceIndex].normal) * rigidBodyA.orientation;
			const vec3<float32> b = vec3<float32>(hullA.faces[twinEdgeA.faceIndex].normal) * rigidBodyA.orientation;
			const vec3<float32> ab = b - a;

			uint16 edgeIndexB = 0;

			while (edgeIndexB < hullB.edges.count()) {
				const Edge3D edgeB = hullB.edges[edgeIndexB];
				const Edge3D twinEdgeB = hullA.edges[edgeIndexB + 1];
				const vec3<float32> c = vec3<float32>(hullB.faces[edgeB.faceIndex].normal) * rigidBodyB.orientation;
				const vec3<float32> d = vec3<float32>(hullB.faces[twinEdgeB.faceIndex].normal) * rigidBodyB.orientation;

				if (isMinkowskiFace(a, b, -c, -d)) {
					const float32 separation = calculateEdgeDistance(hullCentroidA, a, ab, c, d - c);

					if (separation > query.separation) {
						query.separation = separation;
						query.edgeA = edgeA;
						query.edgeB = edgeB;
					}
				}

				edgeIndexB += 2;
			}

			edgeIndexA += 2;
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

		output.edgeQuery.separation = min<float32>();
		/* output.edgeQuery = queryEdgeDirections(rigidBodyA, rigidBodyB, hullA, hullB);

		if (output.edgeQuery.separation > 0.0f) {
			return output;
		} */

		output.overlap = true;

		return output;
	}
}