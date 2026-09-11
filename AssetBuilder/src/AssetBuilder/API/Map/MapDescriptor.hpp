// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"

namespace Trident {
	struct SunLight {
		// TODO(asset-builder/map)
	};

	struct MapDescriptor {
		ref<ImageAsset> environmentMapImageAsset;

		sequence<ref<Geometry3DAsset>> geometryAssets;

		sequence<ref<MaterialAsset>> staticMaterialAssets;
		sequence<ref<MaterialAsset>> dynamicMaterialAssets;

		SunLight sunLight;
	};
}