// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct LightmapMaterial {
		vec3<float32> emissiveColor;
		float32 emissiveIntensity;
		alignas(16) vec3<float32> albedo;
	};

	struct Lightmap {
		uint32 geometryIndex;
		uint32 materialIndex;

		/// The offset is relative to the start of the geometry.
		range<uint32, uint32> triangleRange;

		/// The offset is relative to the start of the vertex buffer.
		range<uint64, uint64> vertexRange;

		uint32 coordinateOffset;

		/// Lightmaps typically use 4 coordinates but the static geometry faces
		/// might have more vertices and/or carved holes.
		uint32 coordinateCount;

		vec2<float32> minCoordinate;
		vec2<float32> maxCoordinate;
		vec2<float32> scale;
		vec2<float32> luxelScale;
		Extent2D size;
	};

	struct LightmapLookup {
		uint32 index;
		uint32 area;
	};

	struct Rectangle {
		Extent2D offset;
		Extent2D size;
	};

	struct LightmapNode {
		LightmapNode* children[2]{};
		Rectangle rectangle;
		uint32 index = max<uint32>();

		~LightmapNode() {
			delete children[0];
			delete children[1];
		}

		bool leaf() const {
			return children[0] == nullptr && children[1] == nullptr;
		}
	};
}