// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/Visibility/CameraBuffer.hpp"
#include "Engine/Renderer/Visibility/CameraDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/ClusterDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/DebugDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/DebugPipeline.hpp"
#include "Engine/Renderer/Visibility/DebugPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/DepthStencilImage.hpp"
#include "Engine/Renderer/Visibility/InstanceDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialBaseDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialCoordinateBuffer.hpp"
#include "Engine/Renderer/Visibility/MaterialCoordinateDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialCoordinatePipeline.hpp"
#include "Engine/Renderer/Visibility/MaterialCoordinatePipelineLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialCountBuffer.hpp"
#include "Engine/Renderer/Visibility/MaterialIndirectBuffer.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetBuffer.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetPipeline.hpp"
#include "Engine/Renderer/Visibility/MaterialOffsetPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/MaterialPipeline.hpp"
#include "Engine/Renderer/Visibility/MaterialPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/OutputDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/OutputImage.hpp"
#include "Engine/Renderer/Visibility/OutputPipeline.hpp"
#include "Engine/Renderer/Visibility/OutputPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/Renderer.hpp"
#include "Engine/Renderer/Visibility/RenderPass.hpp"
#include "Engine/Renderer/Visibility/VisibilityImage.hpp"
#include "Engine/Renderer/Visibility/VisibilityIndirectBuffer.hpp"
#include "Engine/Renderer/Visibility/VisibilityMaterialCountDescriptorSetLayout.hpp"
#include "Engine/Renderer/Visibility/VisibilityPipelineLayout.hpp"
#include "Engine/Renderer/Visibility/VisibilityPipeline.hpp"

namespace Trident {
	void VisibilityRenderer::build() {
		renderPass = _createRenderPass();

		createFrameResources();

		cameraDescriptorSetLayout = createCameraDescriptorSetLayout();
		clusterDescriptorSetLayout = createClusterDescriptorSetLayout();
		instanceDescriptorSetLayout = createInstanceDescriptorSetLayout();
		visibilityMaterialCountDescriptorSetLayout = createVisibilityMaterialCountDescriptorSetLayout();
		materialOffsetDescriptorSetLayout = createMaterialOffsetDescriptorSetLayout();
		materialCoordinateDescriptorSetLayout = createMaterialCoordinateDescriptorSetLayout();
		materialBaseDescriptorSetLayout = createMaterialBaseDescriptorSetLayout();
		outputDescriptorSetLayout = createOutputDescriptorSetLayout();
		debugDescriptorSetLayout = createDebugDescriptorSetLayout();

		allocateDescriptorSets();

		visibilityPipelineLayout = createVisibilityPipelineLayout();
		materialOffsetPipelineLayout = createMaterialOffsetPipelineLayout();
		materialCoordinatePipelineLayout = createMaterialCoordinatePipelineLayout();
		outputPipelineLayout = createOutputPipelineLayout();
		debugPipelineLayout = createDebugPipelineLayout();

		visibilityPipeline = createVisibilityPipeline();
		materialOffsetPipeline = createMaterialOffsetPipeline();
		materialCoordinatePipeline = createMaterialCoordinatePipeline();
		outputPipeline = createOutputPipeline();
		debugPipeline = createDebugPipeline();

		visibilityIndirectBuffer = createVisibilityIndirectBuffer();

		cameraBuffer = createCameraBuffer();
		cameraMappedRange = cameraBuffer->map<Camera>(0, cameraBuffer->size);

		updateViewport();
	}

	void VisibilityRenderer::postSetScene() {
		createVertexPositionComponentBuffer();
		createVertexNormalComponentBuffer();
		createVertexTextureCoordinateComponentBuffer();
		createVertexPositionIndexBuffer();
		createVertexNormalIndexBuffer();
		createVertexTextureCoordinateIndexBuffer();
		createClusterBuffer();
		createInstanceBuffer();
		createGeometryBuffer();
		materialCountBuffer = createMaterialCountBuffer();
		materialOffsetBuffer = createMaterialOffsetBuffer();
		materialIndirectBuffer = createMaterialIndirectBuffer();

		fillSceneBuffers();

		updateBufferDescriptorSets();
		updateDebugInputAttachmentDescriptorSets();

		createMaterialPipelineLayouts();
		createMaterialPipelines();
	}

