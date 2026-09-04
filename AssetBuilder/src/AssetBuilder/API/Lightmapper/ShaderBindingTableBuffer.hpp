// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Buffer> Lightmapper::createShaderBindingTableBuffer() const {
		// TODO(asset-builder/lightmapper): Get the required SBT size.
		static constexpr uint64 size = 1024;

		// TODO(asset-builder/lightmapper): Create in static allocation.
		return device->createDynamicBuffer({
			.size = size,
			.usage = BufferUsage::SHADER_BINDING_TABLE,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Shader binding table",
			#endif
		});
	}
}