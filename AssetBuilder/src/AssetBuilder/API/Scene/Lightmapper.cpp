// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Lightmapper.hpp"

namespace Trident {
	struct Rectangle {
		uint32 x;
		uint32 y;
		uint32 width;
		uint32 height;
	};

	struct LightmapNode {
		LightmapNode* children[2]{};
		Rectangle rectangle;
		bool assigned = false;

		~LightmapNode() {
			delete children[0];
			delete children[1];
		}

		bool leaf() const {
			return children[0] == nullptr && children[1] == nullptr;
		}
	};

	LightmapNode* Lightmapper::insertNode(Lightmap& lightmap, LightmapNode* node) {
		if (!node->leaf()) {
			LightmapNode* newNode = insertNode(lightmap, node->children[0]);

			if (newNode != nullptr) {
				return newNode;
			}

			return insertNode(lightmap, node->children[1]);
		}

		if (node->assigned) {
			return nullptr;
		}

		if (
			node->rectangle.width < lightmap.size.width ||
			node->rectangle.height < lightmap.size.height
		) {
			return nullptr;
		}

		if (
			node->rectangle.width == lightmap.size.width &&
			node->rectangle.height == lightmap.size.height
		) {
			return node;
		}

		node->children[0] = new LightmapNode();
		node->children[0]->rectangle = node->rectangle;

		node->children[1] = new LightmapNode();
		node->children[1]->rectangle = node->rectangle;

		const Extent2D remainingSize = {
			node->rectangle.width - lightmap.size.width,
			node->rectangle.height - lightmap.size.height,
		};

		// Split towards the axis with the largest available space.
		if (remainingSize.width > remainingSize.height) {
			node->children[0]->rectangle.width = lightmap.size.width;

			node->children[1]->rectangle.x += lightmap.size.width;
			node->children[1]->rectangle.width -= lightmap.size.width;
		}
		else {
			node->children[0]->rectangle.height = lightmap.size.height;

			node->children[1]->rectangle.y += lightmap.size.height;
			node->children[1]->rectangle.height -= lightmap.size.height;
		}

		return insertNode(lightmap, node->children[0]);
	}

	void Lightmapper::updateAtlasCoordinates(sequence<Lightmap>& lightmaps, view<LightmapIndex> lightmapIndices, StaticScene3D& scene) {
		const svec2<float32> atlasSize = {
			static_cast<float32>(ATLAS_SIZE.width),
			static_cast<float32>(ATLAS_SIZE.height),
		};

		LightmapNode rootNode{};
		rootNode.rectangle = {0, 0, ATLAS_SIZE.width, ATLAS_SIZE.height};

		uint32 unsortedLightmapIndex = 0;
		uint32 triangleOffset = 0;

		while (unsortedLightmapIndex < lightmapIndices.count()) {
			const uint32 sortedLightmapIndex = lightmapIndices[unsortedLightmapIndex].index;

			Lightmap& lightmap = lightmaps[sortedLightmapIndex];

			LightmapNode* node = insertNode(lightmap, &rootNode);

			lightmap.offset.width = node->rectangle.x;
			lightmap.offset.height = node->rectangle.y;

			DEBUG_ASSERT(node != nullptr);

			node->assigned = true;

			const view<Edge3D> edges = lightmap.geometryAsset->getHull3DEdges();
			const view<uint16> faceEdgeIndices = lightmap.geometryAsset->getHull3DFaceEdgeIndices();

			uint16 coordinateIndex = 0;

			while (coordinateIndex < lightmap.edgeIndexRange.count) {
				const uint16 edgeIndex = faceEdgeIndices[lightmap.edgeIndexRange.offset + coordinateIndex];
				const Edge3D edge = edges[edgeIndex];

				svec2<float32>& coordinate = scene.lightmapCoordinates[lightmap.coordinateOffset + edge.originVertexIndex];
				coordinate.x += lightmap.offset.width;
				coordinate.y += lightmap.offset.height;
				coordinate /= atlasSize;

				coordinateIndex += 1;
			}

			const view<uint32> vertexPositionIndices = lightmap.geometryAsset->getVertexPositionIndices();
			const uint32 triangleCount = lightmap.edgeIndexRange.count / 2;

			uint32 vertexPositionIndexOffset = lightmap.edgeIndexRange.offset / 2 * 3;
			uint32 triangleIndex = 0;

			while (triangleIndex < triangleCount) {
				StaticTriangle3D& triangle = scene.triangles[triangleOffset + triangleIndex];
				triangle.vertices[0].lightmapCoordinateIndex = lightmap.coordinateOffset + vertexPositionIndices[vertexPositionIndexOffset + 0];
				triangle.vertices[1].lightmapCoordinateIndex = lightmap.coordinateOffset + vertexPositionIndices[vertexPositionIndexOffset + 1];
				triangle.vertices[2].lightmapCoordinateIndex = lightmap.coordinateOffset + vertexPositionIndices[vertexPositionIndexOffset + 2];

				triangleIndex += 1;
				vertexPositionIndexOffset += 3;
			}

			unsortedLightmapIndex += 1;
			triangleOffset += triangleCount;
		}
	}

