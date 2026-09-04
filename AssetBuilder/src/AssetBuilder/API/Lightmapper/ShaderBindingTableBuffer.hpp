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
		const uint32 rayHitShaderOffset = ceil(rayGenerationShaderOffset + shaderSize, limits->shaderGroupBaseAlignment);
		const uint32 rayMissShaderOffset = ceil(rayHitShaderOffset + shaderSize, limits->shaderGroupBaseAlignment);
		const uint32 rayCallableShaderOffset = rayMissShaderOffset;
		const uint64 size = rayCallableShaderOffset + shaderSize;

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
		std::copy_n(&shaderGroupHandleData[shaderSize * 1], shaderSize, shaderBindingTableMappedRange + rayHitShaderOffset);
		std::copy_n(&shaderGroupHandleData[shaderSize * 2], shaderSize, shaderBindingTableMappedRange + rayMissShaderOffset);

		shaderBindingTableBuffer->unmap();

		const uint64 deviceAddress = device->getBufferDeviceAddress(shaderBindingTableBuffer);

		rayGenerationShaderBindingTable = {
			.deviceAddress = deviceAddress + rayGenerationShaderOffset,
			.stride = shaderSize,
			.size = shaderSize,
		};
		rayMissShaderBindingTable = {
			.deviceAddress = deviceAddress + rayMissShaderOffset,
			.stride = shaderSize,
			.size = shaderSize,
		};
		rayHitShaderBindingTable = {
			.deviceAddress = deviceAddress + rayHitShaderOffset,
			.stride = shaderSize,
			.size = shaderSize,
		};
		rayCallableShaderBindingTable = {
			.deviceAddress = 0,
			.stride = 0,
			.size = 0,
		};
	}
}