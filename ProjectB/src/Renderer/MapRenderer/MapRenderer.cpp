// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	void MapRenderer::build() {
		renderPass = createRenderPass();
		staticGeometryDescriptorSetLayout = createStaticGeometryDescriptorSetLayout();

		const ref<DescriptorSetLayout> descriptorSetLayouts[] = {
			staticGeometryDescriptorSetLayout,
			staticGeometryDescriptorSetLayout,
			staticGeometryDescriptorSetLayout,
		};

		staticGeometryDescriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayouts);
		staticGeometryPipelineLayout = createStaticGeometryPipelineLayout();
		staticGeometryPipeline = createStaticGeometryPipeline();

		createCameraBuffers();

		depthStencilImage = createDepthStencilImage();
		depthStencilImageView = createDepthStencilImageView();

		createFramebuffers();
	}

	StaticMemoryDescriptor MapRenderer::getStaticMemoryDescriptor() const {
		return {
			.buffers = cameraBuffers,
			.images = {},
		};
	}

	void MapRenderer::postBuild() {
		uint32 cameraBufferIndex = 0;

		while (cameraBufferIndex < cameraBuffers.count()) {
			cameraMappedRanges[cameraBufferIndex] = cameraViews[cameraBufferIndex].map();

			cameraBufferIndex += 1;
		}

		updateDescriptorSets();
	}

	void MapRenderer::encodeMapOneTimeCommands() const {
		const uint64 vertexSize = map.staticGeometry.vertices.count() * sizeof(Vertex3D);
		const uint64 indexSize = map.staticGeometry.indices.count() * sizeof(uint32);

		const ref<Buffer> mapStagingBuffer = createDynamicBuffer({
			.size = vertexSize + indexSize,
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Map staging",
			#endif
		});
		const BufferView<Vertex3D> vertexStagingView = mapStagingBuffer->createView<Vertex3D>(0, vertexSize);
		const BufferView<uint32> indexStagingView = mapStagingBuffer->createView<uint32>(vertexSize, indexSize);

		std::copy(map.staticGeometry.vertices.begin(), map.staticGeometry.vertices.end(), vertexStagingView.map());
		std::copy(map.staticGeometry.indices.begin(), map.staticGeometry.indices.end(), indexStagingView.map());

		const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Map one-time commands",
			#endif
		});

		commandEncoder->begin();

		const BufferCopyRegion bufferCopyRegions[] = {
			{
				.sourceOffset = 0,
				.destinationOffset = 0,
				.size = vertexSize,
			},
			{
				.sourceOffset = vertexSize,
				.destinationOffset = 0,
				.size = indexSize,
			},
		};

		commandEncoder->copyBufferToBuffer(mapStagingBuffer, vertexBuffer, bufferCopyRegions[0]);
		commandEncoder->copyBufferToBuffer(mapStagingBuffer, indexBuffer, bufferCopyRegions[1]);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();
	}

	void MapRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const uint32 imageIndex = getSwapchainImageIndex();
		const ref<Framebuffer> framebuffer = framebuffers[imageIndex];
		const vec2<float32> size = vec2<float32>(framebuffer->getExtent());

		const sequence<ClearValue> clearValues = {
			{0.0f, 0.0f, 0.0f, 1.0f},
			{
				.depth = 0.0f,
				.stencil = 0,
			},
		};

		commandEncoder->beginRenderPass({
			.framebuffer = framebuffer,
			.clearValues = clearValues,
		});

		if (loadedMap) {
			commandEncoder->bindGraphicsPipeline(staticGeometryPipeline);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
			commandEncoder->bindIndexBuffer<IndexFormat::R32_UINT>(indexBuffer, 0);
			commandEncoder->bindDescriptorSets(staticGeometryPipelineLayout, 0, staticGeometryDescriptorSets[imageIndex]);
			commandEncoder->setViewport({0.0f, size.y}, {size.x, -size.y}, 0.0f, 1.0f);
			commandEncoder->setScissor(0, 0, framebuffer->getExtent());
			commandEncoder->drawIndexed(static_cast<uint32>(map.staticGeometry.indices.count()), 1, 0, 0, 0);
		}

		commandEncoder->endRenderPass();
	}

	void MapRenderer::resize(WindowResizeEvent& event) {
		depthStencilImage = createDepthStencilImage();
		depthStencilImageView = createDepthStencilImageView();

		createFramebuffers();
	}

	void MapRenderer::updateCamera(const PerspectiveCamera3D& camera) const {
		const uint32 imageIndex = getSwapchainImageIndex();

		cameraMappedRanges[imageIndex]->projectionView = camera.getProjectionView();
		cameraMappedRanges[imageIndex]->position = camera.position;
	}

	void MapRenderer::loadMap(const Map& map) {
		this->map = map;
		loadedMap = true;

		vertexBuffer = createVertexBuffer();
		indexBuffer = createIndexBuffer();

		encodeMapOneTimeCommands();
	}
}