// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/SutherlandHodgman3D.hpp"

namespace Trident {
	static Plane3D getSidePlane(const RigidBody3DComponent& body, const Hull3D& hull, Plane3D plane, index edgeOffset) {
		const uint16 edgeIndexA = hull.faceEdgeIndices[plane.edgeIndexRange.offset + edgeOffset];
		const uint16 edgeIndexB = hull.faceEdgeIndices[plane.edgeIndexRange.offset + (edgeOffset + 1) % plane.edgeIndexRange.count];
		const Edge3D edgeA = hull.edges[edgeIndexA];
		const Edge3D edgeB = hull.edges[edgeIndexB];
		const vec3<float32> a = hull.vertices[edgeA.originVertexIndex] * body.world;
		const vec3<float32> b = hull.vertices[edgeB.originVertexIndex] * body.world;

		Plane3D sidePlane;
		sidePlane.normal = normalized(cross(normalized(b - a), plane.normal));
		sidePlane.distance = dot(sidePlane.normal, a);
		sidePlane.edgeIndexRange.offset = 0;
		sidePlane.edgeIndexRange.count = 0;

		return *pointer_cast<const Plane3D*>(&sidePlane);
	}

	pool<ClipVertex3D, 8> SutherlandHodgman3D(const RigidBody3DComponent& referenceBody, const Hull3D& referenceHull, uint16 referenceFaceIndex, pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> inputVertices) {
		const Plane3D referenceFace = referenceBody.facePlanes[referenceFaceIndex];

		index planeIndex = 0;

		while (planeIndex < referenceFace.edgeIndexRange.count) {
			const Plane3D plane = getSidePlane(referenceBody, referenceHull, referenceFace, planeIndex);

			pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> outputVertices;
			index vertexOffset = 0;

			while (vertexOffset < inputVertices.count()) {
				const ClipVertex3D& a = inputVertices[vertexOffset];
				const ClipVertex3D& b = inputVertices[(vertexOffset + 1) % inputVertices.count()];

				const float32 distanceA = signedDistance(plane, a.position);
				const float32 distanceB = signedDistance(plane, b.position);

				// If A and B are on different sides of the plane, keep the intersection point.
				if (distanceA * distanceB < 0.0f) {
					const float32 t = distanceA / (distanceA - distanceB);

					ClipVertex3D c;
					c.position = lerp(a.position, b.position, t);
					c.separation = 0.0f;

					if (distanceA > 0.0f) {
						c.id = a.id;
						c.id.referenceFaceAIndex = max<uint16>();
						c.id.referenceFaceBIndex = referenceFaceIndex;
					}
					else {
						c.id = b.id;
						c.id.incidentFaceAIndex = max<uint16>();
						c.id.incidentFaceBIndex = referenceFaceIndex;
					}

					outputVertices.add(c);
				}

				// If B is below the plane, keep it.
				if (distanceB <= 0.0f) {
					outputVertices.add(b);
				}

				vertexOffset += 1;
			}

			inputVertices = outputVertices;

			planeIndex += 1;
		}

		return inputVertices;
	}
}