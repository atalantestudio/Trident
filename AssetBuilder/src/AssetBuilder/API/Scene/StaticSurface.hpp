// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	struct Transform {
		/// Position of the center point in world-space.
		vec3<float32> position;

		/// Rotation in Euler angles.
		vec3<float32> rotation;

		vec2<float32> scale;
	};

	struct TextureFilteringDescriptor {
		svec2<float32> translation;
		float32 rotation;
		svec2<float32> scale;
	};

	struct SurfaceCarving {
		/// Offset from the top left.
		svec2<float32> offset;

		svec2<float32> size;
	};

	struct Quad {
		svec2<float32> start;
		svec2<float32> end;
	};

	struct StaticSurface {
		uint32 materialIndex;
		uint32 triangleOffset;
		Transform transform;
		TextureFilteringDescriptor textureFiltering;
		svec2<float32> lightmapScale;
		std::vector<SurfaceCarving> carvings;
		std::vector<Quad> quads;
	};
}