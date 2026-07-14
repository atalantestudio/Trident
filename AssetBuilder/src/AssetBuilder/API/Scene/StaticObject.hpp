// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	struct Transform {
		vec3<float32> position;
		vec3<float32> rotation;

		/// Applied after the rotation.
		vec3<float32> scale;
	};

	struct TextureFilteringDescriptor {
		svec2<float32> translation;
		float32 rotation;
		svec2<float32> scale;
	};

	struct StaticObject {
		view<char8> label;
		ref<GeometryAsset> geometryAsset;
		Transform transform;
		TextureFilteringDescriptor textureFiltering;
		uint32 materialIndex;

		/// Applies to all faces.
		svec2<float32> lightmapPixelScale;
	};
}