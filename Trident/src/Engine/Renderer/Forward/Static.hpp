// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer> ForwardRenderer::createStaticBuffer() const {
		const ref<Device> device = getDevice();
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const uint64 minStorageBufferOffsetAlignment = device->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
		const uint64 triangleRangeSize = sceneAsset->getStaticTriangles().count() * sizeof(StaticTriangle3D);
		const uint64 vertexRangeSize = sceneAsset->getStaticVertices().count() * sizeof(svec3<float32>);
		const uint64 normalRangeSize = sceneAsset->getStaticNormals().count() * sizeof(svec3<float32>);
		const uint64 textureCoordinateRangeSize = sceneAsset->getStaticTextureCoordinates().count() * sizeof(svec2<float32>);
		const uint64 triangleAlignedRangeSize = ceil(triangleRangeSize, minStorageBufferOffsetAlignment);
		const uint64 vertexAlignedRangeSize = ceil(vertexRangeSize, minStorageBufferOffsetAlignment);
		const uint64 normalAlignedRangeSize = ceil(normalRangeSize, minStorageBufferOffsetAlignment);
		const uint64 textureCoordinateAlignedRangeSize = ceil(textureCoordinateRangeSize, minStorageBufferOffsetAlignment);
		const ref<Buffer> staticStagingBuffer = createDynamicBuffer({
			.size = triangleRangeSize + vertexRangeSize + normalRangeSize + textureCoordinateRangeSize,
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const ref<BufferView<StaticTriangle3D>> triangleView = staticStagingBuffer->createView<StaticTriangle3D>(0, triangleRangeSize);
		const ref<BufferView<svec3<float32>>> vertexView = staticStagingBuffer->createView<svec3<float32>>(triangleView->getOffset() + triangleView->getSize(), vertexRangeSize);
		const ref<BufferView<svec3<float32>>> normalView = staticStagingBuffer->createView<svec3<float32>>(vertexView->getOffset() + vertexView->getSize(), normalRangeSize);
		const ref<BufferView<svec2<float32>>> textureCoordinateView = staticStagingBuffer->createView<svec2<float32>>(normalView->getOffset() + normalView->getSize(), textureCoordinateRangeSize);

		std::copy(sceneAsset->getStaticTriangles().begin(), sceneAsset->getStaticTriangles().end(), triangleView->map());
		std::copy(sceneAsset->getStaticVertices().begin(), sceneAsset->getStaticVertices().end(), vertexView->map());
		std::copy(sceneAsset->getStaticNormals().begin(), sceneAsset->getStaticNormals().end(), normalView->map());
		std::copy(sceneAsset->getStaticTextureCoordinates().begin(), sceneAsset->getStaticTextureCoordinates().end(), textureCoordinateView->map());

		staticStagingBuffer->unmap();

		const ref<Buffer> staticBuffer = createDynamicBuffer({
			.size = triangleAlignedRangeSize + vertexAlignedRangeSize + normalAlignedRangeSize + textureCoordinateAlignedRangeSize,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static",
			#endif
		});

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy static buffer",
			#endif
		});

		commandEncoder->begin();

		const sequence<BufferCopyRegion> bufferCopyRegions = {
			// Triangles
			{
				.sourceOffset = triangleView->getOffset(),
				.destinationOffset = 0,
				.size = triangleView->getSize(),
			},

			// Vertices
			{
				.sourceOffset = vertexView->getOffset(),
				.destinationOffset = triangleAlignedRangeSize,
				.size = vertexView->getSize(),
			},

			// Normals
			{
				.sourceOffset = normalView->getOffset(),
				.destinationOffset = triangleAlignedRangeSize + vertexAlignedRangeSize,
				.size = normalView->getSize(),
			},

			// Texture coordinates
			{
				.sourceOffset = textureCoordinateView->getOffset(),
				.destinationOffset = triangleAlignedRangeSize + vertexAlignedRangeSize + normalAlignedRangeSize,
				.size = textureCoordinateView->getSize(),
			},
		};

		commandEncoder->copyBufferToBuffer(staticStagingBuffer, staticBuffer, bufferCopyRegions);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		return staticBuffer;
	}
}