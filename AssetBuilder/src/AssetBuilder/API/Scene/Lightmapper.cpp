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

	// See https://blackpawn.com/texts/lightmaps.
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

	void Lightmapper::updateAtlasCoordinates(StaticScene3D& scene, sequence<Lightmap>& lightmaps, view<LightmapIndex> lightmapIndices) {
		const svec2<float32> atlasSize = {
			static_cast<float32>(ATLAS_SIZE.width),
			static_cast<float32>(ATLAS_SIZE.height),
		};

		LightmapNode rootNode{};
		rootNode.rectangle = {0, 0, ATLAS_SIZE.width, ATLAS_SIZE.height};

		uint32 unsortedLightmapIndex = 0;

		while (unsortedLightmapIndex < lightmapIndices.count()) {
			const uint32 lightmapIndex = lightmapIndices[unsortedLightmapIndex].index;

			Lightmap& lightmap = lightmaps[lightmapIndex];

			LightmapNode* node = insertNode(lightmap, &rootNode);

			DEBUG_ASSERT(node != nullptr);

			node->assigned = true;

			lightmap.offset.width = node->rectangle.x;
			lightmap.offset.height = node->rectangle.y;

			uint16 coordinateIndex = 0;

			while (coordinateIndex < 4) {
				svec2<float32>& coordinate = scene.lightmapCoordinates[lightmap.coordinateOffset + coordinateIndex];
				coordinate.x += lightmap.offset.width;
				coordinate.y += lightmap.offset.height;
				coordinate /= atlasSize;

				coordinateIndex += 1;
			}

			StaticTriangle3D& triangle1 = scene.triangles[lightmap.triangleOffset + 0];
			triangle1.vertices[0].lightmapCoordinateIndex = lightmap.coordinateOffset + 0;
			triangle1.vertices[1].lightmapCoordinateIndex = lightmap.coordinateOffset + 1;
			triangle1.vertices[2].lightmapCoordinateIndex = lightmap.coordinateOffset + 2;

			StaticTriangle3D& triangle2 = scene.triangles[lightmap.triangleOffset + 1];
			triangle2.vertices[0].lightmapCoordinateIndex = lightmap.coordinateOffset + 1;
			triangle2.vertices[1].lightmapCoordinateIndex = lightmap.coordinateOffset + 3;
			triangle2.vertices[2].lightmapCoordinateIndex = lightmap.coordinateOffset + 2;

			unsortedLightmapIndex += 1;
		}
	}

	void Lightmapper::buildLightmapAtlas(const StaticScene3D& scene, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) {
		const uint32 triangleCount = static_cast<uint32>(scene.triangles.count());
		const uint32 vertexCount = triangleCount * 3;
		const ref<Device> device = Renderer::getDevice();

		rasterizeLightmapsRenderPass = createRasterizeLightmapsRenderPass(lightmapAtlasImageAssetDescriptor.format);

		lightmapAtlasImage = createLightmapAtlasImage(lightmapAtlasImageAssetDescriptor);
		lightmapAtlasImageView = createLightmapAtlasImageView();

		rasterizeLightmapsFramebuffer = createRasterizeLightmapsFramebuffer();

		const ref<DescriptorSetLayout> rasterizeLightmapsDescriptorSetLayout = createRasterizeLightmapsDescriptorSetLayout();
		const ref<DescriptorSetLayout> traceRaysDescriptorSetLayout = createTraceRaysDescriptorSetLayout();
		const ref<DescriptorSetLayout> postProcessingDescriptorSetLayout = createPostProcessingDescriptorSetLayout();

		rasterizeLightmapsPipelineLayout = createRasterizeLightmapsPipelineLayout({rasterizeLightmapsDescriptorSetLayout});
		traceRaysPipelineLayout = createTraceRaysPipelineLayout({traceRaysDescriptorSetLayout});
		postProcessingPipelineLayout = createPostProcessingPipelineLayout({postProcessingDescriptorSetLayout});

		rasterizeLightmapsPipeline = createRasterizeLightmapsPipeline();
		traceRaysPipeline = createTraceRaysPipeline();
		postProcessingPipeline = createPostProcessingPipeline();

		const sequence<ref<DescriptorSet>> descriptorSets = device->allocateDescriptorSets(sequence<ref<DescriptorSetLayout>>{
			rasterizeLightmapsDescriptorSetLayout,
			traceRaysDescriptorSetLayout,
			postProcessingDescriptorSetLayout,
		});

		rasterizeLightmapsDescriptorSet = descriptorSets[0];
		traceRaysDescriptorSet = descriptorSets[1];
		postProcessingDescriptorSet = descriptorSets[2];

		lightmapCoordinateBuffer = createLightmapCoordinateBuffer(scene.lightmapCoordinates.count() * sizeof(svec2<float32>));
		lightmapCoordinateIndexBuffer = createLightmapCoordinateIndexBuffer(vertexCount * sizeof(uint32));
		traceRaysArgumentBuffer = createTraceRaysArgumentBuffer();
		lightmapPixelBuffer = createLightmapPixelBuffer();
		triangleBuffer = createTriangleBuffer(scene.triangles.count() * sizeof(StaticTriangle3D));
		vertexComponentBuffer = createVertexComponentBuffer(scene.vertices.count() * sizeof(svec3<float32>));
		normalComponentBuffer = createNormalComponentBuffer(scene.normals.count() * sizeof(svec3<float32>));

		encodeOneTimeCommands(scene);

		const Extent2D framebufferExtent = rasterizeLightmapsFramebuffer->getExtent();
		const float32 framebufferWidth = static_cast<float32>(framebufferExtent.width);
		const float32 framebufferHeight = static_cast<float32>(framebufferExtent.height);

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Generate lightmaps",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->beginRenderPass({
			.framebuffer = rasterizeLightmapsFramebuffer,
			.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}},
		});
		commandEncoder->setViewport({0.0f, framebufferHeight}, {framebufferWidth, -framebufferHeight}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, framebufferExtent);
		commandEncoder->bindGraphicsPipeline(rasterizeLightmapsPipeline);
		commandEncoder->bindVertexBuffer(0, lightmapCoordinateIndexBuffer, 0, sizeof(uint32));
		commandEncoder->bindDescriptorSets(rasterizeLightmapsPipelineLayout, 0, rasterizeLightmapsDescriptorSet);
		commandEncoder->draw(vertexCount, 1, 0, 0);
		commandEncoder->endRenderPass();

		ImageMemoryBarrier imageMemoryBarrier;

		// TODO(graphics/dx12/end-render-pass): Automatically transition the attachments to their final layout.
		if (Renderer::getGraphicsApi() == GraphicsAPI::DX12) {
			imageMemoryBarrier = {
				.image = lightmapAtlasImage,
				.newLayout = ImageLayout::GENERAL,
				.newAccess = Access::SHADER_READ | Access::SHADER_WRITE,
			};

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
		}

		commandEncoder->bindComputePipeline(traceRaysPipeline);
		commandEncoder->bindDescriptorSets(traceRaysPipelineLayout, 0, traceRaysDescriptorSet);

		// TODO(graphics/dx12/dispatch): Fix artifacts on the lightmap atlas due to not enough threads being dispatched.
		commandEncoder->dispatchIndirect(traceRaysArgumentBuffer, offsetof(TraceRaysArguments, TraceRaysArguments::traceRaysCommand));

		imageMemoryBarrier = {
			.image = lightmapAtlasImage,
			.newLayout = ImageLayout::GENERAL,
			.newAccess = Access::SHADER_READ | Access::SHADER_WRITE,
		};

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

		commandEncoder->bindComputePipeline(postProcessingPipeline);
		commandEncoder->bindDescriptorSets(postProcessingPipelineLayout, 0, postProcessingDescriptorSet);
		commandEncoder->dispatch(lightmapAtlasImageAssetDescriptor.extent.width / 16, lightmapAtlasImageAssetDescriptor.extent.height / 16, 1);

		imageMemoryBarrier = {
			.image = lightmapAtlasImage,
			.newLayout = ImageLayout::TRANSFER_SRC_OPTIMAL,
			.newAccess = Access::TRANSFER_READ,
		};

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

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

		commandEncoder->copyImageToBuffer(lightmapAtlasImage, lightmapAtlasStagingBuffer, lightmapAtlasBufferImageCopyRegion);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		const ref<BufferView<uint8>> lightmapAtlasView = lightmapAtlasStagingBuffer->createView<uint8>(0, lightmapAtlasStagingBuffer->getSize());

		std::copy_n(lightmapAtlasView->map(), lightmapAtlasImageAssetDescriptor.data.count(), &lightmapAtlasImageAssetDescriptor.data[0]);

		lightmapAtlasStagingBuffer->unmap();
	}

	ref<RenderPass> Lightmapper::createRasterizeLightmapsRenderPass(ImageFormat lightmapAtlasImageFormat) const {
		return Renderer::createRenderPass({
			.attachments = {
				// Lightmap atlas
				{
					.format = lightmapAtlasImageFormat,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::GENERAL,
				},
			},
			.subpasses = {
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

		return rasterizeLightmapsRenderPass->createInputAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = lightmapAtlasImageAssetDescriptor.extent,
				.mipLevelCount = lightmapAtlasImageAssetDescriptor.mipLevelCount,
				.layerCount = lightmapAtlasImageAssetDescriptor.layerCount,
				.additionalUsage = ImageUsage::TRANSFER_SRC | ImageUsage::STORAGE,
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

	ref<Framebuffer> Lightmapper::createRasterizeLightmapsFramebuffer() const {
		const Extent3D extent = lightmapAtlasImage->getExtent();

		return Renderer::createFramebuffer({
			.renderPass = rasterizeLightmapsRenderPass,
			.imageViews = {lightmapAtlasImageView},
			.extent = {extent.width, extent.height},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Rasterize lightmaps",
			#endif
		});
	}

	ref<DescriptorSetLayout> Lightmapper::createRasterizeLightmapsDescriptorSetLayout() const {
		return Renderer::createDescriptorSetLayout({
			// Lightmap coordinate
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX,
			},

			// Trace rays argument
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},

			// Lightmap pixel
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},
		});
	}

	ref<DescriptorSetLayout> Lightmapper::createTraceRaysDescriptorSetLayout() const {
		return Renderer::createDescriptorSetLayout({
			// Trace rays argument
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Lightmap pixel
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Triangle
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Vertex component
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Normal component
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Lightmap coordinate
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Lightmap atlas
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},
		});
	}

	ref<DescriptorSetLayout> Lightmapper::createPostProcessingDescriptorSetLayout() const {
		return Renderer::createDescriptorSetLayout({
			// Lightmap atlas
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},
		});
	}

	ref<PipelineLayout> Lightmapper::createRasterizeLightmapsPipelineLayout(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) const {
		return Renderer::createPipelineLayout({
			.descriptorSetLayouts = descriptorSetLayouts,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Rasterize lightmaps",
			#endif
		});
	}

	ref<PipelineLayout> Lightmapper::createTraceRaysPipelineLayout(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) const {
		return Renderer::createPipelineLayout({
			.descriptorSetLayouts = descriptorSetLayouts,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Trace rays",
			#endif
		});
	}

	ref<PipelineLayout> Lightmapper::createPostProcessingPipelineLayout(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) const {
		return Renderer::createPipelineLayout({
			.descriptorSetLayouts = descriptorSetLayouts,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Post processing",
			#endif
		});
	}

	ref<GraphicsPipeline> Lightmapper::createRasterizeLightmapsPipeline() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<uint32>();

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		return Renderer::createGraphicsPipeline({
			.pipelineLayout = rasterizeLightmapsPipelineLayout,
			.vertexInputBindings = {vertexInputBinding},
			.vertexShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Rasterize lightmaps (vertex)",
					#endif
				},
				"Lightmapper/RasterizeLightmaps.vert"
			),
			.fragmentShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Rasterize lightmaps (fragment)",
					#endif
				},
				"Lightmapper/RasterizeLightmaps.frag"
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
			.renderPass = rasterizeLightmapsRenderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Rasterize lightmaps",
			#endif
		});
	}

	ref<ComputePipeline> Lightmapper::createTraceRaysPipeline() const {
		return Renderer::createComputePipeline({
			.pipelineLayout = traceRaysPipelineLayout,
			.computeShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Trace rays (compute)",
					#endif
				},
				"Lightmapper/TraceRays.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Trace rays",
			#endif
		});
	}

	ref<ComputePipeline> Lightmapper::createPostProcessingPipeline() const {
		return Renderer::createComputePipeline({
			.pipelineLayout = postProcessingPipelineLayout,
			.computeShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Post processing (compute)",
					#endif
				},
				"Lightmapper/PostProcessing.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Post processing",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createLightmapCoordinateBuffer(uint64 size) const {
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(size, minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap coordinate",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createLightmapCoordinateIndexBuffer(uint64 size) const {
		return Renderer::createDynamicBuffer({
			.size = size,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap coordinate index",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createTraceRaysArgumentBuffer() const {
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(sizeof(TraceRaysArguments), minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_WRITE_STORAGE | BufferUsage::INDIRECT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Trace rays argument",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createLightmapPixelBuffer() const {
		const Extent3D lightmapAtlasImageExtent = lightmapAtlasImage->getExtent();
		const uint64 size = lightmapAtlasImageExtent.width * lightmapAtlasImageExtent.height * sizeof(LightmapPixel);
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(size, minStorageBufferOffsetAlignment),
			.usage = BufferUsage::READ_WRITE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap pixel",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createTriangleBuffer(uint64 size) const {
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(size, minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Triangle",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createVertexComponentBuffer(uint64 size) const {
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(size, minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex component",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createNormalComponentBuffer(uint64 size) const {
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(size, minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Normal component",
			#endif
		});
	}

	void Lightmapper::encodeOneTimeCommands(const StaticScene3D& scene) const {
		const ref<Device> device = Renderer::getDevice();
		const ref<Buffer> stagingBuffer = Renderer::createDynamicBuffer({
			.size = lightmapCoordinateBuffer->getSize() + lightmapCoordinateIndexBuffer->getSize() + traceRaysArgumentBuffer->getSize() + triangleBuffer->getSize() + vertexComponentBuffer->getSize() + normalComponentBuffer->getSize(),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const ref<BufferView<svec2<float32>>> lightmapCoordinateStagingView = stagingBuffer->createView<svec2<float32>>(0, lightmapCoordinateBuffer->getSize());
		const ref<BufferView<uint32>> lightmapCoordinateIndexStagingView = stagingBuffer->createView<uint32>(lightmapCoordinateStagingView->getOffset() + lightmapCoordinateStagingView->getSize(), lightmapCoordinateIndexBuffer->getSize());
		const ref<BufferView<TraceRaysArguments>> traceRaysArgumentStagingView = stagingBuffer->createView<TraceRaysArguments>(lightmapCoordinateIndexStagingView->getOffset() + lightmapCoordinateIndexStagingView->getSize(), traceRaysArgumentBuffer->getSize());
		const ref<BufferView<StaticTriangle3D>> triangleStagingView = stagingBuffer->createView<StaticTriangle3D>(traceRaysArgumentStagingView->getOffset() + traceRaysArgumentStagingView->getSize(), triangleBuffer->getSize());
		const ref<BufferView<svec3<float32>>> vertexComponentStagingView = stagingBuffer->createView<svec3<float32>>(triangleStagingView->getOffset() + triangleStagingView->getSize(), vertexComponentBuffer->getSize());
		const ref<BufferView<svec3<float32>>> normalComponentStagingView = stagingBuffer->createView<svec3<float32>>(vertexComponentStagingView->getOffset() + vertexComponentStagingView->getSize(), normalComponentBuffer->getSize());

		std::copy(scene.lightmapCoordinates.begin(), scene.lightmapCoordinates.end(), lightmapCoordinateStagingView->map());

		uint32* lightmapCoordinateIndexMappedRange = lightmapCoordinateIndexStagingView->map();
		uint32 coordinateIndexIndex = 0;

		for (const StaticTriangle3D& triangle : scene.triangles) {
			lightmapCoordinateIndexMappedRange[coordinateIndexIndex + 0] = triangle.vertices[0].lightmapCoordinateIndex;
			lightmapCoordinateIndexMappedRange[coordinateIndexIndex + 1] = triangle.vertices[1].lightmapCoordinateIndex;
			lightmapCoordinateIndexMappedRange[coordinateIndexIndex + 2] = triangle.vertices[2].lightmapCoordinateIndex;

			coordinateIndexIndex += 3;
		}

		TraceRaysArguments* traceRaysArguments = traceRaysArgumentStagingView->map();
		traceRaysArguments->atlasSize = ATLAS_SIZE;
		traceRaysArguments->lightmapPixelCount = 0;
		traceRaysArguments->triangleCount = static_cast<uint32>(scene.triangles.count());
		traceRaysArguments->traceRaysCommand.x = 1;
		traceRaysArguments->traceRaysCommand.y = 1;
		traceRaysArguments->traceRaysCommand.z = 1;

		std::copy(scene.triangles.begin(), scene.triangles.end(), triangleStagingView->map());
		std::copy(scene.vertices.begin(), scene.vertices.end(), vertexComponentStagingView->map());
		std::copy(scene.normals.begin(), scene.normals.end(), normalComponentStagingView->map());

		stagingBuffer->unmap();

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy buffers",
			#endif
		});

		commandEncoder->begin();

		BufferCopyRegion bufferCopyRegion = {
			.sourceOffset = lightmapCoordinateStagingView->getOffset(),
			.destinationOffset = 0,
			.size = lightmapCoordinateStagingView->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, lightmapCoordinateBuffer, bufferCopyRegion);

		bufferCopyRegion = {
			.sourceOffset = lightmapCoordinateIndexStagingView->getOffset(),
			.destinationOffset = 0,
			.size = lightmapCoordinateIndexStagingView->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, lightmapCoordinateIndexBuffer, bufferCopyRegion);

		bufferCopyRegion = {
			.sourceOffset = traceRaysArgumentStagingView->getOffset(),
			.destinationOffset = 0,
			.size = traceRaysArgumentStagingView->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, traceRaysArgumentBuffer, bufferCopyRegion);

		bufferCopyRegion = {
			.sourceOffset = triangleStagingView->getOffset(),
			.destinationOffset = 0,
			.size = triangleStagingView->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, triangleBuffer, bufferCopyRegion);

		bufferCopyRegion = {
			.sourceOffset = vertexComponentStagingView->getOffset(),
			.destinationOffset = 0,
			.size = vertexComponentStagingView->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexComponentBuffer, bufferCopyRegion);

		bufferCopyRegion = {
			.sourceOffset = normalComponentStagingView->getOffset(),
			.destinationOffset = 0,
			.size = normalComponentStagingView->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, normalComponentBuffer, bufferCopyRegion);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		const ref<BufferView<svec2<float32>>> lightmapCoordinateView = lightmapCoordinateBuffer->createView<svec2<float32>>(0, lightmapCoordinateBuffer->getSize());
		const ref<BufferView<TraceRaysArguments>> traceRaysArgumentView = traceRaysArgumentBuffer->createView<TraceRaysArguments>(0, traceRaysArgumentBuffer->getSize());
		const ref<BufferView<LightmapPixel>> lightmapPixelView = lightmapPixelBuffer->createView<LightmapPixel>(0, lightmapPixelBuffer->getSize());
		const ref<BufferView<StaticTriangle3D>> triangleView = triangleBuffer->createView<StaticTriangle3D>(0, triangleBuffer->getSize());
		const ref<BufferView<float32>> vertexComponentView = vertexComponentBuffer->createView<float32>(0, vertexComponentBuffer->getSize());
		const ref<BufferView<float32>> normalComponentView = normalComponentBuffer->createView<float32>(0, normalComponentBuffer->getSize());

		const DescriptorImage lightmapAtlasDescriptorImage = {
			.imageView = lightmapAtlasImageView,
			.imageLayout = ImageLayout::GENERAL,
			.sampler = nullptr,
		};

		const sequence<WriteDescriptorSet> writeDescriptorSets = {
			{
				.descriptorSet = rasterizeLightmapsDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = lightmapCoordinateView,
			},
			{
				.descriptorSet = rasterizeLightmapsDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = traceRaysArgumentView,
			},
			{
				.descriptorSet = rasterizeLightmapsDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = lightmapPixelView,
			},
			{
				.descriptorSet = traceRaysDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = traceRaysArgumentView,
			},
			{
				.descriptorSet = traceRaysDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = lightmapPixelView,
			},
			{
				.descriptorSet = traceRaysDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = triangleView,
			},
			{
				.descriptorSet = traceRaysDescriptorSet,
				.bindingIndex = 3,
				.descriptorBufferViews = vertexComponentView,
			},
			{
				.descriptorSet = traceRaysDescriptorSet,
				.bindingIndex = 4,
				.descriptorBufferViews = normalComponentView,
			},
			{
				.descriptorSet = traceRaysDescriptorSet,
				.bindingIndex = 5,
				.descriptorBufferViews = lightmapCoordinateView,
			},
			{
				.descriptorSet = traceRaysDescriptorSet,
				.bindingIndex = 6,
				.descriptorImages = lightmapAtlasDescriptorImage,
			},
			{
				.descriptorSet = postProcessingDescriptorSet,
				.bindingIndex = 0,
				.descriptorImages = lightmapAtlasDescriptorImage,
			},
		};

		device->updateDescriptorSets(writeDescriptorSets);
	}
}