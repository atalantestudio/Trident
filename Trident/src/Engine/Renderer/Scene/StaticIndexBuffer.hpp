// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/BaseSceneRenderer.hpp"

namespace Trident {
	template<typename CameraType>
	ref<Buffer> BaseSceneRenderer<CameraType>::createStaticIndexBuffer() const {
		return createDynamicBuffer({
			.size = scene->getAsset()->staticIndexRange.count * sizeof(StaticVertex3D),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static index",
			#endif
		});
	}
}