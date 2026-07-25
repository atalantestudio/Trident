// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/StaticGeometry3D.hpp"
#include "AssetBuilder/API/Scene/Builder/StaticSceneBuilder.hpp"

namespace Trident {
	class MeshSceneBuilder : public StaticSceneBuilder {
		public:
			void placeMesh(const StaticGeometryDescriptor& descriptor);
	};
}