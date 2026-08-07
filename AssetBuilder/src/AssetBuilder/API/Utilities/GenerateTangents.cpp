// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	void generateTangents(BuildGeometry3D& geometry) {
		sequence<svec3<float32>> bitangents(geometry.indices.count());
		uint64 indexOffset = 0;

		while (indexOffset < geometry.indices.count()) {
			BuildVertex3D& a = geometry.vertices[geometry.indices[indexOffset + 0]];
			BuildVertex3D& b = geometry.vertices[geometry.indices[indexOffset + 1]];
			BuildVertex3D& c = geometry.vertices[geometry.indices[indexOffset + 2]];

			const svec3<float32> ab = b.position - a.position;
			const svec3<float32> ac = c.position - a.position;

			const svec2<float32> uv = b.textureCoordinate - a.textureCoordinate;
			const svec2<float32> uw = c.textureCoordinate - a.textureCoordinate;

			const float32 f = 1.0f / (uv.x * uw.y - uv.y * uw.x);

			const svec3<float32> tangent = f * (ab * uw.y - ac * uv.y);
			const svec3<float32> bitangent = f * (ab * uw.x - ac * uv.x);

			a.tangent += tangent;
			b.tangent += tangent;
			c.tangent += tangent;

			bitangents[indexOffset + 0] += bitangent;
			bitangents[indexOffset + 1] += bitangent;
			bitangents[indexOffset + 2] += bitangent;

			indexOffset += 3;
		}

		uint64 vertexIndex = 0;

		while (vertexIndex < geometry.vertices.count()) {
			BuildVertex3D& vertex = geometry.vertices[vertexIndex];

			const vec3<float32> normal = vertex.normal;
			const vec3<float32> tangent = vertex.tangent;
			const vec3<float32> bitangent = bitangents[vertexIndex];
			const vec3<float32> t = normalized(tangent - (normal * dot(normal, tangent)));

			vertex.tangent = svec3<float32>(t);

			vertexIndex += 1;
		}
	}
}