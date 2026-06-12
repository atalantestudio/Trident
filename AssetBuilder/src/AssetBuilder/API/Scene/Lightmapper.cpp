// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/GUI/LayoutDirection.hpp"

#include "AssetBuilder/API/Scene/Lightmapper.hpp"

namespace Trident {
	struct LightmapNode {
		LightmapNode* children[2]{};

		/// Direction in which to split the node.
		LayoutDirection splitDirection;

		/// Size along the split direction.
		uint32 size;

		uint32 lightmapIndex = max<uint32>();

		~LightmapNode() {
			delete children[0];
			delete children[1];
		}

		bool leaf() const {
			return children[0] == nullptr && children[1] == nullptr;
		}

		bool canContain(Extent2D extent) const {
			return size >= extent[splitDirection];
		}

		Extent2D getExtent() const {
			Extent2D extent = Lightmapper::ATLAS_SIZE;
			extent[splitDirection] = size;

			return extent;
		}
	};

	LightmapNode* Lightmapper::insertNode(Lightmap& lightmap, LightmapNode* node) {
		if (!node->leaf()) {
			LightmapNode* newNode = insertNode(lightmap, node->children[0]);

			if (newNode != nullptr) {
				return newNode;
			}

			lightmap.offset[node->children[0]->splitDirection] += node->children[0]->size;

			return insertNode(lightmap, node->children[1]);
		}

		if (node->lightmapIndex != max<uint32>()) {
			return nullptr;
		}

		const Extent2D nodeSize = node->getExtent();

		if (!node->canContain(lightmap.size)) {
			return nullptr;
		}

		if (node->size == lightmap.size[node->splitDirection]) {
			return node;
		}

		node->children[0] = new LightmapNode();
		node->children[1] = new LightmapNode();

		const Extent2D remainingSize = {
			nodeSize.width - lightmap.size.width,
			nodeSize.height - lightmap.size.height,
		};

		if (remainingSize.width > remainingSize.height) {
			// TODO(lightmapper/packing/node-split-direction)
			// Here the parent is split horizontally so its split direction is HORIZONTAL.
			// Those of the children are not known yet so they shouldn't be set.
			// node->splitDirection = LayoutDirection::HORIZONTAL;
			node->children[0]->splitDirection = LayoutDirection::HORIZONTAL;
			node->children[1]->splitDirection = LayoutDirection::HORIZONTAL;

			node->children[0]->size = lightmap.size.width;
			node->children[1]->size = ATLAS_SIZE[node->children[1]->splitDirection] - lightmap.size.width;
		}
		else {
			node->children[0]->splitDirection = LayoutDirection::VERTICAL;
			node->children[1]->splitDirection = LayoutDirection::VERTICAL;

			node->children[0]->size = lightmap.size.height;
			node->children[1]->size = ATLAS_SIZE[node->children[1]->splitDirection] - lightmap.size.height;
		}

		return insertNode(lightmap, node->children[0]);
	}

	void Lightmapper::updateAtlasCoordinates(std::vector<Lightmap>& lightmaps, StaticScene3D& scene) {
		const svec2<float32> atlasSize = {static_cast<float32>(ATLAS_SIZE.width), static_cast<float32>(ATLAS_SIZE.height)};

		LightmapNode rootNode{};
		rootNode.splitDirection = LayoutDirection::HORIZONTAL;
		rootNode.size = ATLAS_SIZE[rootNode.splitDirection];

		uint32 lightmapCoordinateOffset = 0;
		uint32 lightmapIndex = 0;
		uint32 triangleOffset = 0;

		while (lightmapIndex < lightmaps.size()) {
			Lightmap& lightmap = lightmaps[lightmapIndex];

			LightmapNode* node = insertNode(lightmap, &rootNode);
			node->lightmapIndex = lightmapIndex;

			const view<Edge3D> edges = lightmap.geometryAsset->getHull3DEdges();
			const view<uint16> faceEdgeIndices = lightmap.geometryAsset->getHull3DFaceEdgeIndices();

			uint16 coordinateIndex = 0;

			while (coordinateIndex < lightmap.edgeIndexRange.count) {
				const uint16 edgeIndex = faceEdgeIndices[lightmap.edgeIndexRange.offset + coordinateIndex];
				const Edge3D edge = edges[edgeIndex];

				svec2<float32>& coordinate = scene.lightmapCoordinates[lightmapCoordinateOffset + edge.originVertexIndex];
				coordinate.x += lightmap.offset.width;
				coordinate.y += lightmap.offset.height;
				coordinate /= atlasSize;

				coordinateIndex += 1;
			}

			const uint32 triangleCount = lightmap.edgeIndexRange.count / 2;

			uint32 triangleIndex = 0;

			while (triangleIndex < triangleCount) {
				StaticTriangle3D& triangle = scene.triangles[triangleOffset + triangleIndex];
				triangle.vertices[0].lightmapCoordinateIndex = triangle.vertices[0].textureCoordinateIndex;
				triangle.vertices[1].lightmapCoordinateIndex = triangle.vertices[1].textureCoordinateIndex;
				triangle.vertices[2].lightmapCoordinateIndex = triangle.vertices[2].textureCoordinateIndex;

				triangleIndex += 1;
			}

			lightmapCoordinateOffset += lightmap.edgeIndexRange.count;
			lightmapIndex += 1;
			triangleOffset += triangleCount;
		}
	}

