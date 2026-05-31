// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	struct Transform {
		vec3<float32> position;
		vec3<float32> scale;
	};

	struct TextureFilteringDescriptor {
		svec2<float32> translation;
		float32 rotation;
		svec2<float32> scale;
	};

	struct StaticObjectDescriptor {
		view<char8> label;
		view<char8> geometryAssetName;
		Transform transform;
		TextureFilteringDescriptor textureFiltering;
	};

	struct StaticObject {
		ref<GeometryAsset> geometryAsset;
		Transform transform;
		mat3x3<float32> texture;
	};
}