	void Lightmapper::buildLightmapAtlas(const StaticScene3D& scene, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) {
		const uint32 indexCount = static_cast<uint32>(scene.triangles.count() * 3);

		renderPass = createRenderPass(lightmapAtlasImageAssetDescriptor.format);

		lightmapAtlasImage = createLightmapAtlasImage(lightmapAtlasImageAssetDescriptor);
		lightmapAtlasImageView = createLightmapAtlasImageView();

		framebuffer = createFramebuffer();

		lightmapAtlasPipelineLayout = createLightmapAtlasPipelineLayout();

		lightmapAtlasPipeline = createLightmapAtlasPipeline();

		coordinateBuffer = createCoordinateBuffer(scene.lightmapCoordinates.count() * sizeof(svec2<float32>));
		coordinateIndexBuffer = createCoordinateIndexBuffer(indexCount * sizeof(uint32));

		copyCoordinateBuffers(scene);

		const Extent2D framebufferExtent = framebuffer->getExtent();
		const float32 framebufferWidth = static_cast<float32>(framebufferExtent.width);
		const float32 framebufferHeight = static_cast<float32>(framebufferExtent.height);

		const ref<Buffer> lightmapAtlasStagingBuffer = Renderer::createDynamicBuffer({
			.size = lightmapAtlasImage->getSize(),
			.usage = BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas staging",
			#endif
		});

		const BufferImageCopyRegion lightmapAtlasBufferImageCopyRegion = {
			.bufferOffset = 0,
			.imageOffsetX = 0,
			.imageOffsetY = 0,
			.imageOffsetZ = 0,
			.imageExtent = lightmapAtlasImage->getExtent(),
			.imageMipLevel = lightmapAtlasImageView->getMipLevelOffset(),
			.imageLayerOffset = lightmapAtlasImageView->getLayerOffset(),
			.imageLayerCount = lightmapAtlasImageView->getLayerCount(),
		};

		const ref<Queue> queue = Renderer::getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Build lightmap atlas",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->beginRenderPass({
			.framebuffer = framebuffer,
			.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}},
		});
		commandEncoder->setViewport({0.0f, framebufferHeight}, {framebufferWidth, -framebufferHeight}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, framebufferExtent);
		commandEncoder->bindGraphicsPipeline(lightmapAtlasPipeline);
		commandEncoder->bindVertexBuffer(0, coordinateBuffer, 0, sizeof(svec2<float32>));
		commandEncoder->bindIndexBuffer<uint32>(coordinateIndexBuffer, 0);
		commandEncoder->drawIndexed(indexCount, 1, 0, 0, 0);
		commandEncoder->endRenderPass();

		// TODO(graphics/vk/copy-lightmap-atlas): Fix image memory barrier causing pixel grid artifacts.
		if (Renderer::getGraphicsApi() != GraphicsAPI::VK) {
			const ImageMemoryBarrier imageMemoryBarrier = {
				.image = lightmapAtlasImage,
				.newLayout = ImageLayout::TRANSFER_SRC_OPTIMAL,
				.newAccess = Access::TRANSFER_READ,
			};

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
		}

		commandEncoder->copyImageToBuffer(lightmapAtlasImage, lightmapAtlasStagingBuffer, lightmapAtlasBufferImageCopyRegion);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		const ref<BufferView<uint8>> lightmapAtlasView = lightmapAtlasStagingBuffer->createView<uint8>(0, lightmapAtlasStagingBuffer->getSize());

		uint8* lightmapAtlasMappedRange = lightmapAtlasView->map();

		std::copy_n(lightmapAtlasMappedRange, lightmapAtlasImageAssetDescriptor.data.count(), &lightmapAtlasImageAssetDescriptor.data[0]);

		lightmapAtlasStagingBuffer->unmap();
	}

	ref<RenderPass> Lightmapper::createRenderPass(ImageFormat lightmapAtlasImageFormat) const {
		return Renderer::createRenderPass({
			.attachments = {
				// Lightmap atlas
				{
					.format = lightmapAtlasImageFormat,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::TRANSFER_SRC_OPTIMAL,
				},
			},
			.subpasses = {
				// Write to lightmap atlas
				{
					.colorAttachmentReferences = {
						{
							.index = 0,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
				},
			},
		});
	}

	ref<Image> Lightmapper::createLightmapAtlasImage(const ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) const {
		const Extent2D extent = Renderer::getSwapchain()->getExtent();

		return renderPass->createInputAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = lightmapAtlasImageAssetDescriptor.extent,
				.mipLevelCount = lightmapAtlasImageAssetDescriptor.mipLevelCount,
				.layerCount = lightmapAtlasImageAssetDescriptor.layerCount,
				.additionalUsage = ImageUsage::TRANSFER_SRC,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Lightmap atlas",
				#endif
			},
			0
		);
	}

	ref<ImageView> Lightmapper::createLightmapAtlasImageView() {
		return lightmapAtlasImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas view",
			#endif
		});
	}

	ref<Framebuffer> Lightmapper::createFramebuffer() const {
		const Extent3D extent = lightmapAtlasImage->getExtent();

		return Renderer::createFramebuffer({
			.renderPass = renderPass,
			.imageViews = {lightmapAtlasImageView},
			.extent = {extent.width, extent.height},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas",
			#endif
		});
	}

	ref<PipelineLayout> Lightmapper::createLightmapAtlasPipelineLayout() const {
		return Renderer::createPipelineLayout({
			.descriptorSetLayouts = {},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas",
			#endif
		});
	}

	ref<GraphicsPipeline> Lightmapper::createLightmapAtlasPipeline() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>();

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		return Renderer::createGraphicsPipeline({
			.pipelineLayout = lightmapAtlasPipelineLayout,
			.vertexInputBindings = {vertexInputBinding},
			.vertexShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Lightmap atlas vertex",
					#endif
				},
				"Lightmapper/LightmapAtlas.vert"
			),
			.fragmentShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Lightmap atlas fragment",
					#endif
				},
				"Lightmapper/LightmapAtlas.frag"
			),
			.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
			.polygonMode = PolygonMode::FILL,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = colorBlendAttachments,
			},
			.depthStencilState = {
				.depthTest = false,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createCoordinateBuffer(uint64 size) const {
		return Renderer::createDynamicBuffer({
			.size = size,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Coordinate",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createCoordinateIndexBuffer(uint64 size) const {
		return Renderer::createDynamicBuffer({
			.size = size,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Coordinate index",
			#endif
		});
	}

	void Lightmapper::copyCoordinateBuffers(const StaticScene3D& scene) const {
		const ref<Buffer> stagingBuffer = Renderer::createDynamicBuffer({
			.size = coordinateBuffer->getSize() + coordinateIndexBuffer->getSize(),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const ref<BufferView<svec2<float32>>> coordinateView = stagingBuffer->createView<svec2<float32>>(0, coordinateBuffer->getSize());
		const ref<BufferView<uint32>> coordinateIndexView = stagingBuffer->createView<uint32>(coordinateBuffer->getSize(), coordinateIndexBuffer->getSize());

		svec2<float32>* coordinateMappedRange = coordinateView->map();

		std::copy(scene.lightmapCoordinates.begin(), scene.lightmapCoordinates.end(), coordinateMappedRange);

		uint32* coordinateIndexMappedRange = coordinateIndexView->map();
		uint32 coordinateIndexIndex = 0;

		for (const StaticTriangle3D& triangle : scene.triangles) {
			coordinateIndexMappedRange[coordinateIndexIndex + 0] = triangle.vertices[0].lightmapCoordinateIndex;
			coordinateIndexMappedRange[coordinateIndexIndex + 1] = triangle.vertices[1].lightmapCoordinateIndex;
			coordinateIndexMappedRange[coordinateIndexIndex + 2] = triangle.vertices[2].lightmapCoordinateIndex;

			coordinateIndexIndex += 3;
		}

		stagingBuffer->unmap();

		const ref<Queue> queue = Renderer::getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy coordinate buffers",
			#endif
		});

		commandEncoder->begin();

		BufferCopyRegion bufferCopyRegion = {
			.sourceOffset = 0,
			.destinationOffset = 0,
			.size = coordinateBuffer->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, coordinateBuffer, bufferCopyRegion);

		bufferCopyRegion = {
			.sourceOffset = coordinateBuffer->getSize(),
			.destinationOffset = 0,
			.size = coordinateIndexBuffer->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, coordinateIndexBuffer, bufferCopyRegion);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();
	}
}