// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Map/Face.hpp"
#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	svec3<float32>& Face::getVertex(uint32 indexOffset) {
		return builder->facePositions[indices[indexOffset]];
	}

	Face Face::carve(view<svec3<float32>> vertices) {
		return Face(builder);
	}

	Face Face::extrude(float32 scale) {
		return Face(builder);
	}

	Face::Face(MapBuilder* builder) :
		builder(builder)
	{}
}