// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/StaticGeometry3D.hpp"

namespace Trident {
	struct TextureFilteringDescriptor {
		svec2<float32> translation;
		float32 rotation;
		svec2<float32> scale;
	};

	struct Carving {
		/// Offset from the top left.
		svec2<float32> offset;

		svec2<float32> size;
	};

	struct SubsurfaceQuad {
		svec2<float32> start;
		svec2<float32> end;
	};

	struct SurfaceDescriptor {
		StaticGeometry3D geometry;
		TextureFilteringDescriptor textureFiltering;
		uint32 materialIndex;
		svec2<float32> luxelScale;
		std::vector<Carving> carvings;
	};

	struct Transform {
		/// Position of the center point in world-space.
		vec3<float32> position;

		/// Rotation axis.
		svec3<float32> axis;

		/// Rotation angle in radians.
		float32 angle;
	};

	struct StaticSubsurface {
		sequence<float32> vertexComponents;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> vertexIndices;
		sequence<uint32> vertexTextureCoordinateIndices;
		svec2<float32> scale;
	};

	struct StaticSurface {
		/// Each edge has an origin vertex index.
		sequence<uint16> edges;

		svec3<float32> normal;
		svec2<float32> scale;
		std::vector<StaticSubsurface> subsurfaces;
		TextureFilteringDescriptor textureFiltering;
		uint32 triangleOffset;
		uint32 materialIndex;
		svec2<float32> luxelScale;
	};
}