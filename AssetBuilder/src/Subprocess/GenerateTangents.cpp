// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "GenerateTangents.hpp"

namespace Trident {
	sequence<svec3<float32>> generateTangents(view<svec3<float32>> positions, view<svec3<float32>> normals, view<svec2<float32>> textureCoordinates, view<GeometryIndex3D> vertices) {
		// TODO(asset-builder/geometry): Finish generating tangents.

		/* const uint64 tangentCount = normals.count();

		sequence<svec3<float32>> tangents(vertices.count());
		sequence<svec3<float32>> bitangents(vertices.count());
		uint32 vertexOffset = 0;

		while (vertexOffset < vertices.count()) {
			const GeometryIndex3D a = vertices[vertexOffset + 0];
			const GeometryIndex3D b = vertices[vertexOffset + 1];
			const GeometryIndex3D c = vertices[vertexOffset + 2];

			const svec3<float32> edgeAB = positions[b.positionIndex] - positions[a.positionIndex];
			const svec3<float32> edgeAC = positions[c.positionIndex] - positions[a.positionIndex];

			const svec2<float32> uvAB = textureCoordinates[b.textureCoordinateIndex] - textureCoordinates[a.textureCoordinateIndex];
			const svec2<float32> uvAC = textureCoordinates[c.textureCoordinateIndex] - textureCoordinates[a.textureCoordinateIndex];

			const float32 f = 1.0f / (uvAB.x * uvAC.y - uvAB.y * uvAC.x);

			const svec3<float32> tangent = f * (edgeAB * uvAC.y - edgeAC * uvAB.y);
			const svec3<float32> bitangent = f * (edgeAB * uvAC.x - edgeAC * uvAB.x);

			tangents[vertexOffset + 0] += tangent;
			tangents[vertexOffset + 1] += tangent;
			tangents[vertexOffset + 2] += tangent;

			bitangents[vertexOffset + 0] += bitangent;
			bitangents[vertexOffset + 1] += bitangent;
			bitangents[vertexOffset + 2] += bitangent;

			vertexOffset += 3;
		}

		uint32 vertexIndex = 0;

		while (vertexIndex < vertices.count()) {
			const GeometryIndex3D vertex = vertices[vertexIndex];
			const vec3<float32> normal = normals[vertex.normalIndex];
			const vec3<float32> tangent = tangents[vertexIndex];
			const vec3<float32> bitangent = bitangents[vertexIndex];

			const vec3<float32> t = normalized(tangent - (normal * dot(normal, tangent)));
			const vec3<float32> c = cross(normal, tangent);

			float32 w = 1.0f;

			if (dot(c, bitangent) < 0.0f) {
				w = -1.0f;
			}

			tangents[vertexIndex] = svec3<float32>(t * w);

			vertexIndex += 1;
		} */

		/* sequence<svec3<float32>> finalTangents(tangentCount);

		vertexIndex = 0;

		while (vertexIndex < vertices.count()) {
			const GeometryIndex3D vertex = vertices[vertexIndex];
			const svec3<float32> tangent = tangents[vertexIndex];

			finalTangents[vertex.normalIndex] = tangent;

			vertexIndex += 1;
		} */

		sequence<svec3<float32>> tangents(vertices.count() / 3);
		uint32 vertexOffset = 0;

		while (vertexOffset < vertices.count()) {
			const GeometryIndex3D a = vertices[vertexOffset + 0];
			const GeometryIndex3D b = vertices[vertexOffset + 1];
			const GeometryIndex3D c = vertices[vertexOffset + 2];
			const svec3<float32> pA = positions[a.positionIndex];
			const svec3<float32> pB = positions[b.positionIndex];
			const svec3<float32> pC = positions[c.positionIndex];
			const svec2<float32> uvA = textureCoordinates[a.textureCoordinateIndex];
			const svec2<float32> uvB = textureCoordinates[b.textureCoordinateIndex];
			const svec2<float32> uvC = textureCoordinates[c.textureCoordinateIndex];

			const svec3<float32> delta_pB = pB - pA;
			const svec3<float32> delta_pC = pC - pA;
			const svec2<float32> delta_uvB = uvB - uvA;
			const svec2<float32> delta_uvC = uvC - uvA;
			const float32 f = 1.0f / (delta_uvB[0] * delta_uvC[1] - delta_uvC[0] * delta_uvB[1]);

			const svec3<float32> tangent = f * (delta_uvC[1] * delta_pB - delta_uvB[1] * delta_pC);

			tangents[vertexOffset / 3] = tangent;

			vertexOffset += 3;
		}

		return tangents;
	}
}