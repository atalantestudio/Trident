// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct TextureFilteringDescriptor {
		svec2<float32> translation;
		float32 rotation;
		svec2<float32> scale;
	};

	struct StaticObjectDescriptor {
		view<char8> label;
		sequence<svec3<float32>> vertices;
		TextureFilteringDescriptor textureFiltering;
	};

	struct StaticObject {
		sequence<uint16> vertexPositionIndices;
		mat3x3<float32> texture;
	};
}