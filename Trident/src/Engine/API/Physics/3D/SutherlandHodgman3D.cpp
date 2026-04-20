// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/SutherlandHodgman3D.hpp"

namespace Trident {
	pool<ClipVertex3D, 8> SutherlandHodgman3D(const FaceQuery& faceQuery, uint16 faceIndex, pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> inputVertices, const std::vector<Face3D>& referenceSideFaces) {
		for (const Face3D& referenceFace : referenceSideFaces) {
			const Face3D F = transformFace(referenceFace, faceQuery.referenceBody->world);

			pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> outputVertices;
			index vertexOffset = 0;

			while (vertexOffset < inputVertices.count()) {
				const ClipVertex3D& a = inputVertices[vertexOffset];
				const ClipVertex3D& b = inputVertices[(vertexOffset + 1) % inputVertices.count()];

				const float32 distanceA = signedDistanceFP(vec3<float32>(F.normal), F.offset, a.position);
				const float32 distanceB = signedDistanceFP(vec3<float32>(F.normal), F.offset, b.position);

				// If A and B are on different sides of the plane, keep the intersection point.
				if (distanceA * distanceB < 0.0f) {
					const float32 t = distanceA / (distanceA - distanceB);

					ClipVertex3D c{};
					c.position = lerp(a.position, b.position, t);

					outputVertices.add(c);

					if (distanceA > 0.0f) {
						c.id = a.id;
						c.id.referenceFaceAIndex = max<uint16>();
						c.id.referenceFaceBIndex = faceIndex;
					}
					else {
						c.id = b.id;
						c.id.incidentFaceAIndex = max<uint16>();
						c.id.incidentFaceBIndex = faceIndex;
					}
				}

				// If B is below the plane, keep it.
				if (distanceB <= 0.0f) {
					outputVertices.add(b);
				}

				vertexOffset += 1;
			}

			inputVertices = outputVertices;
		}

		return inputVertices;
	}
}