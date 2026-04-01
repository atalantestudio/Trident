// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/Visibility/CameraBuffer.hpp"
#include "Engine/Renderer/Visibility/CameraDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/ClusterDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/DataBuffer.hpp"
#include "Engine/Renderer/Visibility/DebugDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/DebugPipeline.hpp"
#include "Engine/Renderer/Visibility/DebugPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/DepthStencilImage.hpp"
#include "Engine/Renderer/Visibility/InstanceBuffer.hpp"
#include "Engine/Renderer/Visibility/InstanceDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialBaseDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialCoordinateBuffer.hpp"
#include "Engine/Renderer/Visibility/MaterialCoordinateDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialCoordinatePipeline.hpp"
#include "Engine/Renderer/Visibility/MaterialCoordinatePipelineLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialCountBuffer.hpp"
#include "Engine/Renderer/Visibility/MaterialCountDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialCountPipeline.hpp"
#include "Engine/Renderer/Visibility/MaterialCountPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialImageSampler.hpp"
#include "Engine/Renderer/Visibility/MaterialIndirectBuffer.hpp"
#include "Engine/Renderer/Visibility/MaterialObjects.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetBuffer.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetPipeline.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/OutputDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/OutputImage.hpp"
#include "Engine/Renderer/Visibility/OutputPipeline.hpp"
#include "Engine/Renderer/Visibility/OutputPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/PresentFramebuffers.hpp"
#include "Engine/Renderer/Visibility/PresentRenderPass.hpp"
#include "Engine/Renderer/Visibility/Renderer.hpp"
#include "Engine/Renderer/Visibility/VisibilityFramebuffer.hpp"
#include "Engine/Renderer/Visibility/VisibilityImage.hpp"
#include "Engine/Renderer/Visibility/VisibilityIndirectBuffer.hpp"
#include "Engine/Renderer/Visibility/VisibilityPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/VisibilityPipeline.hpp"
#include "Engine/Renderer/Visibility/VisibilityRenderPass.hpp"

namespace Trident {
	void VisibilityRenderer::build() {
		visibilityRenderPass = createVisibilityRenderPass();
		presentRenderPass = createPresentRenderPass();

		createFrameResources();

		cameraDescriptorSetLayout = createCameraDescriptorSetLayout();
		clusterDescriptorSetLayout = createClusterDescriptorSetLayout();
		instanceDescriptorSetLayout = createInstanceDescriptorSetLayout();
		materialCountDescriptorSetLayout = createMaterialCountDescriptorSetLayout();
		materialOffsetDescriptorSetLayout = createMaterialOffsetDescriptorSetLayout();
		materialCoordinateDescriptorSetLayout = createMaterialCoordinateDescriptorSetLayout();
		materialBaseDescriptorSetLayout = createMaterialBaseDescriptorSetLayout();
		outputDescriptorSetLayout = createOutputDescriptorSetLayout();
		debugDescriptorSetLayout = createDebugDescriptorSetLayout();

		allocateDescriptorSets();

		visibilityPipelineLayout = createVisibilityPipelineLayout();
		materialCountPipelineLayout = createMaterialCountPipelineLayout();
		materialOffsetPipelineLayout = createMaterialOffsetPipelineLayout();
		materialCoordinatePipelineLayout = createMaterialCoordinatePipelineLayout();
		outputPipelineLayout = createOutputPipelineLayout();
		// debugPipelineLayout = createDebugPipelineLayout();

		visibilityPipeline = createVisibilityPipeline();
		materialCountPipeline = createMaterialCountPipeline();
		materialOffsetPipeline = createMaterialOffsetPipeline();
		materialCoordinatePipeline = createMaterialCoordinatePipeline();
		outputPipeline = createOutputPipeline();
		// debugPipeline = createDebugPipeline();

		visibilityIndirectBuffer = createVisibilityIndirectBuffer();

		cameraBuffer = createCameraBuffer();
		cameraMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());

		materialImageSampler = createMaterialImageSampler();

