// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	void Lightmapper::initialize() {
		const ref<Device> device = Renderer::getDevice();

		gBufferRenderPass = createGBufferRenderPass();
		dilationRenderPass = createDilationRenderPass();

		gBufferDescriptorSetLayout = createGBufferDescriptorSetLayout();
		traceRaysDescriptorSetLayout = createTraceRaysDescriptorSetLayout();
		postProcessingDescriptorSetLayout = createPostProcessingDescriptorSetLayout();
		dilationDescriptorSetLayout = createDilationDescriptorSetLayout();

		gBufferPipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {gBufferDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer",
			#endif
		});
		traceRaysPipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {traceRaysDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Trace rays",
			#endif
		});
		postProcessingPipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {postProcessingDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Post processing",
			#endif
		});
		dilationPipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {dilationDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Dilation",
			#endif
		});

		gBufferPipeline = createGBufferPipeline();
		traceRaysPipeline = createTraceRaysPipeline();
		intermediateDilationPipeline = createIntermediateDilationPipeline();
		horizontalBilateralFilterPipeline = createHorizontalBilateralFilterPipeline();
		verticalBilateralFilterPipeline = createVerticalBilateralFilterPipeline();
		dilationPipeline = createDilationPipeline();

		descriptorSets = device->allocateDescriptorSets(sequence<ref<DescriptorSetLayout>>{
			gBufferDescriptorSetLayout,
			traceRaysDescriptorSetLayout,
			postProcessingDescriptorSetLayout,
			dilationDescriptorSetLayout,
		});

		intermediateLightmapAtlasSampler = createIntermediateLightmapAtlasSampler();

		traceRaysArgumentBuffer = createTraceRaysArgumentBuffer();

		initialized = true;
	}

	void Lightmapper::buildLightmapAtlas(view<GeometryIndex3D> vertices, view<svec3<float32>> positions, view<svec3<float32>> normals, view<svec2<float32>> lightmapCoordinates, const LightmapperArguments& lightmapperArguments, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) {
		imageAssetDescriptor = lightmapAtlasImageAssetDescriptor;
		arguments = lightmapperArguments;

		if (!initialized) {
			initialize();
		}

		context = {};

		context.gBufferPositionImage = createGBufferPositionImage();
		context.gBufferTriangleNormalImage = createGBufferTriangleNormalImage();
		context.gBufferVertexNormalImage = createGBufferVertexNormalImage();
		context.intermediateLightmapAtlasImage = createIntermediateLightmapAtlasImage();
		context.lightmapAtlasImage = createLightmapAtlasImage();

		context.gBufferPositionImageView = createGBufferPositionImageView();
		context.gBufferTriangleNormalImageView = createGBufferTriangleNormalImageView();
		context.gBufferVertexNormalImageView = createGBufferVertexNormalImageView();
		context.intermediateLightmapAtlasImageView = createIntermediateLightmapAtlasImageView();
		context.lightmapAtlasImageView = createLightmapAtlasImageView();

		context.gBufferFramebuffer = createGBufferFramebuffer();
		context.dilationFramebuffer = createDilationFramebuffer();

		context.vertexBuffer = createVertexBuffer(vertices.count());
		context.positionBuffer = createPositionBuffer(positions.count());
		context.normalBuffer = createNormalBuffer(normals.count());

		encodeOneTimeCommands(vertices, positions, normals, lightmapCoordinates);

		context.lightmapAtlasStagingBuffer = Renderer::createDynamicBuffer({
			.size = context.lightmapAtlasImage->getSize(),
			.usage = BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas staging",
			#endif
		});

		generateLightmaps(static_cast<uint32>(vertices.count()));

		const ref<BufferView<uint8>> lightmapAtlasView = context.lightmapAtlasStagingBuffer->createView<uint8>(0, context.lightmapAtlasStagingBuffer->getSize());

		std::copy_n(lightmapAtlasView->map(), lightmapAtlasImageAssetDescriptor.data.count(), &lightmapAtlasImageAssetDescriptor.data[0]);

		context.lightmapAtlasStagingBuffer->unmap();
	}

	ref<RenderPass> Lightmapper::createGBufferRenderPass() const {
		return Renderer::createRenderPass({
			.attachments = {
				// G-Buffer position
				{
					.format = ImageFormat::R32G32B32A32_SFLOAT,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::GENERAL,
				},

				// G-Buffer triangle normal
				{
					.format = ImageFormat::R32G32B32A32_SFLOAT,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::GENERAL,
				},

				// G-Buffer vertex normal
				{
					.format = ImageFormat::R32G32B32A32_SFLOAT,
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
						{
							.index = 1,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
						{
							.index = 2,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
				},
			},
		});
	}

	ref<RenderPass> Lightmapper::createDilationRenderPass() const {
		return Renderer::createRenderPass({
			.attachments = {
				// Lightmap atlas
				{
					.format = imageAssetDescriptor.format,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::TRANSFER_SRC_OPTIMAL,
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

	ref<Image> Lightmapper::createGBufferPositionImage() const {
		return gBufferRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = imageAssetDescriptor.extent,
				.mipLevelCount = 1,
				.layerCount = 1,
				.additionalUsage = ImageUsage::STORAGE,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "G-Buffer position",
				#endif
			},
			0
		);
	}

	ref<Image> Lightmapper::createGBufferTriangleNormalImage() const {
		return gBufferRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = imageAssetDescriptor.extent,
				.mipLevelCount = 1,
				.layerCount = 1,
				.additionalUsage = ImageUsage::STORAGE,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "G-Buffer triangle normal",
				#endif
			},
			1
		);
	}

	ref<Image> Lightmapper::createGBufferVertexNormalImage() const {
		return gBufferRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = imageAssetDescriptor.extent,
				.mipLevelCount = 1,
				.layerCount = 1,
				.additionalUsage = ImageUsage::STORAGE,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "G-Buffer vertex normal",
				#endif
			},
			2
		);
	}

	ref<Image> Lightmapper::createIntermediateLightmapAtlasImage() const {
		return Renderer::createDynamicImage({
			.type = ImageType::TEXTURE_2D,
			.format = imageAssetDescriptor.format,
			.extent = imageAssetDescriptor.extent,
			.mipLevelCount = imageAssetDescriptor.mipLevelCount,
			.layerCount = imageAssetDescriptor.layerCount,
			.sampleCount = 1,
			.usage = ImageUsage::SAMPLED | ImageUsage::STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Intermediate lightmap atlas",
			#endif
		});
	}

	ref<Image> Lightmapper::createLightmapAtlasImage() const {
		return dilationRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = imageAssetDescriptor.extent,
				.mipLevelCount = imageAssetDescriptor.mipLevelCount,
				.layerCount = imageAssetDescriptor.layerCount,
				.additionalUsage = ImageUsage::TRANSFER_SRC,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Lightmap atlas",
				#endif
			},
			0
		);
	}

	ref<ImageView> Lightmapper::createGBufferPositionImageView() {
		return context.gBufferPositionImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer position",
			#endif
		});
	}

	ref<ImageView> Lightmapper::createGBufferTriangleNormalImageView() {
		return context.gBufferTriangleNormalImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer triangle normal",
			#endif
		});
	}

	ref<ImageView> Lightmapper::createGBufferVertexNormalImageView() {
		return context.gBufferVertexNormalImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer vertex normal",
			#endif
		});
	}

	ref<ImageView> Lightmapper::createIntermediateLightmapAtlasImageView() {
		return context.intermediateLightmapAtlasImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = imageAssetDescriptor.mipLevelCount,
			.layerOffset = 0,
			.layerCount = imageAssetDescriptor.layerCount,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Intermediate lightmap atlas view",
			#endif
		});
	}

	ref<ImageView> Lightmapper::createLightmapAtlasImageView() {
		return context.lightmapAtlasImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = imageAssetDescriptor.mipLevelCount,
			.layerOffset = 0,
			.layerCount = imageAssetDescriptor.layerCount,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas view",
			#endif
		});
	}

	ref<Framebuffer> Lightmapper::createGBufferFramebuffer() const {
		return Renderer::createFramebuffer({
			.renderPass = gBufferRenderPass,
			.imageViews = {
				context.gBufferPositionImageView,
				context.gBufferTriangleNormalImageView,
				context.gBufferVertexNormalImageView,
			},
			.extent = {imageAssetDescriptor.extent.width, imageAssetDescriptor.extent.height},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer",
			#endif
		});
	}

	ref<Framebuffer> Lightmapper::createDilationFramebuffer() const {
		return Renderer::createFramebuffer({
			.renderPass = dilationRenderPass,
			.imageViews = {context.lightmapAtlasImageView},
			.extent = {imageAssetDescriptor.extent.width, imageAssetDescriptor.extent.height},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Dilation",
			#endif
		});
	}

	ref<DescriptorSetLayout> Lightmapper::createGBufferDescriptorSetLayout() const {
		const sequence<DescriptorSetLayoutBinding> bindings = {
			// Vertex
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX,
			},

			// Normal
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX,
			},
		};

		return Renderer::createDescriptorSetLayout(bindings);
	}

	ref<DescriptorSetLayout> Lightmapper::createTraceRaysDescriptorSetLayout() const {
		const sequence<DescriptorSetLayoutBinding> bindings = {
			// Trace rays argument
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Index
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

			// G-Buffer position
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// G-Buffer triangle normal
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// G-Buffer vertex normal
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Intermediate lightmap atlas
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},
		};

		return Renderer::createDescriptorSetLayout(bindings);
	}

	ref<DescriptorSetLayout> Lightmapper::createPostProcessingDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding lightmapAtlasBinding = {
			.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::COMPUTE,
		};

		return Renderer::createDescriptorSetLayout(lightmapAtlasBinding);
	}

	ref<DescriptorSetLayout> Lightmapper::createDilationDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding intermediateLightmapAtlasBinding = {
			.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::FRAGMENT,
		};

		return Renderer::createDescriptorSetLayout(intermediateLightmapAtlasBinding);
	}

	ref<GraphicsPipeline> Lightmapper::createGBufferPipeline() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX, false);
		vertexInputBinding.registerAttribute<svec2<float32>>();
		vertexInputBinding.registerAttribute<svec2<uint32>>();

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
			{
				.blend = false,
			},
			{
				.blend = false,
			},
		};

		return Renderer::createGraphicsPipeline({
			.pipelineLayout = gBufferPipelineLayout,
			.vertexInputBindings = {vertexInputBinding},
			.vertexShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "G-Buffer vertex",
					#endif
				},
				"Lightmapper/GBuffer.vert"
			),
			.fragmentShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "G-Buffer fragment",
					#endif
				},
				"Lightmapper/GBuffer.frag"
			),
			.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
			.polygonMode = PolygonMode::FILL,
			.cullMode = CullMode::NONE,
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
			.renderPass = gBufferRenderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer",
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
						.name = "Trace rays compute",
					#endif
				},
				"Lightmapper/TraceRays.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Trace rays",
			#endif
		});
	}

	ref<ComputePipeline> Lightmapper::createIntermediateDilationPipeline() const {
		return Renderer::createComputePipeline({
			.pipelineLayout = postProcessingPipelineLayout,
			.computeShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Intermediate dilation compute",
					#endif
				},
				"Lightmapper/IntermediateDilation.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Intermediate dilation",
			#endif
		});
	}

	ref<ComputePipeline> Lightmapper::createHorizontalBilateralFilterPipeline() const {
		return Renderer::createComputePipeline({
			.pipelineLayout = postProcessingPipelineLayout,
			.computeShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Horizontal bilateral filter compute",
					#endif
				},
				"Lightmapper/HorizontalBilateralFilter.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Horizontal bilateral filter",
			#endif
		});
	}

	ref<ComputePipeline> Lightmapper::createVerticalBilateralFilterPipeline() const {
		return Renderer::createComputePipeline({
			.pipelineLayout = postProcessingPipelineLayout,
			.computeShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertical bilateral filter compute",
					#endif
				},
				"Lightmapper/VerticalBilateralFilter.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertical bilateral filter",
			#endif
		});
	}

	ref<GraphicsPipeline> Lightmapper::createDilationPipeline() const {
		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		return Renderer::createGraphicsPipeline({
			.pipelineLayout = dilationPipelineLayout,
			.vertexInputBindings = {},
			.vertexShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Dilation vertex",
					#endif
				},
				"Lightmapper/Dilation.vert"
			),
			.fragmentShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Dilation fragment",
					#endif
				},
				"Lightmapper/Dilation.frag"
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
			.renderPass = dilationRenderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Dilation",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createTraceRaysArgumentBuffer() const {
		const uint64 alignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(sizeof(TraceRaysArguments), alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_WRITE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Trace rays argument",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createVertexBuffer(uint64 vertexCount) const {
		const uint64 alignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(vertexCount * sizeof(Vertex), alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createPositionBuffer(uint64 size) const {
		const uint64 alignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(size * sizeof(svec3<float32>), alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Position",
			#endif
		});
	}

	ref<Buffer> Lightmapper::createNormalBuffer(uint64 size) const {
		const uint64 alignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(size * sizeof(svec3<float32>), alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Normal",
			#endif
		});
	}

	ref<Sampler> Lightmapper::createIntermediateLightmapAtlasSampler() const {
		return Renderer::createSampler({
			.magFilter = Filter::NEAREST,
			.minFilter = Filter::NEAREST,
			.addressModeU = AddressMode::REPEAT,
			.addressModeV = AddressMode::REPEAT,
			.addressModeW = AddressMode::REPEAT,
		});
	}

	void Lightmapper::encodeOneTimeCommands(view<GeometryIndex3D> vertices, view<svec3<float32>> positions, view<svec3<float32>> normals, view<svec2<float32>> lightmapCoordinates) const {
		const ref<Device> device = Renderer::getDevice();
		const ref<Buffer> stagingBuffer = Renderer::createDynamicBuffer({
			.size = (
				traceRaysArgumentBuffer->getSize() +
				context.vertexBuffer->getSize() +
				context.positionBuffer->getSize() +
				context.normalBuffer->getSize()
			),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const ref<BufferView<TraceRaysArguments>> traceRaysArgumentStagingView = stagingBuffer->createView<TraceRaysArguments>(0, traceRaysArgumentBuffer->getSize());
		const ref<BufferView<Vertex>> vertexStagingView = stagingBuffer->createView<Vertex>(traceRaysArgumentStagingView->getOffset() + traceRaysArgumentStagingView->getSize(), context.vertexBuffer->getSize());
		const ref<BufferView<svec3<float32>>> positionStagingView = stagingBuffer->createView<svec3<float32>>(vertexStagingView->getOffset() + vertexStagingView->getSize(), context.positionBuffer->getSize());
		const ref<BufferView<svec3<float32>>> normalStagingView = stagingBuffer->createView<svec3<float32>>(positionStagingView->getOffset() + positionStagingView->getSize(), context.normalBuffer->getSize());

		TraceRaysArguments* traceRaysArguments = traceRaysArgumentStagingView->map();
		traceRaysArguments->atlasExtent = {imageAssetDescriptor.extent.width, imageAssetDescriptor.extent.height};
		traceRaysArguments->triangleCount = static_cast<uint32>(vertices.count() / 3);
		traceRaysArguments->skyColor = arguments.skyColor;
		traceRaysArguments->sunDirection = arguments.sunDirection;
		traceRaysArguments->sunColor = arguments.sunColor;
		traceRaysArguments->sunIntensity = arguments.sunIntensity;

		Vertex* vertexMappedRange = vertexStagingView->map();
		uint32 vertexIndex = 0;

		while (vertexIndex < vertices.count()) {
			vertexMappedRange[vertexIndex].lightmapCoordinate = lightmapCoordinates[vertexIndex];
			vertexMappedRange[vertexIndex].positionIndex = vertices[vertexIndex].positionIndex;
			vertexMappedRange[vertexIndex].normalIndex = vertices[vertexIndex].normalIndex;

			vertexIndex += 1;
		}

		std::copy(positions.begin(), positions.end(), positionStagingView->map());
		std::copy(normals.begin(), normals.end(), normalStagingView->map());

		stagingBuffer->unmap();

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "One-time commands",
			#endif
		});

		commandEncoder->begin();

		BufferCopyRegion bufferCopyRegion = {
			.sourceOffset = traceRaysArgumentStagingView->getOffset(),
			.destinationOffset = 0,
			.size = traceRaysArgumentStagingView->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, traceRaysArgumentBuffer, bufferCopyRegion);

		bufferCopyRegion = {
			.sourceOffset = vertexStagingView->getOffset(),
			.destinationOffset = 0,
			.size = vertexStagingView->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, context.vertexBuffer, bufferCopyRegion);

		bufferCopyRegion = {
			.sourceOffset = positionStagingView->getOffset(),
			.destinationOffset = 0,
			.size = positionStagingView->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, context.positionBuffer, bufferCopyRegion);

		bufferCopyRegion = {
			.sourceOffset = normalStagingView->getOffset(),
			.destinationOffset = 0,
			.size = normalStagingView->getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, context.normalBuffer, bufferCopyRegion);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		const ref<BufferView<TraceRaysArguments>> traceRaysArgumentView = traceRaysArgumentBuffer->createView<TraceRaysArguments>(0, traceRaysArgumentBuffer->getSize());
		const ref<BufferView<Vertex>> vertexView = context.vertexBuffer->createView<Vertex>(0, context.vertexBuffer->getSize());
		const ref<BufferView<float32>> positionView = context.positionBuffer->createView<float32>(0, context.positionBuffer->getSize());
		const ref<BufferView<float32>> normalView = context.normalBuffer->createView<float32>(0, context.normalBuffer->getSize());

		const DescriptorImage gBufferPositionDescriptorImage = {
			.imageView = context.gBufferPositionImageView,
			.imageLayout = ImageLayout::GENERAL,
			.sampler = nullptr,
		};
		const DescriptorImage gBufferTriangleNormalDescriptorImage = {
			.imageView = context.gBufferTriangleNormalImageView,
			.imageLayout = ImageLayout::GENERAL,
			.sampler = nullptr,
		};
		const DescriptorImage gBufferVertexNormalDescriptorImage = {
			.imageView = context.gBufferVertexNormalImageView,
			.imageLayout = ImageLayout::GENERAL,
			.sampler = nullptr,
		};
		const DescriptorImage intermediateLightmapAtlasDescriptorImage = {
			.imageView = context.intermediateLightmapAtlasImageView,
			.imageLayout = ImageLayout::GENERAL,
			.sampler = nullptr,
		};
		const DescriptorImage sampledIntermediateLightmapAtlasDescriptorImage = {
			.imageView = context.intermediateLightmapAtlasImageView,
			.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
			.sampler = intermediateLightmapAtlasSampler,
		};

		const sequence<WriteDescriptorSet> writeDescriptorSets = {
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 0,
				.descriptorBufferViews = positionView,
			},
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 1,
				.descriptorBufferViews = normalView,
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 0,
				.descriptorBufferViews = traceRaysArgumentView,
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 1,
				.descriptorBufferViews = vertexView,
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 2,
				.descriptorBufferViews = positionView,
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 3,
				.descriptorImages = gBufferPositionDescriptorImage,
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 4,
				.descriptorImages = gBufferTriangleNormalDescriptorImage,
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 5,
				.descriptorImages = gBufferVertexNormalDescriptorImage,
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 6,
				.descriptorImages = intermediateLightmapAtlasDescriptorImage,
			},
			{
				.descriptorSet = descriptorSets[2],
				.bindingIndex = 0,
				.descriptorImages = intermediateLightmapAtlasDescriptorImage,
			},
			{
				.descriptorSet = descriptorSets[3],
				.bindingIndex = 0,
				.descriptorImages = sampledIntermediateLightmapAtlasDescriptorImage,
			},
		};

		device->updateDescriptorSets(writeDescriptorSets);
	}

	void Lightmapper::generateLightmaps(uint32 vertexCount) const {
		const ref<Device> device = Renderer::getDevice();
		const float32 width = static_cast<float32>(imageAssetDescriptor.extent.width);
		const float32 height = static_cast<float32>(imageAssetDescriptor.extent.height);
		const uint32 workGroupX = ceil(imageAssetDescriptor.extent.width / 16, 16u);
		const uint32 workGroupY = ceil(imageAssetDescriptor.extent.height / 16, 16u);
		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Generate lightmaps",
			#endif
		});

		// TODO(graphics/command-encoder/bind-resources): Ensure that descriptor sets and vertex buffers, if any, are always bound after the pipeline.
		commandEncoder->begin();
		commandEncoder->beginRenderPass({
			.framebuffer = context.gBufferFramebuffer,
			.clearValues = {
				{0.0f, 0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f, 0.0f},
			},
		});
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, context.gBufferFramebuffer->getExtent());
		commandEncoder->bindGraphicsPipeline(gBufferPipeline);
		commandEncoder->bindVertexBuffer(0, context.vertexBuffer, 0, sizeof(Vertex));
		commandEncoder->bindDescriptorSets(gBufferPipelineLayout, 0, descriptorSets[0]);
		commandEncoder->draw(vertexCount, 25, 0, 0);
		commandEncoder->endRenderPass();

		// TODO(graphics/render-pass/subpass): Add support for subpass dependencies.
		{
			const sequence<ImageMemoryBarrier> imageMemoryBarriers = {
				{
					.image = context.gBufferPositionImage,
					.newLayout = ImageLayout::GENERAL,
					.newAccess = Access::SHADER_READ,
				},
				{
					.image = context.gBufferTriangleNormalImage,
					.newLayout = ImageLayout::GENERAL,
					.newAccess = Access::SHADER_READ,
				},
				{
					.image = context.gBufferVertexNormalImage,
					.newLayout = ImageLayout::GENERAL,
					.newAccess = Access::SHADER_READ,
				},
				{
					.image = context.intermediateLightmapAtlasImage,
					.newLayout = ImageLayout::GENERAL,
					.newAccess = Access::SHADER_WRITE,
				},
			};

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);
		}

		commandEncoder->bindComputePipeline(traceRaysPipeline);
		commandEncoder->bindDescriptorSets(traceRaysPipelineLayout, 0, descriptorSets[1]);
		commandEncoder->dispatch(workGroupX, workGroupY, 1);

		ImageMemoryBarrier imageMemoryBarrier = {
			.image = context.intermediateLightmapAtlasImage,
			.newLayout = ImageLayout::GENERAL,
			.newAccess = Access::SHADER_READ | Access::SHADER_WRITE,
		};

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

		commandEncoder->bindComputePipeline(intermediateDilationPipeline);
		commandEncoder->bindDescriptorSets(postProcessingPipelineLayout, 0, descriptorSets[2]);
		commandEncoder->dispatch(workGroupX, workGroupY, 1);

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

		commandEncoder->bindComputePipeline(horizontalBilateralFilterPipeline);
		commandEncoder->bindDescriptorSets(postProcessingPipelineLayout, 0, descriptorSets[2]);
		commandEncoder->dispatch(workGroupX, workGroupY, 1);

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

		commandEncoder->bindComputePipeline(verticalBilateralFilterPipeline);
		commandEncoder->bindDescriptorSets(postProcessingPipelineLayout, 0, descriptorSets[2]);
		commandEncoder->dispatch(workGroupX, workGroupY, 1);

		imageMemoryBarrier.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

		commandEncoder->beginRenderPass({
			.framebuffer = context.dilationFramebuffer,
			.clearValues = {{0.0f, 0.0f, 0.0f, 0.0f}},
		});
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, context.dilationFramebuffer->getExtent());
		commandEncoder->bindGraphicsPipeline(dilationPipeline);
		commandEncoder->bindDescriptorSets(dilationPipelineLayout, 0, descriptorSets[3]);
		commandEncoder->draw(3, 1, 0, 0);
		commandEncoder->endRenderPass();

		// TODO(graphics/render-pass/subpass): Add support for subpass dependencies.
		if (Renderer::getGraphicsApi() == GraphicsAPI::DX12) {
			imageMemoryBarrier = {
				.image = context.lightmapAtlasImage,
				.newLayout = ImageLayout::TRANSFER_SRC_OPTIMAL,
				.newAccess = Access::TRANSFER_READ,
			};

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
		}

		const BufferImageCopyRegion imageCopyRegion = {
			.bufferOffset = 0,
			.imageOffsetX = 0,
			.imageOffsetY = 0,
			.imageOffsetZ = 0,
			.imageExtent = context.lightmapAtlasImage->getExtent(),
			.imageMipLevel = context.lightmapAtlasImageView->getMipLevelOffset(),
			.imageLayerOffset = context.lightmapAtlasImageView->getLayerOffset(),
			.imageLayerCount = context.lightmapAtlasImageView->getLayerCount(),
		};

		commandEncoder->copyImageToBuffer(context.lightmapAtlasImage, context.lightmapAtlasStagingBuffer, imageCopyRegion);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();
	}
}