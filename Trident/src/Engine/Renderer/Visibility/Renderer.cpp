// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/Visibility/CameraBuffer.hpp"
#include "Engine/Renderer/Visibility/CameraDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/ClusterDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/DataBuffer.hpp"
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
#include "Engine/Renderer/Visibility/MaterialObjects.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetBuffer.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetPipeline.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialShaderImageSampler.hpp"
#include "Engine/Renderer/Visibility/MaterialShaderIndirectBuffer.hpp"
#include "Engine/Renderer/Visibility/OutputDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/OutputImage.hpp"
#include "Engine/Renderer/Visibility/OutputPipeline.hpp"
#include "Engine/Renderer/Visibility/OutputPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/PresentFramebuffers.hpp"
#include "Engine/Renderer/Visibility/PresentRenderPass.hpp"
#include "Engine/Renderer/Visibility/Renderer.hpp"
#include "Engine/Renderer/Visibility/VisibilityDebugDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/VisibilityDebugPipeline.hpp"
#include "Engine/Renderer/Visibility/VisibilityDebugPipelineLayout.hpp"
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

		#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
			visibilityDebugDescriptorSetLayout = createVisibilityDebugDescriptorSetLayout();
		#endif

		materialCountDescriptorSetLayout = createMaterialCountDescriptorSetLayout();
		materialOffsetDescriptorSetLayout = createMaterialOffsetDescriptorSetLayout();
		materialCoordinateDescriptorSetLayout = createMaterialCoordinateDescriptorSetLayout();
		materialBaseDescriptorSetLayout = createMaterialBaseDescriptorSetLayout();
		outputDescriptorSetLayout = createOutputDescriptorSetLayout();

		allocateDescriptorSets();

		visibilityPipelineLayout = createVisibilityPipelineLayout();

		#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
			visibilityDebugPipelineLayout = createVisibilityDebugPipelineLayout();
		#endif

		materialCountPipelineLayout = createMaterialCountPipelineLayout();
		materialOffsetPipelineLayout = createMaterialOffsetPipelineLayout();
		materialCoordinatePipelineLayout = createMaterialCoordinatePipelineLayout();
		outputPipelineLayout = createOutputPipelineLayout();

		visibilityPipeline = createVisibilityPipeline();

		#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
			visibilityDebugPipeline = createVisibilityDebugPipeline();
		#endif

		materialCountPipeline = createMaterialCountPipeline();
		materialOffsetPipeline = createMaterialOffsetPipeline();
		materialCoordinatePipeline = createMaterialCoordinatePipeline();
		outputPipeline = createOutputPipeline();

		visibilityIndirectBuffer = createVisibilityIndirectBuffer();

		cameraBuffer = createCameraBuffer();
		cameraMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());

		materialShaderImageSampler = createMaterialShaderImageSampler();

		updateViewport();
	}

	void VisibilityRenderer::postSetScene() {
		const uint64 minStorageBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
		const uint64 vertexPositionComponentRegionSize = ceil(scene->vertexPositionComponents.count() * sizeof(float32), minStorageBufferOffsetAlignment);
		const uint64 vertexNormalComponentRegionSize = ceil(scene->vertexNormalComponents.count() * sizeof(float32), minStorageBufferOffsetAlignment);
		const uint64 vertexTextureCoordinateComponentRegionSize = ceil(scene->vertexTextureCoordinateComponents.count() * sizeof(float32), minStorageBufferOffsetAlignment);
		const uint64 vertexPositionIndexRegionSize = ceil(scene->vertexPositionIndices.count() * sizeof(uint32), minStorageBufferOffsetAlignment);
		const uint64 vertexNormalIndexRegionSize = ceil(scene->vertexNormalIndices.count() * sizeof(uint32), minStorageBufferOffsetAlignment);
		const uint64 vertexTextureCoordinateIndexRegionSize = ceil(scene->vertexTextureCoordinateIndices.count() * sizeof(uint32), minStorageBufferOffsetAlignment);
		const uint64 clusterRegionSize = ceil(scene->clusters.count() * sizeof(Cluster), minStorageBufferOffsetAlignment);
		const uint64 geometryRegionSize = ceil(scene->geometries.count() * sizeof(Geometry), minStorageBufferOffsetAlignment);

		dataBuffer = createDataBuffer(vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize + vertexPositionIndexRegionSize + vertexNormalIndexRegionSize + vertexTextureCoordinateIndexRegionSize + clusterRegionSize + geometryRegionSize);
		instanceBuffer = createInstanceBuffer();
		materialCountBuffer = createMaterialCountBuffer();
		materialOffsetBuffer = createMaterialOffsetBuffer();
		materialShaderIndirectBuffer = createMaterialShaderIndirectBuffer();

		instanceMappedRange = instanceBuffer->map(0, instanceBuffer->getSize());

		writeVisibilityIndirectCommand();

		visibilityIndirectBuffer->unmap();

		const ref<Buffer<uint8>> stagingBuffer = createStagingBuffer<uint8>(dataBuffer->getSize() + instanceBuffer->getSize());

		uint8* mappedRange = stagingBuffer->map(0, stagingBuffer->getSize());

		std::copy(scene->vertexPositionComponents.begin(), scene->vertexPositionComponents.end(), pointer_cast<float32*>(mappedRange));
		mappedRange += vertexPositionComponentRegionSize;

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

		Geometry* geometryMappedRange = pointer_cast<Geometry*>(mappedRange);
		uint32 geometryIndex = 0;

		while (geometryIndex < scene->geometries.count()) {
			geometryMappedRange[geometryIndex].vertexOffset = scene->geometries[geometryIndex].vertexOffset;
			geometryMappedRange[geometryIndex].vertexTextureCoordinateOffset = scene->geometries[geometryIndex].vertexTextureCoordinateOffset;
			geometryMappedRange[geometryIndex].clusterCount = scene->geometries[geometryIndex].clusterCount;

			geometryIndex += 1;
		}

		mappedRange += geometryRegionSize;

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

		transitionOutputImage();
		updateBufferDescriptorSets(vertexPositionComponentRegionSize, vertexNormalComponentRegionSize, vertexTextureCoordinateComponentRegionSize, vertexPositionIndexRegionSize, vertexNormalIndexRegionSize, vertexTextureCoordinateIndexRegionSize, clusterRegionSize, geometryRegionSize);
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

		BufferMemoryBarrier bufferMemoryBarrier;

		// Visibility render pass
		{
			commandEncoder->beginRenderPass({
				.framebuffer = visibilityFramebuffer,
				.clearValues = {
					{0.0f, 0.0f, 0.0f, 1.0f},
					{
						.depth = 1.0f,
						.stencil = 0,
					},
				},
			});
			commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
			commandEncoder->setScissor(0, 0, extent);
			commandEncoder->bindGraphicsPipeline(visibilityPipeline);
			commandEncoder->bindDescriptorSets(visibilityPipelineLayout, 0, {cameraDescriptorSet, clusterDescriptorSet, instanceDescriptorSet});
			commandEncoder->drawIndirect(visibilityIndirectBuffer, 0, 1, 0);
			commandEncoder->endRenderPass();
		}

		// TODO(render-pass/sync-image-layout): The layout of attachment images must be automatically updated when calling CommandEncoder->beginRenderPass.
		visibilityImage->layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
		depthStencilImage->layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		const ImageMemoryBarrier visibilityImageMemoryBarrier = {
			.image = visibilityImage,
			.newLayout = ImageLayout::GENERAL,
			.newAccess = Access::SHADER_READ,
		};

		#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
			// Visibility debug pass
			{
				const ImageMemoryBarrier depthStencilImageMemoryBarrier = {
					.image = depthStencilImage,
					.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({}, {visibilityImageMemoryBarrier, depthStencilImageMemoryBarrier});

				const uint32 groupCountX = static_cast<uint32>(ceil(width / 16.0f));
				const uint32 groupCountY = static_cast<uint32>(ceil(height / 16.0f));

				commandEncoder->bindComputePipeline(visibilityDebugPipeline);
				commandEncoder->bindDescriptorSets(visibilityDebugPipelineLayout, 0, {visibilityDebugDescriptorSet, clusterDescriptorSet, instanceDescriptorSet});
				commandEncoder->dispatch(groupCountX, groupCountY, 1);
			}
		#else
			commandEncoder->insertMemoryBarriers({}, {visibilityImageMemoryBarrier});

			// Material compute pass
			{
				const uint32 groupCountX = static_cast<uint32>(ceil(width / 16.0f));
				const uint32 groupCountY = static_cast<uint32>(ceil(height / 16.0f));

				// Count the number of pixels used by each material.
				commandEncoder->bindComputePipeline(materialCountPipeline);
				commandEncoder->bindDescriptorSets(materialCountPipelineLayout, 0, {materialCountDescriptorSet});
				commandEncoder->dispatch(groupCountX, groupCountY, 1);

				bufferMemoryBarrier = {
					.buffer = materialCountBuffer,
					.offset = 0,
					.size = materialCountBuffer->getSize(),
					.newAccess = Access::SHADER_READ | Access::SHADER_WRITE,
				};

				commandEncoder->insertMemoryBarriers({bufferMemoryBarrier}, {});

				// Get the material pixel offsets with a prefix sum and write indirect arguments for each material pipeline.
				commandEncoder->bindComputePipeline(materialOffsetPipeline);
				commandEncoder->bindDescriptorSets(materialOffsetPipelineLayout, 0, {materialOffsetDescriptorSet});
				commandEncoder->dispatch(1, 1, 1);

				// Re-order the pixels per material index.
				commandEncoder->bindComputePipeline(materialCoordinatePipeline);
				commandEncoder->bindDescriptorSets(materialCoordinatePipelineLayout, 0, {materialCoordinateDescriptorSet});
				commandEncoder->dispatch(groupCountX, groupCountY, 1);

				bufferMemoryBarrier = {
					.buffer = materialOffsetBuffer,
					.offset = 0,
					.size = materialOffsetBuffer->getSize(),
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({bufferMemoryBarrier}, {});

				uint32 materialShaderIndex = 0;

				while (materialShaderIndex < scene->materialShaders.size()) {
					commandEncoder->bindComputePipeline(materialShaderPipelines[materialShaderIndex]);
					commandEncoder->bindDescriptorSets(materialShaderPipelineLayouts[materialShaderIndex], 0, {materialBaseDescriptorSet, materialShaderUserDescriptorSets[materialShaderIndex]});
					commandEncoder->dispatchIndirect(materialShaderIndirectBuffer, materialShaderIndex * sizeof(DispatchIndirectCommand));

					materialShaderIndex += 1;
				}
			}
		#endif

		// Present render pass
		{
			commandEncoder->beginRenderPass({
				.framebuffer = presentFramebuffers[getSwapchainImageIndex()],
				.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}},
			});
			commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
			commandEncoder->setScissor(0, 0, extent);
			commandEncoder->bindGraphicsPipeline(outputPipeline);
			commandEncoder->bindDescriptorSets(outputPipelineLayout, 0, {outputDescriptorSet});
			commandEncoder->draw(3, 1, 0, 0);
			commandEncoder->endRenderPass();
		}
	}

	void VisibilityRenderer::resize(WindowResizeEvent& event) {
		createFrameResources();
		transitionOutputImage();
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

	void VisibilityRenderer::updateInstanceWorld(Entity entity, const mat4x4<float32>& world) {
		instanceMappedRange[entity].world = world;
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
			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				visibilityDebugDescriptorSetLayout,
			#endif
		});

		cameraDescriptorSet = descriptorSets[0];
		clusterDescriptorSet = descriptorSets[1];
		instanceDescriptorSet = descriptorSets[2];
		materialCountDescriptorSet = descriptorSets[3];
		materialOffsetDescriptorSet = descriptorSets[4];
		materialCoordinateDescriptorSet = descriptorSets[5];
		materialBaseDescriptorSet = descriptorSets[6];
		outputDescriptorSet = descriptorSets[7];
		#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
			visibilityDebugDescriptorSet = descriptorSets[8];
		#endif
	}

	void VisibilityRenderer::transitionOutputImage() const {
		if (getGraphicsApi() != GraphicsAPI::DX12) {
			// TODO: Avoid creating a command encoder for a single image transition?
			const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
			const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder({
				.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
				#if defined(BUILD_MODE_DEBUG)
					.name = "Transition output image",
				#endif
			});

			commandEncoder->begin();

			const ImageMemoryBarrier outputImageMemoryBarrier = {
				.image = outputImage,
				.newLayout = ImageLayout::GENERAL,
				.newAccess = Access::SHADER_WRITE,
			};

			commandEncoder->insertMemoryBarriers({}, {outputImageMemoryBarrier});

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			graphicsQueue->submit({commandBuffer});
			graphicsQueue->waitIdle();
		}
	}

	void VisibilityRenderer::writeVisibilityIndirectCommand() const {
		DrawIndirectCommand* visibilityIndirectMappedRange = visibilityIndirectBuffer->map(0, visibilityIndirectBuffer->getSize());

		DrawIndirectCommand& drawIndirectCommand = visibilityIndirectMappedRange[0];
		drawIndirectCommand.vertexCount = CLUSTER_INDEX_COUNT;
		drawIndirectCommand.instanceCount = static_cast<uint32>(scene->clusters.count());
		drawIndirectCommand.vertexOffset = 0;
		drawIndirectCommand.instanceOffset = 0;
	}

	void VisibilityRenderer::updateBufferDescriptorSets(uint64 vertexPositionComponentRegionSize, uint64 vertexNormalComponentRegionSize, uint64 vertexTextureCoordinateComponentRegionSize, uint64 vertexPositionIndexRegionSize, uint64 vertexNormalIndexRegionSize, uint64 vertexTextureCoordinateIndexRegionSize, uint64 clusterRegionSize, uint64 geometryRegionSize) {
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
			.offset = sizeof(uint32),
			.size = materialOffsetBuffer->getSize() - sizeof(uint32),
		};
		const DescriptorBuffer materialShaderIndirectDescriptorBuffer = {
			.buffer = materialShaderIndirectBuffer,
			.offset = 0,
			.size = materialShaderIndirectBuffer->getSize(),
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

		#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
			const WriteDescriptorSet visibilityDebugCameraWriteDescriptorSet = {
				.descriptorSet = visibilityDebugDescriptorSet,
				.bindingIndex = 0,
				.descriptorBuffers = {cameraDescriptorBuffer},
			};
			const WriteDescriptorSet visibilityDebugVertexPositionComponentWriteDescriptorSet = {
				.descriptorSet = visibilityDebugDescriptorSet,
				.bindingIndex = 3,
				.descriptorBuffers = {vertexPositionComponentDescriptorBuffer},
			};
			const WriteDescriptorSet visibilityDebugVertexNormalComponentWriteDescriptorSet = {
				.descriptorSet = visibilityDebugDescriptorSet,
				.bindingIndex = 4,
				.descriptorBuffers = {vertexNormalComponentDescriptorBuffer},
			};
			const WriteDescriptorSet visibilityDebugVertexTextureCoordinateComponentWriteDescriptorSet = {
				.descriptorSet = visibilityDebugDescriptorSet,
				.bindingIndex = 5,
				.descriptorBuffers = {vertexTextureCoordinateComponentDescriptorBuffer},
			};
			const WriteDescriptorSet visibilityDebugVertexPositionIndexWriteDescriptorSet = {
				.descriptorSet = visibilityDebugDescriptorSet,
				.bindingIndex = 6,
				.descriptorBuffers = {vertexPositionIndexDescriptorBuffer},
			};
			const WriteDescriptorSet visibilityDebugVertexNormalIndexWriteDescriptorSet = {
				.descriptorSet = visibilityDebugDescriptorSet,
				.bindingIndex = 7,
				.descriptorBuffers = {vertexNormalIndexDescriptorBuffer},
			};
			const WriteDescriptorSet visibilityDebugVertexTextureCoordinateIndexWriteDescriptorSet = {
				.descriptorSet = visibilityDebugDescriptorSet,
				.bindingIndex = 8,
				.descriptorBuffers = {vertexTextureCoordinateIndexDescriptorBuffer},
			};
		#endif

		WriteDescriptorSet materialOffsetCountWriteDescriptorSet{};
		materialOffsetCountWriteDescriptorSet.descriptorSet = materialOffsetDescriptorSet;
		materialOffsetCountWriteDescriptorSet.bindingIndex = 0;
		materialOffsetCountWriteDescriptorSet.descriptorBuffers = {materialCountDescriptorBuffer};

		WriteDescriptorSet materialOffsetWriteDescriptorSet{};
		materialOffsetWriteDescriptorSet.descriptorSet = materialOffsetDescriptorSet;
		materialOffsetWriteDescriptorSet.bindingIndex = 1;
		materialOffsetWriteDescriptorSet.descriptorBuffers = {materialOffsetDescriptorBuffer};

		WriteDescriptorSet materialShaderIndirectWriteDescriptorSet{};
		materialShaderIndirectWriteDescriptorSet.descriptorSet = materialOffsetDescriptorSet;
		materialShaderIndirectWriteDescriptorSet.bindingIndex = 2;
		materialShaderIndirectWriteDescriptorSet.descriptorBuffers = {materialShaderIndirectDescriptorBuffer};

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

		const WriteDescriptorSet materialBaseCameraWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 0,
			.descriptorBuffers = {cameraDescriptorBuffer},
		};
		const WriteDescriptorSet materialBaseVertexPositionComponentWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 3,
			.descriptorBuffers = {vertexPositionComponentDescriptorBuffer},
		};
		const WriteDescriptorSet materialBaseVertexNormalComponentWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 4,
			.descriptorBuffers = {vertexNormalComponentDescriptorBuffer},
		};
		const WriteDescriptorSet materialBaseVertexTextureCoordinateComponentWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 5,
			.descriptorBuffers = {vertexTextureCoordinateComponentDescriptorBuffer},
		};
		const WriteDescriptorSet materialBaseVertexPositionIndexWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 6,
			.descriptorBuffers = {vertexPositionIndexDescriptorBuffer},
		};
		const WriteDescriptorSet materialBaseVertexNormalIndexWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 7,
			.descriptorBuffers = {vertexNormalIndexDescriptorBuffer},
		};
		const WriteDescriptorSet materialBaseVertexTextureCoordinateIndexWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 8,
			.descriptorBuffers = {vertexTextureCoordinateIndexDescriptorBuffer},
		};
		const WriteDescriptorSet materialBaseClusterWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 9,
			.descriptorBuffers = {clusterDescriptorBuffer},
		};
		const WriteDescriptorSet materialBaseInstanceWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 10,
			.descriptorBuffers = {instanceDescriptorBuffer},
		};
		const WriteDescriptorSet materialBaseGeometryWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 11,
			.descriptorBuffers = {geometryDescriptorBuffer},
		};

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

			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				visibilityDebugCameraWriteDescriptorSet,
				visibilityDebugVertexPositionComponentWriteDescriptorSet,
				visibilityDebugVertexNormalComponentWriteDescriptorSet,
				visibilityDebugVertexTextureCoordinateComponentWriteDescriptorSet,
				visibilityDebugVertexPositionIndexWriteDescriptorSet,
				visibilityDebugVertexNormalIndexWriteDescriptorSet,
				visibilityDebugVertexTextureCoordinateIndexWriteDescriptorSet,
			#endif

			materialOffsetCountWriteDescriptorSet,
			materialOffsetWriteDescriptorSet,
			materialShaderIndirectWriteDescriptorSet,

			materialCoordinateCameraWriteDescriptorSet,
			materialCoordinateClusterWriteDescriptorSet,
			materialCoordinateInstanceWriteDescriptorSet,
			materialCoordinateMaterialOffsetWriteDescriptorSet,

			materialBaseCameraWriteDescriptorSet,
			materialBaseVertexPositionComponentWriteDescriptorSet,
			materialBaseVertexNormalComponentWriteDescriptorSet,
			materialBaseVertexTextureCoordinateComponentWriteDescriptorSet,
			materialBaseVertexPositionIndexWriteDescriptorSet,
			materialBaseVertexNormalIndexWriteDescriptorSet,
			materialBaseVertexTextureCoordinateIndexWriteDescriptorSet,
			materialBaseClusterWriteDescriptorSet,
			materialBaseInstanceWriteDescriptorSet,
			materialBaseGeometryWriteDescriptorSet,
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

		#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
			const DescriptorImage depthStencilDescriptorImage = {
				.imageView = depthStencilImageView,
				.imageLayout = ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL,
				.sampler = materialShaderImageSampler,
			};
		#endif

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

		const WriteDescriptorSet materialBaseMaterialCoordinateWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 1,
			.descriptorBuffers = {materialCoordinateDescriptorBuffer},
		};
		const WriteDescriptorSet materialBaseVisibilityWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 2,
			.descriptorImages = {visibilityDescriptorImage},
		};
		const WriteDescriptorSet materialBaseOutputWriteDescriptorSet = {
			.descriptorSet = materialBaseDescriptorSet,
			.bindingIndex = 12,
			.descriptorImages = {outputDescriptorImage},
		};

		WriteDescriptorSet outputWriteDescriptorSet{};
		outputWriteDescriptorSet.descriptorSet = outputDescriptorSet;
		outputWriteDescriptorSet.bindingIndex = 0;
		outputWriteDescriptorSet.descriptorImages = {outputDescriptorImage};

		#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
			const WriteDescriptorSet visibilityDebugVisibilityWriteDescriptorSet = {
				.descriptorSet = visibilityDebugDescriptorSet,
				.bindingIndex = 1,
				.descriptorImages = {visibilityDescriptorImage},
			};

			const WriteDescriptorSet visibilityDebugDepthStencilWriteDescriptorSet = {
				.descriptorSet = visibilityDebugDescriptorSet,
				.bindingIndex = 2,
				.descriptorImages = {depthStencilDescriptorImage},
			};

			const WriteDescriptorSet visibilityDebugOutputImageWriteDescriptorSet = {
				.descriptorSet = visibilityDebugDescriptorSet,
				.bindingIndex = 9,
				.descriptorImages = {outputDescriptorImage},
			};
		#endif;

		getDevice()->updateDescriptorSets({
			materialCountVisibilityWriteDescriptorSet,

			materialCoordinateVisibilityWriteDescriptorSet,
			materialCoordinateCoordinateWriteDescriptorSet,

			materialBaseMaterialCoordinateWriteDescriptorSet,
			materialBaseVisibilityWriteDescriptorSet,
			materialBaseOutputWriteDescriptorSet,

			outputWriteDescriptorSet,

			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				visibilityDebugVisibilityWriteDescriptorSet,
				visibilityDebugDepthStencilWriteDescriptorSet,
				visibilityDebugOutputImageWriteDescriptorSet,
			#endif
		});
	}

	bool VisibilityRenderer::onWindowResize(WindowResizeEvent& event) {
		resize(event);

		return true;
	}
}