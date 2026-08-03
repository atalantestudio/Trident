// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/BaseSceneRenderer.hpp"

namespace Trident {
	template<typename CameraType>
	ref<Buffer> BaseSceneRenderer<CameraType>::createStaticGeometryBuffer() const {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
		const uint64 positionAlignedRangeSize = ceil(sceneAsset->geometryRanges[1].count, alignment);
		const uint64 normalAlignedRangeSize = ceil(sceneAsset->geometryRanges[2].count, alignment);
		const uint64 textureCoordinateAlignedRangeSize = ceil(sceneAsset->geometryRanges[3].count, alignment);

		return createDynamicBuffer({
			.size = max(alignment, positionAlignedRangeSize + normalAlignedRangeSize + textureCoordinateAlignedRangeSize),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Geometry",
			#endif
		});
	}

	template<typename CameraType>
	ref<Buffer> BaseSceneRenderer<CameraType>::createStaticGeometryStagingBuffer() const {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const ref<Buffer> staticGeometryStagingBuffer = createDynamicBuffer({
			.size = sceneAsset->geometryRanges[1].count + sceneAsset->geometryRanges[2].count + sceneAsset->geometryRanges[3].count,
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Geometry staging",
			#endif
		});
		const ref<BufferView<uint8>> staticGeometryView = staticGeometryStagingBuffer->createView<uint8>(0, staticGeometryStagingBuffer->getSize());

		std::copy(&sceneAsset->geometryData[sceneAsset->geometryRanges[1].offset], sceneAsset->geometryData.begin() + sceneAsset->geometryRanges[4].offset, staticGeometryView->map());

		staticGeometryStagingBuffer->unmap();

		return staticGeometryStagingBuffer;
	}
}