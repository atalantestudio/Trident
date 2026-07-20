// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSurface.hpp"

namespace Trident {
	struct Room {	
		sequence<SurfaceDescriptor> surfaceDescriptors;
		sequence<uint32> surfaceIndices;
	};
}