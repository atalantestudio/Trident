// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct StaticGeometry3D {
		sequence<float32> vertexComponents;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> vertexIndices;
		sequence<uint32> vertexTextureCoordinateIndices;
		sequence<uint16> edgeOriginVertexIndices;
		vec3<float32> normal;
		svec2<float32> scale;
	};
}