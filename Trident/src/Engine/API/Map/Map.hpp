// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Map/PlayerSpawn.hpp"
#include "Engine/API/Geometry/Vertex.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"

namespace Trident {
	struct MapStaticGeometry {
		sequence<Vertex3D> vertices;
		sequence<uint32> indices;
	};

	/// A playable 3D level.
	struct Map : public Asset {
		MapStaticGeometry staticGeometry;
		PlayerSpawn playerSpawn;
	};
}