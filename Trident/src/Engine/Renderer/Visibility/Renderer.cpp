// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Camera/3D/PerspectiveCamera3D.hpp"
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

		const BufferView<Camera> cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());

		cameraMappedRange = cameraView.map();

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

		const BufferView<ClusteredInstance> instanceView = instanceBuffer->createView<ClusteredInstance>(0, instanceBuffer->getSize());

		instanceMappedRange = instanceView.map();

		writeVisibilityIndirectCommand();

		const ref<Buffer> stagingBuffer = createDynamicBuffer({
			.size = dataBuffer->getSize() + instanceBuffer->getSize(),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const BufferView<float32> vertexPositionComponentView = stagingBuffer->createView<float32>(0, vertexPositionComponentRegionSize);
		const BufferView<float32> vertexNormalComponentView = stagingBuffer->createView<float32>(vertexPositionComponentView.getOffset() + vertexPositionComponentView.getSize(), vertexNormalComponentRegionSize);
		const BufferView<float32> vertexTextureCoordinateComponentView = stagingBuffer->createView<float32>(vertexNormalComponentView.getOffset() + vertexNormalComponentView.getSize(), vertexTextureCoordinateComponentRegionSize);
		const BufferView<uint32> vertexPositionIndexView = stagingBuffer->createView<uint32>(vertexTextureCoordinateComponentView.getOffset() + vertexTextureCoordinateComponentView.getSize(), vertexPositionIndexRegionSize);
		const BufferView<uint32> vertexNormalIndexView = stagingBuffer->createView<uint32>(vertexPositionIndexView.getOffset() + vertexPositionIndexView.getSize(), vertexNormalIndexRegionSize);
		const BufferView<uint32> vertexTextureCoordinateIndexView = stagingBuffer->createView<uint32>(vertexNormalIndexView.getOffset() + vertexNormalIndexView.getSize(), vertexTextureCoordinateIndexRegionSize);
		const BufferView<Cluster> clusterView = stagingBuffer->createView<Cluster>(vertexTextureCoordinateIndexView.getOffset() + vertexTextureCoordinateIndexView.getSize(), clusterRegionSize);
		const BufferView<Geometry> geometryView = stagingBuffer->createView<Geometry>(clusterView.getOffset() + clusterView.getSize(), geometryRegionSize);
		const BufferView<ClusteredInstance> instanceStagingView = stagingBuffer->createView<ClusteredInstance>(geometryView.getOffset() + geometryView.getSize(), instanceBuffer->getSize());

		std::copy(scene->vertexPositionComponents.begin(), scene->vertexPositionComponents.end(), vertexPositionComponentView.map());
		std::copy(scene->vertexNormalComponents.begin(), scene->vertexNormalComponents.end(), vertexNormalComponentView.map());
		std::copy(scene->vertexTextureCoordinateComponents.begin(), scene->vertexTextureCoordinateComponents.end(), vertexTextureCoordinateComponentView.map());
		std::copy(scene->vertexPositionIndices.begin(), scene->vertexPositionIndices.end(), vertexPositionIndexView.map());
		std::copy(scene->vertexNormalIndices.begin(), scene->vertexNormalIndices.end(), vertexNormalIndexView.map());
		std::copy(scene->vertexTextureCoordinateIndices.begin(), scene->vertexTextureCoordinateIndices.end(), vertexTextureCoordinateIndexView.map());
		std::copy(scene->clusters.begin(), scene->clusters.end(), clusterView.map());

		Geometry* geometryMappedRange = geometryView.map();
		uint32 geometryIndex = 0;

		while (geometryIndex < scene->geometries.count()) {
			geometryMappedRange[geometryIndex].vertexOffset = scene->geometries[geometryIndex].vertexOffset;
			geometryMappedRange[geometryIndex].vertexTextureCoordinateOffset = scene->geometries[geometryIndex].vertexTextureCoordinateOffset;
			geometryMappedRange[geometryIndex].clusterCount = scene->geometries[geometryIndex].clusterCount;

			geometryIndex += 1;
		}

		std::copy(scene->instances.begin(), scene->instances.end(), instanceStagingView.map());

		stagingBuffer->unmap();

		const sequence<BufferCopyRegion> copyRegions = {
			// Data
			{
				.sourceOffset = 0,
				.destinationOffset = 0,
				.size = dataBuffer->getSize(),
			},

			// Instance
			{
				.sourceOffset = dataBuffer->getSize(),
				.destinationOffset = 0,
				.size = instanceBuffer->getSize(),
			},
		};

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy buffers",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, dataBuffer, copyRegions[0]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, copyRegions[1]);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit(commandBuffer);
		transferQueue->waitIdle();

		transitionOutputImage();
		updateBufferDescriptorSets(vertexPositionComponentRegionSize, vertexNormalComponentRegionSize, vertexTextureCoordinateComponentRegionSize, vertexPositionIndexRegionSize, vertexNormalIndexRegionSize, vertexTextureCoordinateIndexRegionSize, clusterRegionSize, geometryRegionSize);
		updateDebugInputAttachmentDescriptorSets();
		createMaterialObjects();
	}

	void VisibilityRenderer::postSetCameraController() {
		const PerspectiveCamera3D& camera = getCameraController<PerspectiveCamera3D>().getCamera();

		cameraMappedRange->nearPlane = camera.nearPlane;
		cameraMappedRange->farPlane = camera.farPlane;
	}

	void VisibilityRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const Extent2D extent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(extent.width);
		const float32 height = static_cast<float32>(extent.height);

		BufferMemoryBarrier bufferMemoryBarrier;

		// Visibility render pass
		{
			const sequence<ref<DescriptorSet>> descriptorSets = {
				cameraDescriptorSet,
				clusterDescriptorSet,
				instanceDescriptorSet,
			};

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
			commandEncoder->bindDescriptorSets(visibilityPipelineLayout, 0, descriptorSets);
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
				const sequence<ImageMemoryBarrier> imageMemoryBarriers = {
					visibilityImageMemoryBarrier,
					{
						.image = depthStencilImage,
						.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.newAccess = Access::SHADER_READ,
					},
				};

				commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

				const sequence<ref<DescriptorSet>> descriptorSets = {
					visibilityDebugDescriptorSet,
					clusterDescriptorSet,
					instanceDescriptorSet,
				};

				const uint32 groupCountX = static_cast<uint32>(ceil(width / 16.0f));
				const uint32 groupCountY = static_cast<uint32>(ceil(height / 16.0f));

				commandEncoder->bindComputePipeline(visibilityDebugPipeline);
				commandEncoder->bindDescriptorSets(visibilityDebugPipelineLayout, 0, descriptorSets);
				commandEncoder->dispatch(groupCountX, groupCountY, 1);
			}
		#else
			commandEncoder->insertMemoryBarriers({}, visibilityImageMemoryBarrier);

			// Material compute pass
			{
				const uint32 groupCountX = static_cast<uint32>(ceil(width / 16.0f));
				const uint32 groupCountY = static_cast<uint32>(ceil(height / 16.0f));

				// Count the number of pixels used by each material.
				commandEncoder->bindComputePipeline(materialCountPipeline);
				commandEncoder->bindDescriptorSets(materialCountPipelineLayout, 0, materialCountDescriptorSet);
				commandEncoder->dispatch(groupCountX, groupCountY, 1);

				bufferMemoryBarrier = {
					.buffer = materialCountBuffer,
					.offset = 0,
					.size = materialCountBuffer->getSize(),
					.newAccess = Access::SHADER_READ | Access::SHADER_WRITE,
				};

				commandEncoder->insertMemoryBarriers(bufferMemoryBarrier, {});

				// Get the material pixel offsets with a prefix sum and write indirect arguments for each material pipeline.
				commandEncoder->bindComputePipeline(materialOffsetPipeline);
				commandEncoder->bindDescriptorSets(materialOffsetPipelineLayout, 0, materialOffsetDescriptorSet);
				commandEncoder->dispatch(1, 1, 1);

				// Re-order the pixels per material index.
				commandEncoder->bindComputePipeline(materialCoordinatePipeline);
				commandEncoder->bindDescriptorSets(materialCoordinatePipelineLayout, 0, materialCoordinateDescriptorSet);
				commandEncoder->dispatch(groupCountX, groupCountY, 1);

				bufferMemoryBarrier = {
					.buffer = materialOffsetBuffer,
					.offset = 0,
					.size = materialOffsetBuffer->getSize(),
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers(bufferMemoryBarrier, {});

				sequence<ref<DescriptorSet>> descriptorSets = {materialBaseDescriptorSet, nullptr};
				uint32 materialShaderIndex = 0;

				while (materialShaderIndex < scene->materialShaders.count()) {
					descriptorSets[1] = materialShaderUserDescriptorSets[materialShaderIndex];

					commandEncoder->bindComputePipeline(materialShaderPipelines[materialShaderIndex]);
					commandEncoder->bindDescriptorSets(materialShaderPipelineLayouts[materialShaderIndex], 0, descriptorSets);
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
			commandEncoder->bindDescriptorSets(outputPipelineLayout, 0, outputDescriptorSet);
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
		cameraMappedRange->position = getCameraController<PerspectiveCamera3D>().getCamera().position;
	}

	void VisibilityRenderer::updateCameraProjectionView() const {
		cameraMappedRange->projectionView = getCameraController<PerspectiveCamera3D>().getCamera().getProjectionView();
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
		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(sequence<ref<DescriptorSetLayout>>{
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
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Transition output image",
				#endif
			});

			commandEncoder->begin();

			const ImageMemoryBarrier outputImageMemoryBarrier = {
				.image = outputImage,
				.newLayout = ImageLayout::GENERAL,
				.newAccess = Access::SHADER_WRITE,
			};

			commandEncoder->insertMemoryBarriers({}, outputImageMemoryBarrier);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			graphicsQueue->submit(commandBuffer);
			graphicsQueue->waitIdle();
		}
	}

	void VisibilityRenderer::writeVisibilityIndirectCommand() const {
		const BufferView<DrawIndirectCommand> visibilityIndirectView = visibilityIndirectBuffer->createView<DrawIndirectCommand>(0, visibilityIndirectBuffer->getSize());

		DrawIndirectCommand* visibilityIndirectMappedRange = visibilityIndirectView.map();
		visibilityIndirectMappedRange[0].vertexCount = CLUSTER_INDEX_COUNT;
		visibilityIndirectMappedRange[0].instanceCount = static_cast<uint32>(scene->clusters.count());
		visibilityIndirectMappedRange[0].vertexOffset = 0;
		visibilityIndirectMappedRange[0].instanceOffset = 0;

		visibilityIndirectBuffer->unmap();
	}

	void VisibilityRenderer::updateBufferDescriptorSets(uint64 vertexPositionComponentRegionSize, uint64 vertexNormalComponentRegionSize, uint64 vertexTextureCoordinateComponentRegionSize, uint64 vertexPositionIndexRegionSize, uint64 vertexNormalIndexRegionSize, uint64 vertexTextureCoordinateIndexRegionSize, uint64 clusterRegionSize, uint64 geometryRegionSize) {
		const BufferView<Camera> cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());
		const BufferView<float32> vertexPositionComponentView = dataBuffer->createView<float32>(0, vertexPositionComponentRegionSize);
		const BufferView<float32> vertexNormalComponentView = dataBuffer->createView<float32>(vertexPositionComponentRegionSize, vertexNormalComponentRegionSize);
		const BufferView<float32> vertexTextureCoordinateComponentView = dataBuffer->createView<float32>(vertexPositionComponentRegionSize + vertexNormalComponentRegionSize, vertexTextureCoordinateComponentRegionSize);
		const BufferView<uint32> vertexPositionIndexView = dataBuffer->createView<uint32>(vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize, vertexPositionIndexRegionSize);
		const BufferView<uint32> vertexNormalIndexView = dataBuffer->createView<uint32>(vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize + vertexPositionIndexRegionSize, vertexNormalIndexRegionSize);
		const BufferView<uint32> vertexTextureCoordinateIndexView = dataBuffer->createView<uint32>(vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize + vertexPositionIndexRegionSize + vertexNormalIndexRegionSize, vertexTextureCoordinateIndexRegionSize);
		const BufferView<Cluster> clusterView = dataBuffer->createView<Cluster>(vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize + vertexPositionIndexRegionSize + vertexNormalIndexRegionSize + vertexTextureCoordinateIndexRegionSize, clusterRegionSize);
		const BufferView<Geometry> geometryView = dataBuffer->createView<Geometry>(vertexPositionComponentRegionSize + vertexNormalComponentRegionSize + vertexTextureCoordinateComponentRegionSize + vertexPositionIndexRegionSize + vertexNormalIndexRegionSize + vertexTextureCoordinateIndexRegionSize + clusterRegionSize, geometryRegionSize);
		const BufferView<ClusteredInstance> instanceView = instanceBuffer->createView<ClusteredInstance>(0, scene->instances.count() * sizeof(ClusteredInstance));
		const BufferView<uint32> materialCountView = materialCountBuffer->createView<uint32>(0, materialCountBuffer->getSize());
		const BufferView<uint32> materialOffsetView = materialOffsetBuffer->createView<uint32>(sizeof(uint32), materialOffsetBuffer->getSize() - sizeof(uint32));
		const BufferView<DispatchIndirectCommand> materialShaderIndirectView = materialShaderIndirectBuffer->createView<DispatchIndirectCommand>(0, materialShaderIndirectBuffer->getSize());

		const sequence<WriteDescriptorSet> writeDescriptorSets = {
			{
				.descriptorSet = cameraDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = cameraView,
			},
			{
				.descriptorSet = clusterDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = vertexPositionComponentView,
			},
			{
				.descriptorSet = clusterDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = vertexPositionIndexView,
			},
			{
				.descriptorSet = clusterDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = clusterView,
			},
			{
				.descriptorSet = instanceDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = instanceView,
			},
			{
				.descriptorSet = instanceDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = geometryView,
			},
			{
				.descriptorSet = materialCountDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = cameraView,
			},
			{
				.descriptorSet = materialCountDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = clusterView,
			},
			{
				.descriptorSet = materialCountDescriptorSet,
				.bindingIndex = 3,
				.descriptorBufferViews = instanceView,
			},
			{
				.descriptorSet = materialCountDescriptorSet,
				.bindingIndex = 4,
				.descriptorBufferViews = materialCountView,
			},
			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				{
					.descriptorSet = visibilityDebugDescriptorSet,
					.bindingIndex = 0,
					.descriptorBufferViews = cameraView,
				},
				{
					.descriptorSet = visibilityDebugDescriptorSet,
					.bindingIndex = 3,
					.descriptorBufferViews = vertexPositionComponentView,
				},
				{
					.descriptorSet = visibilityDebugDescriptorSet,
					.bindingIndex = 4,
					.descriptorBufferViews = vertexNormalComponentView,
				},
				{
					.descriptorSet = visibilityDebugDescriptorSet,
					.bindingIndex = 5,
					.descriptorBufferViews = vertexTextureCoordinateComponentView,
				},
				{
					.descriptorSet = visibilityDebugDescriptorSet,
					.bindingIndex = 6,
					.descriptorBufferViews = vertexPositionIndexView,
				},
				{
					.descriptorSet = visibilityDebugDescriptorSet,
					.bindingIndex = 7,
					.descriptorBufferViews = vertexNormalIndexView,
				},
				{
					.descriptorSet = visibilityDebugDescriptorSet,
					.bindingIndex = 8,
					.descriptorBufferViews = vertexTextureCoordinateIndexView,
				},
			#endif
			{
				.descriptorSet = materialOffsetDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = materialCountView,
			},
			{
				.descriptorSet = materialOffsetDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = materialOffsetView,
			},
			{
				.descriptorSet = materialOffsetDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = materialShaderIndirectView,
			},
			{
				.descriptorSet = materialCoordinateDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = cameraView,
			},
			{
				.descriptorSet = materialCoordinateDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = clusterView,
			},
			{
				.descriptorSet = materialCoordinateDescriptorSet,
				.bindingIndex = 3,
				.descriptorBufferViews = instanceView,
			},
			{
				.descriptorSet = materialCoordinateDescriptorSet,
				.bindingIndex = 4,
				.descriptorBufferViews = materialOffsetView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = cameraView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 3,
				.descriptorBufferViews = vertexPositionComponentView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 4,
				.descriptorBufferViews = vertexNormalComponentView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 5,
				.descriptorBufferViews = vertexTextureCoordinateComponentView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 6,
				.descriptorBufferViews = vertexPositionIndexView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 7,
				.descriptorBufferViews = vertexNormalIndexView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 8,
				.descriptorBufferViews = vertexTextureCoordinateIndexView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 9,
				.descriptorBufferViews = clusterView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 10,
				.descriptorBufferViews = instanceView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 11,
				.descriptorBufferViews = geometryView,
			},
		};

		getDevice()->updateDescriptorSets(writeDescriptorSets);
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

		const BufferView<uint32> materialCoordinateView = materialCoordinateBuffer->createView<uint32>(0, materialCoordinateBuffer->getSize());

		const sequence<WriteDescriptorSet> writeDescriptorSets = {
			{
				.descriptorSet = materialCountDescriptorSet,
				.bindingIndex = 1,
				.descriptorImages = visibilityDescriptorImage,
			},
			{
				.descriptorSet = materialCoordinateDescriptorSet,
				.bindingIndex = 1,
				.descriptorImages = visibilityDescriptorImage,
			},
			{
				.descriptorSet = materialCoordinateDescriptorSet,
				.bindingIndex = 5,
				.descriptorBufferViews = materialCoordinateView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = materialCoordinateView,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 2,
				.descriptorImages = visibilityDescriptorImage,
			},
			{
				.descriptorSet = materialBaseDescriptorSet,
				.bindingIndex = 12,
				.descriptorImages = outputDescriptorImage,
			},
			{
				.descriptorSet = outputDescriptorSet,
				.bindingIndex = 0,
				.descriptorImages = outputDescriptorImage,
			},
			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				{
					.descriptorSet = visibilityDebugDescriptorSet,
					.bindingIndex = 1,
					.descriptorImages = visibilityDescriptorImage,
				},
				{
					.descriptorSet = visibilityDebugDescriptorSet,
					.bindingIndex = 2,
					.descriptorImages = depthStencilDescriptorImage,
				},
				{
					.descriptorSet = visibilityDebugDescriptorSet,
					.bindingIndex = 9,
					.descriptorImages = outputDescriptorImage,
				},
			#endif
		};

		getDevice()->updateDescriptorSets(writeDescriptorSets);
	}

	bool VisibilityRenderer::onWindowResize(WindowResizeEvent& event) {
		resize(event);

		return true;
	}
}