// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Buffer.hpp"

namespace Trident {
	struct BLASTriangleGeometryDescriptor {
		ref<Buffer> vertexBuffer;
		ref<Buffer> indexBuffer;
	};
}