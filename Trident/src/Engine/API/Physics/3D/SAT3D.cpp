// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/SAT3D.hpp"

namespace Trident {
	inline static float32 signedDistanceFacePoint(Face3D face, vec3<float32> p) {
		return dot(vec3<float32>(face.normal), p - face.offset * vec3<float32>(face.normal));
	}

	inline static Face3D transformFace(const RigidBody3DComponent& rigidBody, Face3D face) {
		Face3D transformedFace{};
		transformedFace.normal = svec3<float32>(vec3<float32>(face.normal) * rigidBody.orientation);
		transformedFace.offset = dot(vec3<float32>(transformedFace.normal), rigidBody.position + (vec3<float32>(face.normal) * face.offset * rigidBody.scale));

		return transformedFace;
	}

	static vec3<float32> support(const Hull3D& hull, vec3<float32> d) {
		float32 maxValue = dot(vec3<float32>(hull.vertexPositions[0]), d);
		uint32 maxVertexIndex = 0;
		uint32 vertexIndex = 1;

		while (vertexIndex < hull.vertexPositions.count()) {
			const float32 value = dot(vec3<float32>(hull.vertexPositions[vertexIndex]), d);

			if (value > maxValue) {
				maxValue = value;
				maxVertexIndex = vertexIndex;
			}

			vertexIndex += 1;
		}

		return vec3<float32>(hull.vertexPositions[maxVertexIndex]);
	}

	static void queryFaceDirections(SATOutput3D& output, const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		// Relative rotation between A and B.
		const quat<float32> R = rigidBodyB.orientation * inverse(rigidBodyA.orientation);

		uint32 faceIndex = 0;

		while (faceIndex < hullA.faces.count()) {
			const Face3D faceA = hullA.faces[faceIndex];
			const vec3<float32> d = vec3<float32>(faceA.normal);
			const vec3<float32> sB = support(hullB, -d * R);
			const Face3D transformedFace = transformFace(rigidBodyA, faceA);
			const vec3<float32> sBT = multiplyTransform(sB, rigidBodyB.world);
			const float32 separation = signedDistanceFacePoint(transformedFace, sBT);

			if (separation > output.separation) {
				output.separation = separation;
				output.axis = d;
				output.referenceFaceIndex = faceIndex;
				output.referenceBody = &rigidBodyA;
				output.incidentBody = &rigidBodyB;
				output.referenceHull = &hullA;
				output.incidentHull = &hullB;
			}

			if (separation > 0.0f) {
				break;
			}

			faceIndex += 1;
		}
	}

	SATOutput3D SAT3D(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB) {
		SATOutput3D output;
		output.separation = min<float32>();

		queryFaceDirections(output, rigidBodyA, rigidBodyB, hullA, hullB);

		if (output.separation > 0.0f) {
			return output;
		}

		queryFaceDirections(output, rigidBodyB, rigidBodyA, hullB, hullA);

		if (output.separation > 0.0f) {
			return output;
		}

		output.axis *= -output.incidentBody->orientation;

		return output;
	}
}