// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer> ForwardRenderer::createLightmapCoordinateBuffer() const {
		return createDynamicBuffer({
			.size = max(sizeof(svec2<float32>), scene->getAsset()->geometryRanges[4].count),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap coordinate",
			#endif
		});
	}
}