		updateViewport();
	}

	void VisibilityRenderer::postSetScene() {
		updateDataRegionSizes();

		dataBuffer = createDataBuffer();
		instanceBuffer = createInstanceBuffer();
		materialCountBuffer = createMaterialCountBuffer();
		materialOffsetBuffer = createMaterialOffsetBuffer();
		materialIndirectBuffer = createMaterialIndirectBuffer();

		instanceMappedRange = instanceBuffer->map(0, instanceBuffer->getSize());

		writeVisibilityIndirectCommand();

		visibilityIndirectBuffer->unmap();

		fillSceneBuffers();

		updateBufferDescriptorSets();
		updateDebugInputAttachmentDescriptorSets();

		createMaterialObjects();
	}

	void VisibilityRenderer::postSetCameraController() {
		const PerspectiveCamera& camera = getCameraController<PerspectiveCamera>().getCamera();

		cameraMappedRange->nearPlane = camera.getNearPlane();
		cameraMappedRange->farPlane = camera.getFarPlane();
	}

	void VisibilityRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const Extent2D extent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(extent.width);
		const float32 height = static_cast<float32>(extent.height);

		// Visibility render pass
		{
			const ClearValue visibilityClearValue = {0.0f, 0.0f, 0.0f, 1.0f};
			const ClearValue outputClearValue = {0.0f, 0.0f, 0.0f, 1.0f};

			ClearValue depthStencilClearValue{};
			depthStencilClearValue.depth = 1.0f;
			depthStencilClearValue.stencil = 0;

			BeginRenderPassDescriptor beginRenderPassDescriptor{};
			beginRenderPassDescriptor.framebuffer = visibilityFramebuffer;
			beginRenderPassDescriptor.clearValues = {visibilityClearValue, outputClearValue, depthStencilClearValue};

			commandEncoder->beginRenderPass(beginRenderPassDescriptor);
			commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
			commandEncoder->setScissor(0, 0, extent);

			// Write pixel visibility and count per material.
			commandEncoder->bindGraphicsPipeline(visibilityPipeline);
			commandEncoder->bindDescriptorSets(visibilityPipelineLayout, 0, {cameraDescriptorSet, clusterDescriptorSet, instanceDescriptorSet});
			commandEncoder->drawIndirect(visibilityIndirectBuffer, 0, 1, 0);

			commandEncoder->endRenderPass();
		}

		commandEncoder->setImageMemoryBarrier(visibilityImage, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
		commandEncoder->setImageMemoryBarrier(outputImage, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		// Material compute pass
		{
			const uint32 groupCountX = static_cast<uint32>(ceil(width / 16.0f));
			const uint32 groupCountY = static_cast<uint32>(ceil(height / 16.0f));

			// Count the number of pixels used by each material.
			commandEncoder->bindComputePipeline(materialCountPipeline);
			commandEncoder->bindDescriptorSets(materialCountPipelineLayout, 0, {materialCountDescriptorSet});
			commandEncoder->dispatch(groupCountX, groupCountY, 1);

			BufferMemoryBarrier bufferMemoryBarrier{};
			bufferMemoryBarrier.buffer = materialCountBuffer;
			bufferMemoryBarrier.offset = 0;
			bufferMemoryBarrier.size = materialCountBuffer->getSize();
			bufferMemoryBarrier.sourceAccess = Access::SHADER_WRITE;
			bufferMemoryBarrier.destinationAccess = Access::SHADER_READ | Access::SHADER_WRITE;

			commandEncoder->insertBufferMemoryBarriers({bufferMemoryBarrier});

			// Get the material pixel offsets with a prefix sum and write indirect arguments for each material pipeline.
			commandEncoder->bindComputePipeline(materialOffsetPipeline);
			commandEncoder->bindDescriptorSets(materialOffsetPipelineLayout, 0, {materialOffsetDescriptorSet});
			commandEncoder->dispatch(1, 1, 1);

			// Re-order the pixels per material index.
			commandEncoder->bindComputePipeline(materialCoordinatePipeline);
			commandEncoder->bindDescriptorSets(materialCoordinatePipelineLayout, 0, {materialCoordinateDescriptorSet});
			commandEncoder->dispatch(groupCountX, groupCountY, 1);

			bufferMemoryBarrier.buffer = materialOffsetBuffer;
			bufferMemoryBarrier.offset = 0;
			bufferMemoryBarrier.size = materialOffsetBuffer->getSize();
			bufferMemoryBarrier.sourceAccess = Access::SHADER_WRITE;
			bufferMemoryBarrier.destinationAccess = Access::SHADER_READ;

			commandEncoder->insertBufferMemoryBarriers({bufferMemoryBarrier});

			uint32 materialIndex = 0;

			while (materialIndex < scene->materials.count()) {
				commandEncoder->bindComputePipeline(materialPipelines[materialIndex]);
				commandEncoder->bindDescriptorSets(materialPipelineLayouts[materialIndex], 0, {materialBaseDescriptorSet});

				// TODO: Use the material reference index to access the correct image descriptor layers.
				commandEncoder->dispatchIndirect(materialIndirectBuffer, sizeof(DispatchIndirectCommand) * materialIndex);

				materialIndex += 1;
			}
		}

		// Present render pass
		{
			const ClearValue swapchainClearValue = {0.0f, 0.0f, 0.0f, 1.0f};

			BeginRenderPassDescriptor beginRenderPassDescriptor{};
			beginRenderPassDescriptor.framebuffer = presentFramebuffers[getSwapchainImageIndex()];
			beginRenderPassDescriptor.clearValues = {swapchainClearValue};

			commandEncoder->beginRenderPass(beginRenderPassDescriptor);
			commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
			commandEncoder->setScissor(0, 0, extent);

			#if 1
				// Present the output image.
				commandEncoder->bindGraphicsPipeline(outputPipeline);
				commandEncoder->bindDescriptorSets(outputPipelineLayout, 0, {outputDescriptorSet});
				commandEncoder->draw(3, 1, 0, 0);
			#else
				commandEncoder->bindGraphicsPipeline(debugPipeline);
				commandEncoder->bindDescriptorSets(debugPipelineLayout, 0, {debugDescriptorSet, clusterDescriptorSet, instanceDescriptorSet});
				commandEncoder->draw(3, 1, 0, 0);
			#endif

			commandEncoder->endRenderPass();
		}
	}

	void VisibilityRenderer::resize(WindowResizeEvent& event) {
		createFrameResources();
		updateDebugInputAttachmentDescriptorSets();
	}

	void VisibilityRenderer::createFrameResources() {
		visibilityImage = createVisibilityImage();
		outputImage = createOutputImage();
		depthStencilImage = createDepthStencilImage();

		visibilityImageView = createVisibilityImageView();
		outputImageView = createOutputImageView();
		depthStencilImageView = createDepthStencilImageView();

		materialCoordinateBuffer = createMaterialCoordinateBuffer();

		visibilityFramebuffer = createVisibilityFramebuffer();
		createPresentFramebuffers();
	}

	void VisibilityRenderer::updateCameraPosition() const {
		cameraMappedRange->position = getCameraController<PerspectiveCamera>().getCamera().getPosition();
	}

	void VisibilityRenderer::updateCameraProjectionView() const {
		cameraMappedRange->projectionView = getCameraController<PerspectiveCamera>().getCamera().getProjectionView();
	}

	void VisibilityRenderer::updateViewport() const {
		cameraMappedRange->viewport = getSwapchain()->getExtent();
	}

	void VisibilityRenderer::updateInstanceTransform(Entity entity, const mat4x4<float32>& transform) {
		instanceMappedRange[entity].transform = transform;
	}

	void VisibilityRenderer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	void VisibilityRenderer::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({
			cameraDescriptorSetLayout,
			clusterDescriptorSetLayout,
			instanceDescriptorSetLayout,
			materialCountDescriptorSetLayout,
			materialOffsetDescriptorSetLayout,
			materialCoordinateDescriptorSetLayout,
			materialBaseDescriptorSetLayout,
			outputDescriptorSetLayout,
			debugDescriptorSetLayout,
		});

		cameraDescriptorSet = descriptorSets[0];
		clusterDescriptorSet = descriptorSets[1];
		instanceDescriptorSet = descriptorSets[2];
		materialCountDescriptorSet = descriptorSets[3];
		materialOffsetDescriptorSet = descriptorSets[4];
		materialCoordinateDescriptorSet = descriptorSets[5];
		materialBaseDescriptorSet = descriptorSets[6];
		outputDescriptorSet = descriptorSets[7];
		debugDescriptorSet = descriptorSets[8];
	}

	void VisibilityRenderer::fillSceneBuffers() const {
		const ref<Buffer<uint8>> stagingBuffer = createStagingBuffer<uint8>(dataBuffer->getSize() + instanceBuffer->getSize());

		uint8* mappedRange = stagingBuffer->map(0, stagingBuffer->getSize());

		std::copy(scene->vertexPositionComponents.begin(), scene->vertexPositionComponents.end(), pointer_cast<float32*>(mappedRange));
		mappedRange += vertexNormalComponentRegionSize;

		std::copy(scene->vertexNormalComponents.begin(), scene->vertexNormalComponents.end(), pointer_cast<float32*>(mappedRange));
		mappedRange += vertexNormalComponentRegionSize;

		std::copy(scene->vertexTextureCoordinateComponents.begin(), scene->vertexTextureCoordinateComponents.end(), pointer_cast<float32*>(mappedRange));
		mappedRange += vertexTextureCoordinateComponentRegionSize;

		std::copy(scene->vertexPositionIndices.begin(), scene->vertexPositionIndices.end(), pointer_cast<uint32*>(mappedRange));
		mappedRange += vertexPositionIndexRegionSize;

		std::copy(scene->vertexNormalIndices.begin(), scene->vertexNormalIndices.end(), pointer_cast<uint32*>(mappedRange));
		mappedRange += vertexNormalIndexRegionSize;

		std::copy(scene->vertexTextureCoordinateIndices.begin(), scene->vertexTextureCoordinateIndices.end(), pointer_cast<uint32*>(mappedRange));
		mappedRange += vertexTextureCoordinateIndexRegionSize;

		std::copy(scene->clusters.begin(), scene->clusters.end(), pointer_cast<Cluster*>(mappedRange));
		mappedRange += clusterRegionSize;

		{
			Geometry* geometryMappedRange = pointer_cast<Geometry*>(mappedRange);
			uint32 geometryIndex = 0;

			while (geometryIndex < scene->geometries.count()) {
				geometryMappedRange[geometryIndex].vertexOffset = scene->geometries[geometryIndex].vertexOffset;
				geometryMappedRange[geometryIndex].clusterCount = scene->geometries[geometryIndex].clusterCount;

				geometryIndex += 1;
			}

			mappedRange += geometryRegionSize;
		}

		std::copy(scene->instances.begin(), scene->instances.end(), pointer_cast<ClusteredInstance*>(mappedRange));

		stagingBuffer->unmap();

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Copy buffers",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(
			stagingBuffer,
			dataBuffer,
			{
				{
					.sourceOffset = 0,
					.destinationOffset = 0,
					.size = dataBuffer->getSize(),
				},
			}
		);
		commandEncoder->copyBufferToBuffer(
			stagingBuffer,
			instanceBuffer,
			{
				{
					.sourceOffset = dataBuffer->getSize(),
					.destinationOffset = 0,
					.size = instanceBuffer->getSize(),
				},
			}
		);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer});
		transferQueue->waitIdle();
	}

	void VisibilityRenderer::writeVisibilityIndirectCommand() const {
		DrawIndirectCommand* visibilityIndirectMappedRange = visibilityIndirectBuffer->map(0, visibilityIndirectBuffer->getSize());

		DrawIndirectCommand& drawIndirectCommand = visibilityIndirectMappedRange[0];
		drawIndirectCommand.vertexCount = CLUSTER_INDEX_COUNT;
		drawIndirectCommand.instanceCount = static_cast<uint32>(scene->clusters.count());
		drawIndirectCommand.vertexOffset = 0;
		drawIndirectCommand.instanceOffset = 0;
	}

	void VisibilityRenderer::updateBufferDescriptorSets() {
		const DescriptorBuffer cameraDescriptorBuffer = {
			.buffer = cameraBuffer,
			.offset = 0,
			.size = cameraBuffer->getSize(),
		};

		const DescriptorBuffer vertexPositionComponentDescriptorBuffer = {
			.buffer = dataBuffer,
			.offset = 0,
			.size = vertexPositionComponentRegionSize,
		};

		const DescriptorBuffer vertexNormalComponentDescriptorBuffer = {
			.buffer = dataBuffer,
			.offset = vertexPositionComponentRegionSize,
			.size = vertexNormalComponentRegionSize,
		};

		const DescriptorBuffer vertexTextureCoordinateComponentDescriptorBuffer = {
			.buffer = dataBuffer,
			.offset = vertexPositionComponentRegionSize + vertexNormalComponentRegionSize,
			.size = vertexTextureCoordinateComponentRegionSize,
		};

		const DescriptorBuffer vertexPositionIndexDescriptorBuffer = {
			.buffer = dataBuffer,
			.offset = vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize,
			.size = vertexPositionIndexRegionSize,
		};

		const DescriptorBuffer vertexNormalIndexDescriptorBuffer = {
			.buffer = dataBuffer,
			.offset = vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize + vertexPositionIndexRegionSize,
			.size = vertexNormalIndexRegionSize,
		};

		const DescriptorBuffer vertexTextureCoordinateIndexDescriptorBuffer = {
			.buffer = dataBuffer,
			.offset = vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize + vertexPositionIndexRegionSize + vertexNormalIndexRegionSize,
			.size = vertexTextureCoordinateIndexRegionSize,
		};

		const DescriptorBuffer clusterDescriptorBuffer = {
			.buffer = dataBuffer,
			.offset = vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize + vertexPositionIndexRegionSize + vertexNormalIndexRegionSize + vertexTextureCoordinateIndexRegionSize,
			.size = clusterRegionSize,
		};

		const DescriptorBuffer geometryDescriptorBuffer = {
			.buffer = dataBuffer,
			.offset = vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize + vertexPositionIndexRegionSize + vertexNormalIndexRegionSize + vertexTextureCoordinateIndexRegionSize + clusterRegionSize,
			.size = geometryRegionSize,
		};

		const DescriptorBuffer instanceDescriptorBuffer = {
			.buffer = instanceBuffer,
			.offset = 0,
			.size = scene->instances.count() * sizeof(ClusteredInstance),
		};

		const DescriptorBuffer materialCountDescriptorBuffer = {
			.buffer = materialCountBuffer,
			.offset = 0,
			.size = materialCountBuffer->getSize(),
		};

		const DescriptorBuffer materialOffsetDescriptorBuffer = {
			.buffer = materialOffsetBuffer,
			.offset = 0,
			.size = materialOffsetBuffer->getSize(),
		};

		const DescriptorBuffer materialIndirectDescriptorBuffer = {
			.buffer = materialIndirectBuffer,
			.offset = 0,
			.size = materialIndirectBuffer->getSize(),
		};

		WriteDescriptorSet cameraWriteDescriptorSet{};
		cameraWriteDescriptorSet.descriptorSet = cameraDescriptorSet;
		cameraWriteDescriptorSet.bindingIndex = 0;
		cameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet vertexPositionComponentWriteDescriptorSet{};
		vertexPositionComponentWriteDescriptorSet.descriptorSet = clusterDescriptorSet;
		vertexPositionComponentWriteDescriptorSet.bindingIndex = 0;
		vertexPositionComponentWriteDescriptorSet.descriptorBuffers = {vertexPositionComponentDescriptorBuffer};

		WriteDescriptorSet vertexPositionIndexWriteDescriptorSet{};
		vertexPositionIndexWriteDescriptorSet.descriptorSet = clusterDescriptorSet;
		vertexPositionIndexWriteDescriptorSet.bindingIndex = 1;
		vertexPositionIndexWriteDescriptorSet.descriptorBuffers = {vertexPositionIndexDescriptorBuffer};

		WriteDescriptorSet clusterWriteDescriptorSet{};
		clusterWriteDescriptorSet.descriptorSet = clusterDescriptorSet;
		clusterWriteDescriptorSet.bindingIndex = 2;
		clusterWriteDescriptorSet.descriptorBuffers = {clusterDescriptorBuffer};

		WriteDescriptorSet instanceWriteDescriptorSet{};
		instanceWriteDescriptorSet.descriptorSet = instanceDescriptorSet;
		instanceWriteDescriptorSet.bindingIndex = 0;
		instanceWriteDescriptorSet.descriptorBuffers = {instanceDescriptorBuffer};

		WriteDescriptorSet geometryWriteDescriptorSet{};
		geometryWriteDescriptorSet.descriptorSet = instanceDescriptorSet;
		geometryWriteDescriptorSet.bindingIndex = 1;
		geometryWriteDescriptorSet.descriptorBuffers = {geometryDescriptorBuffer};

		WriteDescriptorSet materialCountCameraWriteDescriptorSet{};
		materialCountCameraWriteDescriptorSet.descriptorSet = materialCountDescriptorSet;
		materialCountCameraWriteDescriptorSet.bindingIndex = 0;
		materialCountCameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet materialCountClusterWriteDescriptorSet{};
		materialCountClusterWriteDescriptorSet.descriptorSet = materialCountDescriptorSet;
		materialCountClusterWriteDescriptorSet.bindingIndex = 2;
		materialCountClusterWriteDescriptorSet.descriptorBuffers = {clusterDescriptorBuffer};

		WriteDescriptorSet materialCountInstanceWriteDescriptorSet{};
		materialCountInstanceWriteDescriptorSet.descriptorSet = materialCountDescriptorSet;
		materialCountInstanceWriteDescriptorSet.bindingIndex = 3;
		materialCountInstanceWriteDescriptorSet.descriptorBuffers = {instanceDescriptorBuffer};

		WriteDescriptorSet materialCountCountWriteDescriptorSet{};
		materialCountCountWriteDescriptorSet.descriptorSet = materialCountDescriptorSet;
		materialCountCountWriteDescriptorSet.bindingIndex = 4;
		materialCountCountWriteDescriptorSet.descriptorBuffers = {materialCountDescriptorBuffer};

		WriteDescriptorSet debugCameraWriteDescriptorSet{};
		debugCameraWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugCameraWriteDescriptorSet.bindingIndex = 0;
		debugCameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet debugVertexPositionComponentWriteDescriptorSet{};
		debugVertexPositionComponentWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexPositionComponentWriteDescriptorSet.bindingIndex = 3;
		debugVertexPositionComponentWriteDescriptorSet.descriptorBuffers = {vertexPositionComponentDescriptorBuffer};

		WriteDescriptorSet debugVertexNormalComponentWriteDescriptorSet{};
		debugVertexNormalComponentWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexNormalComponentWriteDescriptorSet.bindingIndex = 4;
		debugVertexNormalComponentWriteDescriptorSet.descriptorBuffers = {vertexNormalComponentDescriptorBuffer};

		WriteDescriptorSet debugVertexTextureCoordinateComponentWriteDescriptorSet{};
		debugVertexTextureCoordinateComponentWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexTextureCoordinateComponentWriteDescriptorSet.bindingIndex = 5;
		debugVertexTextureCoordinateComponentWriteDescriptorSet.descriptorBuffers = {vertexTextureCoordinateComponentDescriptorBuffer};

		WriteDescriptorSet debugVertexPositionIndexWriteDescriptorSet{};
		debugVertexPositionIndexWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexPositionIndexWriteDescriptorSet.bindingIndex = 6;
		debugVertexPositionIndexWriteDescriptorSet.descriptorBuffers = {vertexPositionIndexDescriptorBuffer};

		WriteDescriptorSet debugVertexNormalIndexWriteDescriptorSet{};
		debugVertexNormalIndexWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexNormalIndexWriteDescriptorSet.bindingIndex = 7;
		debugVertexNormalIndexWriteDescriptorSet.descriptorBuffers = {vertexNormalIndexDescriptorBuffer};

		WriteDescriptorSet debugVertexTextureCoordinateIndexWriteDescriptorSet{};
		debugVertexTextureCoordinateIndexWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexTextureCoordinateIndexWriteDescriptorSet.bindingIndex = 8;
		debugVertexTextureCoordinateIndexWriteDescriptorSet.descriptorBuffers = {vertexTextureCoordinateIndexDescriptorBuffer};

		WriteDescriptorSet materialOffsetCountWriteDescriptorSet{};
		materialOffsetCountWriteDescriptorSet.descriptorSet = materialOffsetDescriptorSet;
		materialOffsetCountWriteDescriptorSet.bindingIndex = 0;
		materialOffsetCountWriteDescriptorSet.descriptorBuffers = {materialCountDescriptorBuffer};

		WriteDescriptorSet materialOffsetWriteDescriptorSet{};
		materialOffsetWriteDescriptorSet.descriptorSet = materialOffsetDescriptorSet;
		materialOffsetWriteDescriptorSet.bindingIndex = 1;
		materialOffsetWriteDescriptorSet.descriptorBuffers = {materialOffsetDescriptorBuffer};

		WriteDescriptorSet materialIndirectWriteDescriptorSet{};
		materialIndirectWriteDescriptorSet.descriptorSet = materialOffsetDescriptorSet;
		materialIndirectWriteDescriptorSet.bindingIndex = 2;
		materialIndirectWriteDescriptorSet.descriptorBuffers = {materialIndirectDescriptorBuffer};

		// Material coordinate descriptor set

		WriteDescriptorSet materialCoordinateCameraWriteDescriptorSet{};
		materialCoordinateCameraWriteDescriptorSet.descriptorSet = materialCoordinateDescriptorSet;
		materialCoordinateCameraWriteDescriptorSet.bindingIndex = 0;
		materialCoordinateCameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet materialCoordinateClusterWriteDescriptorSet{};
		materialCoordinateClusterWriteDescriptorSet.descriptorSet = materialCoordinateDescriptorSet;
		materialCoordinateClusterWriteDescriptorSet.bindingIndex = 2;
		materialCoordinateClusterWriteDescriptorSet.descriptorBuffers = {clusterDescriptorBuffer};

		WriteDescriptorSet materialCoordinateInstanceWriteDescriptorSet{};
		materialCoordinateInstanceWriteDescriptorSet.descriptorSet = materialCoordinateDescriptorSet;
		materialCoordinateInstanceWriteDescriptorSet.bindingIndex = 3;
		materialCoordinateInstanceWriteDescriptorSet.descriptorBuffers = {instanceDescriptorBuffer};

		WriteDescriptorSet materialCoordinateMaterialOffsetWriteDescriptorSet{};
		materialCoordinateMaterialOffsetWriteDescriptorSet.descriptorSet = materialCoordinateDescriptorSet;
		materialCoordinateMaterialOffsetWriteDescriptorSet.bindingIndex = 4;
		materialCoordinateMaterialOffsetWriteDescriptorSet.descriptorBuffers = {materialOffsetDescriptorBuffer};

		// Material base descriptor set

		WriteDescriptorSet materialBaseCameraWriteDescriptorSet{};
		materialBaseCameraWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseCameraWriteDescriptorSet.bindingIndex = 0;
		materialBaseCameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet materialBaseOffsetWriteDescriptorSet{};
		materialBaseOffsetWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseOffsetWriteDescriptorSet.bindingIndex = 1;
		materialBaseOffsetWriteDescriptorSet.descriptorBuffers = {materialOffsetDescriptorBuffer};

		WriteDescriptorSet materialBaseClusterWriteDescriptorSet{};
		materialBaseClusterWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseClusterWriteDescriptorSet.bindingIndex = 5;
		materialBaseClusterWriteDescriptorSet.descriptorBuffers = {clusterDescriptorBuffer};

		WriteDescriptorSet materialBaseInstanceWriteDescriptorSet{};
		materialBaseInstanceWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseInstanceWriteDescriptorSet.bindingIndex = 6;
		materialBaseInstanceWriteDescriptorSet.descriptorBuffers = {instanceDescriptorBuffer};

		getDevice()->updateDescriptorSets({
			cameraWriteDescriptorSet,
			vertexPositionComponentWriteDescriptorSet,
			vertexPositionIndexWriteDescriptorSet,

			clusterWriteDescriptorSet,

			instanceWriteDescriptorSet,
			geometryWriteDescriptorSet,

			materialCountCameraWriteDescriptorSet,
			materialCountClusterWriteDescriptorSet,
			materialCountInstanceWriteDescriptorSet,
			materialCountCountWriteDescriptorSet,

			debugCameraWriteDescriptorSet,
			debugVertexPositionComponentWriteDescriptorSet,
			debugVertexNormalComponentWriteDescriptorSet,
			debugVertexTextureCoordinateComponentWriteDescriptorSet,
			debugVertexPositionIndexWriteDescriptorSet,
			debugVertexNormalIndexWriteDescriptorSet,
			debugVertexTextureCoordinateIndexWriteDescriptorSet,

			materialOffsetCountWriteDescriptorSet,
			materialOffsetWriteDescriptorSet,
			materialIndirectWriteDescriptorSet,

			materialCoordinateCameraWriteDescriptorSet,
			materialCoordinateClusterWriteDescriptorSet,
			materialCoordinateInstanceWriteDescriptorSet,
			materialCoordinateMaterialOffsetWriteDescriptorSet,

			materialBaseCameraWriteDescriptorSet,
			materialBaseOffsetWriteDescriptorSet,
			materialBaseClusterWriteDescriptorSet,
			materialBaseInstanceWriteDescriptorSet,
		});
	}

	void VisibilityRenderer::updateDebugInputAttachmentDescriptorSets() {
		const DescriptorImage outputDescriptorImage = {
			.imageView = outputImageView,
			.imageLayout = ImageLayout::GENERAL,
			.sampler = nullptr,
		};

		const DescriptorImage visibilityDescriptorImage = {
			.imageView = visibilityImageView,
			.imageLayout = ImageLayout::GENERAL,
			.sampler = nullptr,
		};

		/* const DescriptorImage depthStencilDescriptorImage = {
			.imageView = depthStencilImageView,
			.imageLayout = ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			.sampler = nullptr,
		}; */

		const DescriptorBuffer materialCoordinateDescriptorBuffer = {
			.buffer = materialCoordinateBuffer,
			.offset = 0,
			.size = materialCoordinateBuffer->getSize(),
		};

		WriteDescriptorSet materialCountVisibilityWriteDescriptorSet{};
		materialCountVisibilityWriteDescriptorSet.descriptorSet = materialCountDescriptorSet;
		materialCountVisibilityWriteDescriptorSet.bindingIndex = 1;
		materialCountVisibilityWriteDescriptorSet.descriptorImages = {visibilityDescriptorImage};

		WriteDescriptorSet materialCoordinateVisibilityWriteDescriptorSet{};
		materialCoordinateVisibilityWriteDescriptorSet.descriptorSet = materialCoordinateDescriptorSet;
		materialCoordinateVisibilityWriteDescriptorSet.bindingIndex = 1;
		materialCoordinateVisibilityWriteDescriptorSet.descriptorImages = {visibilityDescriptorImage};

		WriteDescriptorSet materialCoordinateCoordinateWriteDescriptorSet{};
		materialCoordinateCoordinateWriteDescriptorSet.descriptorSet = materialCoordinateDescriptorSet;
		materialCoordinateCoordinateWriteDescriptorSet.bindingIndex = 5;
		materialCoordinateCoordinateWriteDescriptorSet.descriptorBuffers = {materialCoordinateDescriptorBuffer};

		WriteDescriptorSet materialBaseCoordinateWriteDescriptorSet{};
		materialBaseCoordinateWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseCoordinateWriteDescriptorSet.bindingIndex = 2;
		materialBaseCoordinateWriteDescriptorSet.descriptorBuffers = {materialCoordinateDescriptorBuffer};

		WriteDescriptorSet materialBaseOutputWriteDescriptorSet{};
		materialBaseOutputWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseOutputWriteDescriptorSet.bindingIndex = 3;
		materialBaseOutputWriteDescriptorSet.descriptorImages = {outputDescriptorImage};

		WriteDescriptorSet materialBaseVisibilityWriteDescriptorSet{};
		materialBaseVisibilityWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseVisibilityWriteDescriptorSet.bindingIndex = 4;
		materialBaseVisibilityWriteDescriptorSet.descriptorImages = {visibilityDescriptorImage};

		WriteDescriptorSet outputWriteDescriptorSet{};
		outputWriteDescriptorSet.descriptorSet = outputDescriptorSet;
		outputWriteDescriptorSet.bindingIndex = 0;
		outputWriteDescriptorSet.descriptorImages = {outputDescriptorImage};

		WriteDescriptorSet debugVisibilityWriteDescriptorSet{};
		debugVisibilityWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVisibilityWriteDescriptorSet.bindingIndex = 1;
		debugVisibilityWriteDescriptorSet.descriptorImages = {visibilityDescriptorImage};

		/* WriteDescriptorSet debugDepthStencilWriteDescriptorSet{};
		debugDepthStencilWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugDepthStencilWriteDescriptorSet.bindingIndex = 2;
		debugDepthStencilWriteDescriptorSet.descriptorImages = {depthStencilDescriptorImage}; */

		getDevice()->updateDescriptorSets({
			materialCountVisibilityWriteDescriptorSet,

			materialCoordinateVisibilityWriteDescriptorSet,
			materialCoordinateCoordinateWriteDescriptorSet,

			materialBaseCoordinateWriteDescriptorSet,
			materialBaseOutputWriteDescriptorSet,
			materialBaseVisibilityWriteDescriptorSet,

			outputWriteDescriptorSet,

			debugVisibilityWriteDescriptorSet,
			// debugDepthStencilWriteDescriptorSet,
		});
	}

	bool VisibilityRenderer::onWindowResize(WindowResizeEvent& event) {
		resize(event);

		return true;
	}
}