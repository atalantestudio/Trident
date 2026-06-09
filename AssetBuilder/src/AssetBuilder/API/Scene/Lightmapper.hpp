// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/StaticVertex.hpp"

#include "AssetBuilder/API/Scene/Lightmap.hpp"

namespace Trident {
	class Lightmapper {
		private:
			static constexpr Extent2D ATLAS_SIZE = {2048, 2048};

		public:
			static void updateAtlasCoordinates(view<Lightmap> lightmaps, sequence<StaticTriangle3D>& triangles);
	};
}