// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/Renderer/Scene/CameraBuffer.hpp"
#include "Engine/Renderer/Scene/StaticGeometryBuffer.hpp"

namespace Trident {
	template<typename CameraType>
	void BaseSceneRenderer<CameraType>::build() {
		cameraBuffer = createCameraBuffer();

		const ref<BufferView<CameraType>> cameraView = cameraBuffer->createView<CameraType>(0, sizeof(CameraType));

		cameraMappedRange = cameraView->map();
	}

	template<typename CameraType>
	void BaseSceneRenderer<CameraType>::postSetScene() {
		staticGeometryBuffer = createStaticGeometryBuffer();

		if (scene->getAsset()->geometryData.count() > 0) {
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
		}

		// TODO(renderer): Reuse the same command encoder.
		encodeSceneOneTimeCommands();
	}

	template<typename CameraType>
	void BaseSceneRenderer<CameraType>::copyStaticGeometry(ref<CommandEncoder> commandEncoder, ref<Buffer> staticGeometryStagingBuffer) const {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
		const uint64 positionAlignedRangeSize = ceil(sceneAsset->geometryRanges[1].count, alignment);
		const uint64 normalAlignedRangeSize = ceil(sceneAsset->geometryRanges[2].count, alignment);

		const sequence<BufferCopyRegion> bufferCopyRegions = {
			{
				.sourceOffset = 0,
				.destinationOffset = 0,
				.size = sceneAsset->geometryRanges[1].count,
			},
			{
				.sourceOffset = sceneAsset->geometryRanges[1].count,
				.destinationOffset = positionAlignedRangeSize,
				.size = sceneAsset->geometryRanges[2].count,
			},
			{
				.sourceOffset = sceneAsset->geometryRanges[1].count + sceneAsset->geometryRanges[2].count,
				.destinationOffset = positionAlignedRangeSize + normalAlignedRangeSize,
				.size = sceneAsset->geometryRanges[3].count,
			},

			// TODO(forward): Lightmap coordinate vertex buffer.
			/* {
				.sourceOffset = sceneAsset->geometryRanges[4].offset,
				.destinationOffset = 0,
				.size = sceneAsset->geometryRanges[4].count,
			}, */
		};

		commandEncoder->copyBufferToBuffer(staticGeometryStagingBuffer, staticGeometryBuffer, view<BufferCopyRegion>(&bufferCopyRegions[0], 3));
	}
}