// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer<uint8>> ForwardRenderer::createStaticBuffer() const {
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

		const ref<Buffer<uint8>> staticStagingBuffer = createStagingBuffer<uint8>(triangleRangeSize + vertexRangeSize + normalRangeSize + textureCoordinateRangeSize);

		uint8* mappedRange = staticStagingBuffer->map(0, staticStagingBuffer->getSize());

		// Copy triangles.
		{
			std::copy(scene->getAsset()->getStaticTriangles().begin(), scene->getAsset()->getStaticTriangles().end(), pointer_cast<StaticTriangle3D*>(mappedRange));

			mappedRange += triangleRangeSize;
		}

		// Copy vertices.
		{
			std::copy(scene->getAsset()->getStaticVertices().begin(), scene->getAsset()->getStaticVertices().end(), pointer_cast<svec3<float32>*>(mappedRange));

			mappedRange += vertexRangeSize;
		}

		// Copy normals.
		{
			std::copy(scene->getAsset()->getStaticNormals().begin(), scene->getAsset()->getStaticNormals().end(), pointer_cast<svec3<float32>*>(mappedRange));

			mappedRange += normalRangeSize;
		}

		// Copy texture coordinates.
		{
			std::copy(scene->getAsset()->getStaticTextureCoordinates().begin(), scene->getAsset()->getStaticTextureCoordinates().end(), pointer_cast<svec2<float32>*>(mappedRange));

			mappedRange += textureCoordinateRangeSize;
		}

		staticStagingBuffer->unmap();

		const ref<Buffer<uint8>> staticBuffer = createBuffer<uint8>({
			.size = triangleAlignedRangeSize + vertexAlignedRangeSize + normalAlignedRangeSize + textureCoordinateAlignedRangeSize,
			.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST,
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
				.name = "Static",
			#endif
		});

		commandEncoder->begin();

		const sequence<BufferCopyRegion> bufferCopyRegions = {
			// Triangles
			{
				.sourceOffset = 0,
				.destinationOffset = 0,
				.size = triangleRangeSize,
			},

			// Vertices
			{
				.sourceOffset = triangleRangeSize,
				.destinationOffset = triangleAlignedRangeSize,
				.size = vertexRangeSize,
			},

			// Normals
			{
				.sourceOffset = triangleRangeSize + vertexRangeSize,
				.destinationOffset = triangleAlignedRangeSize + vertexAlignedRangeSize,
				.size = normalRangeSize,
			},

			// Texture coordinates
			{
				.sourceOffset = triangleRangeSize + vertexRangeSize + normalRangeSize,
				.destinationOffset = triangleAlignedRangeSize + vertexAlignedRangeSize + normalAlignedRangeSize,
				.size = textureCoordinateRangeSize,
			},
		};

		commandEncoder->copyBufferToBuffer(staticStagingBuffer, staticBuffer, bufferCopyRegions);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		return staticBuffer;
	}
}