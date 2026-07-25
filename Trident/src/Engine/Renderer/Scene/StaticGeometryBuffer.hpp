// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/BaseSceneRenderer.hpp"

namespace Trident {
	template<typename CameraType>
	ref<Buffer> BaseSceneRenderer<CameraType>::createStaticGeometryBuffer() const {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
		const uint64 vertexAlignedRangeSize = ceil(sceneAsset->staticVertexRange.count, alignment);
		const uint64 normalAlignedRangeSize = ceil(sceneAsset->staticNormalRange.count, alignment);
		const uint64 textureCoordinateAlignedRangeSize = ceil(sceneAsset->staticTextureCoordinateRange.count, alignment);

		return createDynamicBuffer({
			.size = vertexAlignedRangeSize + normalAlignedRangeSize + textureCoordinateAlignedRangeSize,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static geometry",
			#endif
		});
	}

	template<typename CameraType>
	ref<Buffer> BaseSceneRenderer<CameraType>::createStaticGeometryStagingBuffer() const {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const ref<Buffer> staticGeometryStagingBuffer = createDynamicBuffer({
			.size = sceneAsset->staticGeometryBuffer.count(),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static geometry staging",
			#endif
		});
		const ref<BufferView<uint8>> staticGeometryView = staticGeometryStagingBuffer->createView<uint8>(0, staticGeometryStagingBuffer->getSize());

		std::copy(sceneAsset->staticGeometryBuffer.begin(), sceneAsset->staticGeometryBuffer.end(), staticGeometryView->map());

		staticGeometryStagingBuffer->unmap();

		return staticGeometryStagingBuffer;
	}
}