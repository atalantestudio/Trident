// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

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
			const vec4<float32> t = {tangent.x, tangent.y, tangent.z, 0.0f};

			a.tangent += t;
			b.tangent += t;
			c.tangent += t;

			bitangents[geometry.indices[indexOffset + 0]] += bitangent;
			bitangents[geometry.indices[indexOffset + 1]] += bitangent;
			bitangents[geometry.indices[indexOffset + 2]] += bitangent;

			indexOffset += 3;
		}

		uint64 vertexIndex = 0;

		while (vertexIndex < geometry.vertices.count()) {
			BuildVertex3D& vertex = geometry.vertices[vertexIndex];

			const vec3<float32> normal = vertex.normal;
			const vec3<float32> tangent = {vertex.tangent[0], vertex.tangent[1], vertex.tangent[2]};
			const vec3<float32> bitangent = bitangents[vertexIndex];
			const vec3<float32> t = normalized(tangent - (normal * dot(normal, tangent)));

			// TODO(asset-builder/geometry/tangents): Use to flip the tangent.
			float32 w = 1.0f;

			if (dot(cross(normal, tangent), bitangent) < 0.0f) {
				w = -1.0f;
			}

			vertex.tangent = vec4<float32>(t[0], t[1], t[2], w);

			vertexIndex += 1;
		}
	}
}