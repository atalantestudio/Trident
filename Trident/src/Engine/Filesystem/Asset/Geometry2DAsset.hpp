// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2DType.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAssetFlags.hpp"

namespace Trident {
	struct Geometry2DAsset : public Asset {
		uint32 getPositionCount() const {
			return static_cast<uint32>(vertexPositionComponents.count() / 2);
		}

		uint32 getTextureCoordinateCount() const {
			return static_cast<uint32>(vertexTextureCoordinateComponents.count() / 2);
		}

		GeometryAssetFlags flags;
		sequence<float32> vertexPositionComponents;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> vertexPositionIndices;

		Hull2DType hullType;
		float32 hullRadius;
		vec2<float32> hullCentroid;
		sequence<uint32> hullLineIndices;
	};
}