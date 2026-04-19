// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/SutherlandHodgman3D.hpp"

namespace Trident {
	pool<ClipVertex3D, 8> SutherlandHodgman3D(const SATOutput3D& sat, pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> inputVertices, const std::vector<Face3D>& referenceSideFaces) {
		for (const Face3D& referenceFace : referenceSideFaces) {
			const vec3<float32> transformedFaceNormal = vec3<float32>(referenceFace.normal) * sat.referenceBody->orientation;
			const float32 transformedFaceOffset = length(multiplyTransform(referenceFace.offset * vec3<float32>(referenceFace.normal), sat.referenceBody->world));

			pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> outputVertices;

			uint8 vertexOffset = 0;

			while (vertexOffset < inputVertices.count()) {
				const ClipVertex3D& a = inputVertices[vertexOffset];
				const ClipVertex3D& b = inputVertices[(vertexOffset + 1) % inputVertices.count()];

				const float32 distanceA = signedDistanceFP(transformedFaceNormal, transformedFaceOffset, a.position);
				const float32 distanceB = signedDistanceFP(transformedFaceNormal, transformedFaceOffset, b.position);

				vertexOffset += 1;

				// If A and B are on different sides of the face, keep the intersection point.
				if (distanceA * distanceB < 0.0f) {
					const float32 t = distanceA / (distanceA - distanceB);

					ClipVertex3D c{};
					c.position = lerp(a.position, b.position, t);

					outputVertices.add(c);
				}

				// If B is below the face, keep it.
				if (distanceB <= 0.0f) {
					outputVertices.add(b);
				}
			}

			inputVertices = outputVertices;
		}

		return inputVertices;
	}
}