	void Lightmapper::buildLightmapAtlas(const StaticScene3D& staticScene, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) {
		const uint32 indexCount = static_cast<uint32>(staticScene.triangles.count() * 3);

		renderPass = createRenderPass(lightmapAtlasImageAssetDescriptor.format);

		lightmapAtlasImage = createLightmapAtlasImage(lightmapAtlasImageAssetDescriptor);
		lightmapAtlasImageView = createLightmapAtlasImageView();

		framebuffer = createFramebuffer();

		lightmapAtlasPipelineLayout = createLightmapAtlasPipelineLayout();

		lightmapAtlasPipeline = createLightmapAtlasPipeline();

		coordinateBuffer = createCoordinateBuffer(staticScene.lightmapCoordinates.count() * sizeof(svec2<float32>));
		coordinateIndexBuffer = createCoordinateIndexBuffer(indexCount * sizeof(uint32));

		copyCoordinateBuffers(staticScene);

		const Extent2D framebufferExtent = framebuffer->getExtent();
		const float32 framebufferWidth = static_cast<float32>(framebufferExtent.width);
		const float32 framebufferHeight = static_cast<float32>(framebufferExtent.height);

		const ref<Buffer<uint8>> lightmapAtlasStagingBuffer = Renderer::createBuffer<uint8>({
			.size = lightmapAtlasImage->getSize(),
			.usage = BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
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

		commandEncoder->copyImageToBuffer(lightmapAtlasImage, lightmapAtlasStagingBuffer, lightmapAtlasBufferImageCopyRegion);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		uint8* lightmapAtlasMappedRange = lightmapAtlasStagingBuffer->map(0, lightmapAtlasStagingBuffer->getSize());

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
				Renderer::createSubpass({
					.colorAttachmentReferences = {
						{
							.index = 0,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
				}),
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

	ref<Buffer<svec2<float32>>> Lightmapper::createCoordinateBuffer(uint64 size) const {
		return Renderer::createBuffer<svec2<float32>>({
			.size = size,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Coordinate",
			#endif
		});
	}

	ref<Buffer<uint32>> Lightmapper::createCoordinateIndexBuffer(uint64 size) const {
		return Renderer::createBuffer<uint32>({
			.size = size,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Coordinate index",
			#endif
		});
	}

	void Lightmapper::copyCoordinateBuffers(const StaticScene3D& staticScene) const {
		const ref<Buffer<uint8>> stagingBuffer = Renderer::createStagingBuffer<uint8>(coordinateBuffer->getSize() + coordinateIndexBuffer->getSize());

		uint8* mappedRange = stagingBuffer->map(0, stagingBuffer->getSize());
		svec2<float32>* coordinateMappedRange = pointer_cast<svec2<float32>*>(mappedRange);

		std::copy(staticScene.lightmapCoordinates.begin(), staticScene.lightmapCoordinates.end(), coordinateMappedRange);

		uint32* coordinateIndexMappedRange = pointer_cast<uint32*>(mappedRange + coordinateBuffer->getSize());
		uint32 coordinateIndexIndex = 0;

		for (const StaticTriangle3D& triangle : staticScene.triangles) {
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