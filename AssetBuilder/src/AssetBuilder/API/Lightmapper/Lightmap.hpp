// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct LightmapMaterial {
		svec3<float32> emissiveColor;
		float32 emissiveIntensity;
		svec3<float32> albedo;
		uint32 _;
	};

	struct Lightmap {
		uint32 geometryIndex;
		range<uint64, uint64> vertexRange;
		uint32 coordinateOffset;
		uint32 triangleOffset;
		svec2<float32> scale;
		svec2<float32> maxCoordinate;
		svec2<float32> minCoordinate;
		svec2<float32> luxelScale;
		Extent2D size;
	};

	struct LightmapLookup {
		uint32 index;
		uint32 area;
	};

	struct Rectangle {
		uint32 x;
		uint32 y;
		uint32 width;
		uint32 height;
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