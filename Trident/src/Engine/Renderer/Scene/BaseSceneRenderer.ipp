// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/Renderer/Scene/CameraBuffer.hpp"
#include "Engine/Renderer/Scene/StaticGeometryBuffer.hpp"
#include "Engine/Renderer/Scene/StaticIndexBuffer.hpp"

namespace Trident {
	template<typename CameraType>
	void BaseSceneRenderer<CameraType>::build() {
		cameraBuffer = createCameraBuffer();

		const ref<BufferView<CameraType>> cameraView = cameraBuffer->createView<CameraType>(0, sizeof(CameraType));

		cameraMappedRange = cameraView->map();
	}

	template<typename CameraType>
	void BaseSceneRenderer<CameraType>::postSetScene() {
		staticIndexBuffer = createStaticIndexBuffer();
		staticGeometryBuffer = createStaticGeometryBuffer();

		const ref<Buffer> staticGeometryStagingBuffer = createStaticGeometryStagingBuffer();

		const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Scene one-time commands",
			#endif
		});

		commandEncoder->begin();

		copyStaticGeometry(commandEncoder, staticGeometryStagingBuffer);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		// TODO(renderer): Reuse the same command encoder.
		encodeSceneOneTimeCommands();
	}

	template<typename CameraType>
	VertexInputBinding BaseSceneRenderer<CameraType>::getStaticGeometryVertexInputBinding() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX, false);

		// Lightmap coordinate
		vertexInputBinding.registerAttribute<svec2<float32>>();

		// Vertex index, normal index, texture coordinate index
		vertexInputBinding.registerAttribute<vec4<uint32>>();

		return vertexInputBinding;
	}

	template<typename CameraType>
	void BaseSceneRenderer<CameraType>::copyStaticGeometry(ref<CommandEncoder> commandEncoder, ref<Buffer> staticGeometryStagingBuffer) const {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
		const uint64 vertexAlignedRangeSize = ceil(sceneAsset->staticVertexRange.count, alignment);
		const uint64 normalAlignedRangeSize = ceil(sceneAsset->staticNormalRange.count, alignment);

		const sequence<BufferCopyRegion> bufferCopyRegions = {
			{
				.sourceOffset = sceneAsset->staticIndexRange.offset,
				.destinationOffset = 0,
				.size = sceneAsset->staticIndexRange.count,
			},
			{
				.sourceOffset = sceneAsset->staticVertexRange.offset,
				.destinationOffset = 0,
				.size = sceneAsset->staticVertexRange.count,
			},
			{
				.sourceOffset = sceneAsset->staticNormalRange.offset,
				.destinationOffset = vertexAlignedRangeSize,
				.size = sceneAsset->staticNormalRange.count,
			},
			{
				.sourceOffset = sceneAsset->staticTextureCoordinateRange.offset,
				.destinationOffset = vertexAlignedRangeSize + normalAlignedRangeSize,
				.size = sceneAsset->staticTextureCoordinateRange.count,
			},
		};

		commandEncoder->copyBufferToBuffer(staticGeometryStagingBuffer, staticIndexBuffer, bufferCopyRegions[0]);
		// commandEncoder->copyBufferToBuffer(staticGeometryStagingBuffer, staticGeometryBuffer, view<BufferCopyRegion>(&bufferCopyRegions[1], 3));
		commandEncoder->copyBufferToBuffer(staticGeometryStagingBuffer, staticGeometryBuffer, bufferCopyRegions[1]);
		commandEncoder->copyBufferToBuffer(staticGeometryStagingBuffer, staticGeometryBuffer, bufferCopyRegions[2]);
		commandEncoder->copyBufferToBuffer(staticGeometryStagingBuffer, staticGeometryBuffer, bufferCopyRegions[3]);
	}
}