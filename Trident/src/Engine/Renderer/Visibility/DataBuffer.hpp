// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<uint8>> VisibilityRenderer::createDataBuffer() const {
		return createBuffer<uint8>({
			.size =
				vertexPositionComponentRegionSize +
				vertexNormalComponentRegionSize +
				vertexTextureCoordinateComponentRegionSize +
				vertexPositionIndexRegionSize +
				vertexNormalIndexRegionSize +
				vertexTextureCoordinateIndexRegionSize +
				clusterRegionSize +
				geometryRegionSize,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Data",
			#endif
		});
	}

	void VisibilityRenderer::updateDataRegionSizes() {
		const uint64 minStorageBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		vertexPositionComponentRegionSize = ceil(scene->vertexPositionComponents.count() * sizeof(float32), minStorageBufferOffsetAlignment);
		vertexNormalComponentRegionSize = ceil(scene->vertexNormalComponents.count() * sizeof(float32), minStorageBufferOffsetAlignment);
		vertexTextureCoordinateComponentRegionSize = ceil(scene->vertexTextureCoordinateComponents.count() * sizeof(float32), minStorageBufferOffsetAlignment);
		vertexPositionIndexRegionSize = ceil(scene->vertexPositionIndices.count() * sizeof(uint32), minStorageBufferOffsetAlignment);
		vertexNormalIndexRegionSize = ceil(scene->vertexNormalIndices.count() * sizeof(uint32), minStorageBufferOffsetAlignment);
		vertexTextureCoordinateIndexRegionSize = ceil(scene->vertexTextureCoordinateIndices.count() * sizeof(uint32), minStorageBufferOffsetAlignment);
		clusterRegionSize = ceil(scene->clusters.count() * sizeof(Cluster), minStorageBufferOffsetAlignment);
		geometryRegionSize = ceil(scene->geometries.count() * sizeof(Geometry), minStorageBufferOffsetAlignment);
	}
}