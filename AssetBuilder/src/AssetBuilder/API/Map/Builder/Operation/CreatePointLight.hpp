// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	void MapBuilder::createPointLight(const PointLight& pointLight) {
		pointLights.emplace_back(pointLight);
	}
}