	void VisibilityRenderer::postSetCameraController() {
		const PerspectiveCamera& camera = getCameraController<PerspectiveCamera>().getCamera();

		cameraMappedRange->nearPlane = camera.getNearPlane();
		cameraMappedRange->farPlane = camera.getFarPlane();
	}

	void VisibilityRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const ClearValue swapchainClearValue = {0.0f, 0.0f, 0.0f, 1.0f};
		const ClearValue visibilityClearValue = {0.0f, 0.0f, 0.0f, 1.0f};

		ClearValue depthStencilClearValue{};
		depthStencilClearValue.depth = 1.0f;
		depthStencilClearValue.stencil = 0;

		BeginRenderPassDescriptor beginRenderPassDescriptor{};
		beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
		beginRenderPassDescriptor.clearValues = {swapchainClearValue, visibilityClearValue, depthStencilClearValue};

		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(swapchainExtent.width);
		const float32 height = static_cast<float32>(swapchainExtent.height);

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchainExtent);

		// Visibility subpass
		{
			// Write pixel visibility and count per material.
			commandEncoder->bindRenderPipeline(visibilityPipeline);
			commandEncoder->bindDescriptorSets(visibilityPipelineLayout, {cameraDescriptorSet, clusterDescriptorSet, instanceDescriptorSet, visibilityMaterialCountDescriptorSet});
			commandEncoder->drawIndirect(visibilityIndirectBuffer, 0, 1, 0);
		}

		// Get the material pixel offsets with a prefix sum and write indirect arguments for each material pipeline.
		commandEncoder->bindComputePipeline(materialOffsetPipeline);
		commandEncoder->bindDescriptorSets(materialOffsetPipelineLayout, {materialOffsetDescriptorSet});
		commandEncoder->dispatch(1, 1, 1);

		commandEncoder->setImageMemoryBarrier(visibilityImage, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		// Re-order the pixels per material index.
		{
			const Extent2D extent = getSwapchain()->getExtent();
			const uint32 groupCountX = static_cast<uint32>(ceil(static_cast<float32>(extent.width) / 16.0f));
			const uint32 groupCountY = static_cast<uint32>(ceil(static_cast<float32>(extent.height) / 16.0f));

			commandEncoder->bindComputePipeline(materialCoordinatePipeline);
			commandEncoder->bindDescriptorSets(materialCoordinatePipelineLayout, {materialCoordinateDescriptorSet});
			commandEncoder->dispatch(groupCountX, groupCountY, 1);
		}

		// Material subpass
		{
			uint32 materialPipelineIndex = 0;
			uint32 materialIndex;

			while (materialPipelineIndex < scene->materialPipelineDescriptors.count()) {
				commandEncoder->bindComputePipeline(materialPipelines[materialPipelineIndex]);

				const MaterialPipelineDescriptor& materialPipeline = scene->materialPipelineDescriptors[materialPipelineIndex];

				materialIndex = 0;

				while (materialIndex < materialPipeline.materialCount) {
					// TODO: Custom material descriptor sets.
					// commandEncoder->bindDescriptorSets(materialPipeline.descriptorSetLayoutBindings);

					commandEncoder->dispatchIndirect(materialIndirectBuffer, sizeof(DispatchIndirectCommand) * materialPipelineIndex);

					materialIndex += 1;
				}

				materialPipelineIndex += 1;
			}
		}

		commandEncoder->nextSubpass();

		#if 1
			// Output subpass
			commandEncoder->bindRenderPipeline(outputPipeline);
			commandEncoder->bindDescriptorSets(outputPipelineLayout, {outputDescriptorSet});
			commandEncoder->draw(3, 1, 0, 0);
		#else
			// Debug subpass
			commandEncoder->bindRenderPipeline(debugPipeline);
			commandEncoder->bindDescriptorSets(debugPipelineLayout, {debugDescriptorSet, clusterDescriptorSet, instanceDescriptorSet});
			commandEncoder->draw(3, 1, 0, 0);
		#endif

		commandEncoder->endRenderPass();
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

		recreateFramebuffers();
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

	void VisibilityRenderer::recreateFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor{};
		framebufferDescriptor.renderPass = renderPass;
		framebufferDescriptor.imageViews = {nullptr, visibilityImageView, depthStencilImageView};
		framebufferDescriptor.extent = swapchain->getExtent();

		#if defined(BUILD_MODE_DEBUG)
			sequence<char8> name;
		#endif

		uint32 framebufferIndex = 0;

		while (framebufferIndex < framebuffers.count()) {
			#if defined(BUILD_MODE_DEBUG)
				name = Logger::format("In flight framebuffer []", framebufferIndex);

				framebufferDescriptor.name = name;
			#endif

			framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

			framebufferIndex += 1;
		}
	}

	void VisibilityRenderer::createVertexPositionComponentBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexPositionComponentBufferDescriptor{};
		vertexPositionComponentBufferDescriptor.size = scene->vertexPositionComponents.count() * sizeof(float32);
		vertexPositionComponentBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		vertexPositionComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(vertexPositionComponentBufferDescriptor, "Vertex position component");

		vertexPositionComponentBuffer = createBuffer(vertexPositionComponentBufferDescriptor);
	}

	void VisibilityRenderer::createVertexNormalComponentBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexNormalComponentBufferDescriptor{};
		vertexNormalComponentBufferDescriptor.size = scene->vertexNormalComponents.count() * sizeof(float32);
		vertexNormalComponentBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		vertexNormalComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(vertexNormalComponentBufferDescriptor, "Vertex normal component");

		vertexNormalComponentBuffer = createBuffer(vertexNormalComponentBufferDescriptor);
	}

	void VisibilityRenderer::createVertexTextureCoordinateComponentBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexTextureCoordinateComponentBufferDescriptor{};
		vertexTextureCoordinateComponentBufferDescriptor.size = scene->vertexTextureCoordinateComponents.count() * sizeof(float32);
		vertexTextureCoordinateComponentBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		vertexTextureCoordinateComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(vertexTextureCoordinateComponentBufferDescriptor, "Vertex texture coordinate component");

		vertexTextureCoordinateComponentBuffer = createBuffer(vertexTextureCoordinateComponentBufferDescriptor);
	}

	void VisibilityRenderer::createVertexPositionIndexBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexPositionIndexBufferDescriptor{};
		vertexPositionIndexBufferDescriptor.size = scene->vertexPositionIndices.count() * sizeof(uint32);
		vertexPositionIndexBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		vertexPositionIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(vertexPositionIndexBufferDescriptor, "Vertex position index");

		vertexPositionIndexBuffer = createBuffer(vertexPositionIndexBufferDescriptor);
	}

	void VisibilityRenderer::createVertexNormalIndexBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexNormalIndexBufferDescriptor{};
		vertexNormalIndexBufferDescriptor.size = scene->vertexNormalIndices.count() * sizeof(uint32);
		vertexNormalIndexBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		vertexNormalIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(vertexNormalIndexBufferDescriptor, "Vertex normal index");

		vertexNormalIndexBuffer = createBuffer(vertexNormalIndexBufferDescriptor);
	}

	void VisibilityRenderer::createVertexTextureCoordinateIndexBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexTextureCoordinateIndexBufferDescriptor{};
		vertexTextureCoordinateIndexBufferDescriptor.size = scene->vertexTextureCoordinateIndices.count() * sizeof(uint32);
		vertexTextureCoordinateIndexBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		vertexTextureCoordinateIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(vertexTextureCoordinateIndexBufferDescriptor, "Vertex texture coordinate index");

		vertexTextureCoordinateIndexBuffer = createBuffer(vertexTextureCoordinateIndexBufferDescriptor);
	}

	void VisibilityRenderer::createClusterBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor clusterBufferDescriptor{};
		clusterBufferDescriptor.size = scene->clusters.count() * sizeof(Cluster);
		clusterBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		clusterBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(clusterBufferDescriptor, "Cluster");

		clusterBuffer = createBuffer(clusterBufferDescriptor);
	}

	void VisibilityRenderer::createInstanceBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor instanceBufferDescriptor{};
		instanceBufferDescriptor.size = scene->instances.count() * sizeof(ClusteredInstance);
		instanceBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		instanceBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(instanceBufferDescriptor, "Instance");

		instanceBuffer = createBuffer(instanceBufferDescriptor);

		instanceMappedRange = instanceBuffer->map<ClusteredInstance>(0, instanceBuffer->size);
	}

	void VisibilityRenderer::createGeometryBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor geometryBufferDescriptor{};
		geometryBufferDescriptor.size = scene->geometries.count() * sizeof(Geometry);
		geometryBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		geometryBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(geometryBufferDescriptor, "Geometry");

		geometryBuffer = createBuffer(geometryBufferDescriptor);
	}

	void VisibilityRenderer::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({
			cameraDescriptorSetLayout,
			clusterDescriptorSetLayout,
			instanceDescriptorSetLayout,
			visibilityMaterialCountDescriptorSetLayout,
			materialOffsetDescriptorSetLayout,
			materialCoordinateDescriptorSetLayout,
			materialBaseDescriptorSetLayout,
			outputDescriptorSetLayout,
			debugDescriptorSetLayout,
		});

		cameraDescriptorSet = descriptorSets[0];
		clusterDescriptorSet = descriptorSets[1];
		instanceDescriptorSet = descriptorSets[2];
		visibilityMaterialCountDescriptorSet = descriptorSets[3];
		materialOffsetDescriptorSet = descriptorSets[4];
		materialCoordinateDescriptorSet = descriptorSets[5];
		materialBaseDescriptorSet = descriptorSets[6];
		outputDescriptorSet = descriptorSets[7];
		debugDescriptorSet = descriptorSets[8];
	}

	void VisibilityRenderer::fillSceneBuffers() const {
		{
			DrawIndirectCommand* visibilityIndirectMappedRange = visibilityIndirectBuffer->map<DrawIndirectCommand>(0, visibilityIndirectBuffer->size);
			visibilityIndirectMappedRange->vertexCount = CLUSTER_INDEX_COUNT;
			visibilityIndirectMappedRange->instanceCount = static_cast<uint32>(scene->clusters.count());
			visibilityIndirectMappedRange->vertexOffset = 0;
			visibilityIndirectMappedRange->instanceOffset = 0;

			visibilityIndirectBuffer->unmap();
		}

		const ref<Buffer> stagingBuffer = createStagingBuffer(
			vertexPositionComponentBuffer->size +
			vertexNormalComponentBuffer->size +
			vertexTextureCoordinateComponentBuffer->size +
			vertexPositionIndexBuffer->size +
			vertexNormalIndexBuffer->size +
			vertexTextureCoordinateIndexBuffer->size +
			clusterBuffer->size +
			instanceBuffer->size +
			geometryBuffer->size
		);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->size);
		uint64 sourceOffset = 0;

		float32* vertexPositionComponentMappedRange = pointer_cast<float32*>(mappedRange + sourceOffset);

		fillVertexPositionComponentBuffer(vertexPositionComponentMappedRange);

		BufferCopyRegion vertexPositionComponentBufferCopyRegion{};
		vertexPositionComponentBufferCopyRegion.sourceOffset = sourceOffset;
		vertexPositionComponentBufferCopyRegion.destinationOffset = 0;
		vertexPositionComponentBufferCopyRegion.size = vertexPositionComponentBuffer->size;

		sourceOffset += vertexPositionComponentBuffer->size;

		float32* vertexNormalComponentMappedRange = pointer_cast<float32*>(mappedRange + sourceOffset);

		fillVertexNormalComponentBuffer(vertexNormalComponentMappedRange);

		BufferCopyRegion vertexNormalComponentBufferCopyRegion{};
		vertexNormalComponentBufferCopyRegion.sourceOffset = sourceOffset;
		vertexNormalComponentBufferCopyRegion.destinationOffset = 0;
		vertexNormalComponentBufferCopyRegion.size = vertexNormalComponentBuffer->size;

		sourceOffset += vertexNormalComponentBuffer->size;

		float32* vertexTextureCoordinateComponentMappedRange = pointer_cast<float32*>(mappedRange + sourceOffset);

		fillVertexTextureCoordinateComponentBuffer(vertexTextureCoordinateComponentMappedRange);

		BufferCopyRegion vertexTextureCoordinateComponentBufferCopyRegion{};
		vertexTextureCoordinateComponentBufferCopyRegion.sourceOffset = sourceOffset;
		vertexTextureCoordinateComponentBufferCopyRegion.destinationOffset = 0;
		vertexTextureCoordinateComponentBufferCopyRegion.size = vertexTextureCoordinateComponentBuffer->size;

		sourceOffset += vertexTextureCoordinateComponentBuffer->size;

		uint32* vertexPositionIndexMappedRange = pointer_cast<uint32*>(mappedRange + sourceOffset);

		fillVertexPositionIndexBuffer(vertexPositionIndexMappedRange);

		BufferCopyRegion vertexPositionIndexBufferCopyRegion{};
		vertexPositionIndexBufferCopyRegion.sourceOffset = sourceOffset;
		vertexPositionIndexBufferCopyRegion.destinationOffset = 0;
		vertexPositionIndexBufferCopyRegion.size = vertexPositionIndexBuffer->size;

		sourceOffset += vertexPositionIndexBuffer->size;

		uint32* vertexNormalIndexMappedRange = pointer_cast<uint32*>(mappedRange + sourceOffset);

		fillVertexNormalIndexBuffer(vertexNormalIndexMappedRange);

		BufferCopyRegion vertexNormalIndexBufferCopyRegion{};
		vertexNormalIndexBufferCopyRegion.sourceOffset = sourceOffset;
		vertexNormalIndexBufferCopyRegion.destinationOffset = 0;
		vertexNormalIndexBufferCopyRegion.size = vertexNormalIndexBuffer->size;

		sourceOffset += vertexNormalIndexBuffer->size;

		uint32* vertexTextureCoordinateIndexMappedRange = pointer_cast<uint32*>(mappedRange + sourceOffset);

		fillVertexTextureCoordinateIndexBuffer(vertexTextureCoordinateIndexMappedRange);

		BufferCopyRegion vertexTextureCoordinateIndexBufferCopyRegion{};
		vertexTextureCoordinateIndexBufferCopyRegion.sourceOffset = sourceOffset;
		vertexTextureCoordinateIndexBufferCopyRegion.destinationOffset = 0;
		vertexTextureCoordinateIndexBufferCopyRegion.size = vertexTextureCoordinateIndexBuffer->size;

		sourceOffset += vertexTextureCoordinateIndexBuffer->size;

		Cluster* clusterMappedRange = pointer_cast<Cluster*>(mappedRange + sourceOffset);

		fillClusterBuffer(clusterMappedRange);

		BufferCopyRegion clusterBufferCopyRegion{};
		clusterBufferCopyRegion.sourceOffset = sourceOffset;
		clusterBufferCopyRegion.destinationOffset = 0;
		clusterBufferCopyRegion.size = clusterBuffer->size;

		sourceOffset += clusterBuffer->size;

		ClusteredInstance* instanceMappedRange = pointer_cast<ClusteredInstance*>(mappedRange + sourceOffset);

		fillInstanceBuffer(instanceMappedRange);

		BufferCopyRegion instanceBufferCopyRegion{};
		instanceBufferCopyRegion.sourceOffset = sourceOffset;
		instanceBufferCopyRegion.destinationOffset = 0;
		instanceBufferCopyRegion.size = instanceBuffer->size;

		sourceOffset += instanceBuffer->size;

		Geometry* geometryMappedRange = pointer_cast<Geometry*>(mappedRange + sourceOffset);

		fillGeometryBuffer(geometryMappedRange);

		BufferCopyRegion geometryBufferCopyRegion{};
		geometryBufferCopyRegion.sourceOffset = sourceOffset;
		geometryBufferCopyRegion.destinationOffset = 0;
		geometryBufferCopyRegion.size = geometryBuffer->size;

		stagingBuffer->unmap();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		SET_DEBUG_NAME(commandBufferDescriptor, "Buffer copy command encoder");

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexPositionComponentBuffer, {vertexPositionComponentBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexNormalComponentBuffer, {vertexNormalComponentBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexTextureCoordinateComponentBuffer, {vertexTextureCoordinateComponentBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexPositionIndexBuffer, {vertexPositionIndexBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexNormalIndexBuffer, {vertexNormalIndexBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexTextureCoordinateIndexBuffer, {vertexTextureCoordinateIndexBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, clusterBuffer, {clusterBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, {instanceBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, geometryBuffer, {geometryBufferCopyRegion});

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
		transferQueue->waitIdle();
	}

	void VisibilityRenderer::fillVertexPositionComponentBuffer(float32* mappedRange) const {
		std::copy(scene->vertexPositionComponents.begin(), scene->vertexPositionComponents.end(), mappedRange);
	}

	void VisibilityRenderer::fillVertexNormalComponentBuffer(float32* mappedRange) const {
		std::copy(scene->vertexNormalComponents.begin(), scene->vertexNormalComponents.end(), mappedRange);
	}

	void VisibilityRenderer::fillVertexTextureCoordinateComponentBuffer(float32* mappedRange) const {
		std::copy(scene->vertexTextureCoordinateComponents.begin(), scene->vertexTextureCoordinateComponents.end(), mappedRange);
	}

	void VisibilityRenderer::fillVertexPositionIndexBuffer(uint32* mappedRange) const {
		std::copy(scene->vertexPositionIndices.begin(), scene->vertexPositionIndices.end(), mappedRange);
	}

	void VisibilityRenderer::fillVertexNormalIndexBuffer(uint32* mappedRange) const {
		std::copy(scene->vertexNormalIndices.begin(), scene->vertexNormalIndices.end(), mappedRange);
	}

	void VisibilityRenderer::fillVertexTextureCoordinateIndexBuffer(uint32* mappedRange) const {
		std::copy(scene->vertexTextureCoordinateIndices.begin(), scene->vertexTextureCoordinateIndices.end(), mappedRange);
	}

	void VisibilityRenderer::fillClusterBuffer(Cluster* mappedRange) const {
		std::copy(scene->clusters.begin(), scene->clusters.end(), mappedRange);
	}

	void VisibilityRenderer::fillInstanceBuffer(ClusteredInstance* mappedRange) const {
		std::copy(scene->instances.begin(), scene->instances.end(), mappedRange);
	}

	void VisibilityRenderer::fillGeometryBuffer(Geometry* mappedRange) const {
		uint32 geometryIndex = 0;

		while (geometryIndex < scene->geometries.count()) {
			mappedRange[geometryIndex].vertexOffset = scene->geometries[geometryIndex].vertexOffset;
			mappedRange[geometryIndex].clusterCount = scene->geometries[geometryIndex].clusterCount;

			geometryIndex += 1;
		}
	}

	void VisibilityRenderer::updateBufferDescriptorSets() {
		DescriptorBuffer cameraDescriptorBuffer{};
		cameraDescriptorBuffer.buffer = cameraBuffer;
		cameraDescriptorBuffer.offset = 0;
		cameraDescriptorBuffer.size = cameraBuffer->size;

		DescriptorBuffer clusterDescriptorBuffer{};
		clusterDescriptorBuffer.buffer = clusterBuffer;
		clusterDescriptorBuffer.offset = 0;
		clusterDescriptorBuffer.size = clusterBuffer->size;

		DescriptorBuffer instanceDescriptorBuffer{};
		instanceDescriptorBuffer.buffer = instanceBuffer;
		instanceDescriptorBuffer.offset = 0;
		instanceDescriptorBuffer.size = instanceBuffer->size;

		DescriptorBuffer geometryDescriptorBuffer{};
		geometryDescriptorBuffer.buffer = geometryBuffer;
		geometryDescriptorBuffer.offset = 0;
		geometryDescriptorBuffer.size = geometryBuffer->size;

		DescriptorBuffer vertexPositionComponentDescriptorBuffer{};
		vertexPositionComponentDescriptorBuffer.buffer = vertexPositionComponentBuffer;
		vertexPositionComponentDescriptorBuffer.offset = 0;
		vertexPositionComponentDescriptorBuffer.size = vertexPositionComponentBuffer->size;

		DescriptorBuffer vertexNormalComponentDescriptorBuffer{};
		vertexNormalComponentDescriptorBuffer.buffer = vertexNormalComponentBuffer;
		vertexNormalComponentDescriptorBuffer.offset = 0;
		vertexNormalComponentDescriptorBuffer.size = vertexNormalComponentBuffer->size;

		DescriptorBuffer vertexTextureCoordinateComponentDescriptorBuffer{};
		vertexTextureCoordinateComponentDescriptorBuffer.buffer = vertexTextureCoordinateComponentBuffer;
		vertexTextureCoordinateComponentDescriptorBuffer.offset = 0;
		vertexTextureCoordinateComponentDescriptorBuffer.size = vertexTextureCoordinateComponentBuffer->size;

		DescriptorBuffer vertexPositionIndexDescriptorBuffer{};
		vertexPositionIndexDescriptorBuffer.buffer = vertexPositionIndexBuffer;
		vertexPositionIndexDescriptorBuffer.offset = 0;
		vertexPositionIndexDescriptorBuffer.size = vertexPositionIndexBuffer->size;

		DescriptorBuffer vertexNormalIndexDescriptorBuffer{};
		vertexNormalIndexDescriptorBuffer.buffer = vertexNormalIndexBuffer;
		vertexNormalIndexDescriptorBuffer.offset = 0;
		vertexNormalIndexDescriptorBuffer.size = vertexNormalIndexBuffer->size;

		DescriptorBuffer vertexTextureCoordinateIndexDescriptorBuffer{};
		vertexTextureCoordinateIndexDescriptorBuffer.buffer = vertexTextureCoordinateIndexBuffer;
		vertexTextureCoordinateIndexDescriptorBuffer.offset = 0;
		vertexTextureCoordinateIndexDescriptorBuffer.size = vertexTextureCoordinateIndexBuffer->size;

		DescriptorBuffer materialCountDescriptorBuffer{};
		materialCountDescriptorBuffer.buffer = materialCountBuffer;
		materialCountDescriptorBuffer.offset = 0;
		materialCountDescriptorBuffer.size = materialCountBuffer->size;

		DescriptorBuffer materialOffsetDescriptorBuffer{};
		materialOffsetDescriptorBuffer.buffer = materialOffsetBuffer;
		materialOffsetDescriptorBuffer.offset = 0;
		materialOffsetDescriptorBuffer.size = materialOffsetBuffer->size;

		DescriptorBuffer materialIndirectDescriptorBuffer{};
		materialIndirectDescriptorBuffer.buffer = materialIndirectBuffer;
		materialIndirectDescriptorBuffer.offset = 0;
		materialIndirectDescriptorBuffer.size = materialIndirectBuffer->size;

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

		WriteDescriptorSet visibilityMaterialCountWriteDescriptorSet{};
		visibilityMaterialCountWriteDescriptorSet.descriptorSet = visibilityMaterialCountDescriptorSet;
		visibilityMaterialCountWriteDescriptorSet.bindingIndex = 0;
		visibilityMaterialCountWriteDescriptorSet.descriptorBuffers = {materialCountDescriptorBuffer};

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

		WriteDescriptorSet materialCountWriteDescriptorSet{};
		materialCountWriteDescriptorSet.descriptorSet = materialOffsetDescriptorSet;
		materialCountWriteDescriptorSet.bindingIndex = 0;
		materialCountWriteDescriptorSet.descriptorBuffers = {materialCountDescriptorBuffer};

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

		WriteDescriptorSet materialBaseClusterWriteDescriptorSet{};
		materialBaseClusterWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseClusterWriteDescriptorSet.bindingIndex = 3;
		materialBaseClusterWriteDescriptorSet.descriptorBuffers = {clusterDescriptorBuffer};

		WriteDescriptorSet materialBaseInstanceWriteDescriptorSet{};
		materialBaseInstanceWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseInstanceWriteDescriptorSet.bindingIndex = 4;
		materialBaseInstanceWriteDescriptorSet.descriptorBuffers = {instanceDescriptorBuffer};

		getDevice()->updateDescriptorSets({
			cameraWriteDescriptorSet,
			vertexPositionComponentWriteDescriptorSet,
			vertexPositionIndexWriteDescriptorSet,

			clusterWriteDescriptorSet,

			instanceWriteDescriptorSet,
			geometryWriteDescriptorSet,

			visibilityMaterialCountWriteDescriptorSet,

			debugCameraWriteDescriptorSet,
			debugVertexPositionComponentWriteDescriptorSet,
			debugVertexNormalComponentWriteDescriptorSet,
			debugVertexTextureCoordinateComponentWriteDescriptorSet,
			debugVertexPositionIndexWriteDescriptorSet,
			debugVertexNormalIndexWriteDescriptorSet,
			debugVertexTextureCoordinateIndexWriteDescriptorSet,

			materialCountWriteDescriptorSet,
			materialOffsetWriteDescriptorSet,
			materialIndirectWriteDescriptorSet,

			materialCoordinateCameraWriteDescriptorSet,
			materialCoordinateClusterWriteDescriptorSet,
			materialCoordinateInstanceWriteDescriptorSet,
			materialCoordinateMaterialOffsetWriteDescriptorSet,

			materialBaseCameraWriteDescriptorSet,
			materialBaseClusterWriteDescriptorSet,
			materialBaseInstanceWriteDescriptorSet,
		});
	}

	void VisibilityRenderer::updateDebugInputAttachmentDescriptorSets() {
		DescriptorImage outputDescriptorImage{};
		outputDescriptorImage.imageView = outputImageView;
		outputDescriptorImage.imageLayout = ImageLayout::GENERAL;
		outputDescriptorImage.sampler = nullptr;

		DescriptorImage visibilityDescriptorImage{};
		visibilityDescriptorImage.imageView = visibilityImageView;
		visibilityDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
		visibilityDescriptorImage.sampler = nullptr;

		DescriptorImage depthStencilDescriptorImage{};
		depthStencilDescriptorImage.imageView = depthStencilImageView;
		depthStencilDescriptorImage.imageLayout = ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		depthStencilDescriptorImage.sampler = nullptr;

		DescriptorBuffer materialCoordinateDescriptorBuffer{};
		materialCoordinateDescriptorBuffer.buffer = materialCoordinateBuffer;
		materialCoordinateDescriptorBuffer.offset = 0;
		materialCoordinateDescriptorBuffer.size = materialCoordinateBuffer->size;

		WriteDescriptorSet materialCoordinateVisibilityWriteDescriptorSet{};
		materialCoordinateVisibilityWriteDescriptorSet.descriptorSet = materialCoordinateDescriptorSet;
		materialCoordinateVisibilityWriteDescriptorSet.bindingIndex = 1;
		materialCoordinateVisibilityWriteDescriptorSet.descriptorImages = {visibilityDescriptorImage};

		WriteDescriptorSet materialCoordinateMaterialCoordinateWriteDescriptorSet{};
		materialCoordinateMaterialCoordinateWriteDescriptorSet.descriptorSet = materialCoordinateDescriptorSet;
		materialCoordinateMaterialCoordinateWriteDescriptorSet.bindingIndex = 5;
		materialCoordinateMaterialCoordinateWriteDescriptorSet.descriptorBuffers = {materialCoordinateDescriptorBuffer};

		WriteDescriptorSet materialBaseMaterialCoordinateWriteDescriptorSet{};
		materialBaseMaterialCoordinateWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseMaterialCoordinateWriteDescriptorSet.bindingIndex = 1;
		materialBaseMaterialCoordinateWriteDescriptorSet.descriptorBuffers = {materialCoordinateDescriptorBuffer};

		WriteDescriptorSet materialBaseOutputWriteDescriptorSet{};
		materialBaseOutputWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseOutputWriteDescriptorSet.bindingIndex = 2;
		materialBaseOutputWriteDescriptorSet.descriptorImages = {outputDescriptorImage};

		WriteDescriptorSet materialBaseVisibilityWriteDescriptorSet{};
		materialBaseVisibilityWriteDescriptorSet.descriptorSet = materialBaseDescriptorSet;
		materialBaseVisibilityWriteDescriptorSet.bindingIndex = 3;
		materialBaseVisibilityWriteDescriptorSet.descriptorImages = {visibilityDescriptorImage};

		WriteDescriptorSet outputWriteDescriptorSet{};
		outputWriteDescriptorSet.descriptorSet = outputDescriptorSet;
		outputWriteDescriptorSet.bindingIndex = 0;
		outputWriteDescriptorSet.descriptorImages = {outputDescriptorImage};

		WriteDescriptorSet debugVisibilityWriteDescriptorSet{};
		debugVisibilityWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVisibilityWriteDescriptorSet.bindingIndex = 1;
		debugVisibilityWriteDescriptorSet.descriptorImages = {visibilityDescriptorImage};

		WriteDescriptorSet debugDepthStencilWriteDescriptorSet{};
		debugDepthStencilWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugDepthStencilWriteDescriptorSet.bindingIndex = 2;
		debugDepthStencilWriteDescriptorSet.descriptorImages = {depthStencilDescriptorImage};

		getDevice()->updateDescriptorSets({
			materialCoordinateVisibilityWriteDescriptorSet,
			materialCoordinateMaterialCoordinateWriteDescriptorSet,

			materialBaseMaterialCoordinateWriteDescriptorSet,
			materialBaseOutputWriteDescriptorSet,
			materialBaseVisibilityWriteDescriptorSet,

			outputWriteDescriptorSet,

			debugVisibilityWriteDescriptorSet,
			debugDepthStencilWriteDescriptorSet,
		});
	}

	bool VisibilityRenderer::onWindowResize(WindowResizeEvent& event) {
		resize(event);

		return true;
	}
}