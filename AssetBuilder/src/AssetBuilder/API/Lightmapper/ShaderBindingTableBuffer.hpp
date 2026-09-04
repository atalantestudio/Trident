// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	void Lightmapper::createShaderBindingTableBuffer() {
		sequence<uint8> shaderGroupHandleData(limits->shaderGroupHandleSize * 3);

		rayTracingPipeline->getShaderGroupHandles({0, 3}, &shaderGroupHandleData[0]);

		const uint32 shaderSize = ceil(limits->shaderGroupHandleSize, limits->shaderGroupHandleAlignment);
		const uint32 rayGenerationShaderOffset = 0;
		const uint32 rayClosestHitShaderOffset = ceil(rayGenerationShaderOffset + shaderSize, limits->shaderGroupBaseAlignment);
		const uint32 rayMissShaderOffset = ceil(rayClosestHitShaderOffset + shaderSize, limits->shaderGroupBaseAlignment);
		const uint32 callableShaderOffset = rayMissShaderOffset;
		const uint64 size = callableShaderOffset + shaderSize;

		// TODO(asset-builder/lightmapper): Create in a static allocation.
		shaderBindingTableBuffer = device->createDynamicBuffer({
			.size = size,
			.usage = BufferUsage::SHADER_BINDING_TABLE,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Shader binding table",
			#endif
		});

		const BufferView<uint8> shaderBindingTableView = shaderBindingTableBuffer->createView<uint8>(0, shaderBindingTableBuffer->getSize());
		uint8* shaderBindingTableMappedRange = shaderBindingTableView.map();

		std::copy_n(&shaderGroupHandleData[shaderSize * 0], shaderSize, shaderBindingTableMappedRange + rayGenerationShaderOffset);
		std::copy_n(&shaderGroupHandleData[shaderSize * 1], shaderSize, shaderBindingTableMappedRange + rayClosestHitShaderOffset);
		std::copy_n(&shaderGroupHandleData[shaderSize * 2], shaderSize, shaderBindingTableMappedRange + rayMissShaderOffset);

		shaderBindingTableBuffer->unmap();

		// TODO(asset-builder/lightmapper): Update device address regions.

		ENGINE_ERROR_UNIMPLEMENTED();
	}
}