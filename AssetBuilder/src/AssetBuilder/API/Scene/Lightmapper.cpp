// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Lightmapper.hpp"

namespace Trident {
	Lightmapper::Lightmapper(Extent2D atlasExtent) :
		atlasExtent(atlasExtent)
	{}

	void Lightmapper::buildLightmapAtlas(const StaticScene3D& scene, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) {
		const uint32 triangleCount = static_cast<uint32>(scene.triangles.count());
		const ref<Device> device = Renderer::getDevice();

		rasterizationRenderPass = createRasterizationRenderPass(lightmapAtlasImageAssetDescriptor.format);
		dilationRenderPass = createDilationRenderPass(lightmapAtlasImageAssetDescriptor.format);

		intermediateLightmapAtlasImage = createIntermediateLightmapAtlasImage(lightmapAtlasImageAssetDescriptor);
		lightmapAtlasImage = createLightmapAtlasImage(lightmapAtlasImageAssetDescriptor);
		intermediateLightmapAtlasImageView = createIntermediateLightmapAtlasImageView();
		lightmapAtlasImageView = createLightmapAtlasImageView();

		rasterizationFramebuffer = createRasterizationFramebuffer();
		dilationFramebuffer = createDilationFramebuffer();

		const ref<DescriptorSetLayout> rasterizationDescriptorSetLayout = createRasterizationDescriptorSetLayout();
		const ref<DescriptorSetLayout> traceRaysDescriptorSetLayout = createTraceRaysDescriptorSetLayout();
		const ref<DescriptorSetLayout> postProcessingDescriptorSetLayout = createPostProcessingDescriptorSetLayout();
		const ref<DescriptorSetLayout> dilationDescriptorSetLayout = createDilationDescriptorSetLayout();

		rasterizationPipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {rasterizationDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Rasterization",
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

		rasterizationPipeline = createRasterizationPipeline();
		traceRaysPipeline = createTraceRaysPipeline();
		intermediateDilationPipeline = createIntermediateDilationPipeline();
		horizontalBilateralFilterPipeline = createHorizontalBilateralFilterPipeline();
		verticalBilateralFilterPipeline = createVerticalBilateralFilterPipeline();
		dilationPipeline = createDilationPipeline();

		const sequence<ref<DescriptorSet>> descriptorSets = device->allocateDescriptorSets(sequence<ref<DescriptorSetLayout>>{
			rasterizationDescriptorSetLayout,
			traceRaysDescriptorSetLayout,
			postProcessingDescriptorSetLayout,
			dilationDescriptorSetLayout,
		});

		rasterizationDescriptorSet = descriptorSets[0];
		traceRaysDescriptorSet = descriptorSets[1];
		postProcessingDescriptorSet = descriptorSets[2];
		dilationDescriptorSet = descriptorSets[3];

		lightmapCoordinateBuffer = createLightmapCoordinateBuffer(triangleCount * sizeof(svec2<float32>) * 3);
		traceRaysArgumentBuffer = createTraceRaysArgumentBuffer();
		luxelBuffer = createLuxelBuffer();
		triangleBuffer = createTriangleBuffer(triangleCount * sizeof(StaticTriangle3D));
		vertexComponentBuffer = createVertexComponentBuffer(scene.vertices.count() * sizeof(svec3<float32>));
		normalComponentBuffer = createNormalComponentBuffer(scene.normals.count() * sizeof(svec3<float32>));

		intermediateLightmapAtlasSampler = createIntermediateLightmapAtlasSampler();

		encodeOneTimeCommands(scene);

		lightmapAtlasStagingBuffer = Renderer::createDynamicBuffer({
			.size = lightmapAtlasImage->getSize(),
			.usage = BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas staging",
			#endif
		});

		generateLightmaps(triangleCount * 3);

		const ref<BufferView<uint8>> lightmapAtlasView = lightmapAtlasStagingBuffer->createView<uint8>(0, lightmapAtlasStagingBuffer->getSize());

		std::copy_n(lightmapAtlasView->map(), lightmapAtlasImageAssetDescriptor.data.count(), &lightmapAtlasImageAssetDescriptor.data[0]);

		lightmapAtlasStagingBuffer->unmap();
	}

	ref<RenderPass> Lightmapper::createRasterizationRenderPass(ImageFormat lightmapAtlasImageFormat) const {
		return Renderer::createRenderPass({
			.attachments = {
				// Intermediate lightmap atlas
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

	ref<RenderPass> Lightmapper::createDilationRenderPass(ImageFormat lightmapAtlasImageFormat) const {
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

	ref<Image> Lightmapper::createIntermediateLightmapAtlasImage(const ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) const {
		return rasterizationRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = lightmapAtlasImageAssetDescriptor.extent,
				.mipLevelCount = lightmapAtlasImageAssetDescriptor.mipLevelCount,
				.layerCount = lightmapAtlasImageAssetDescriptor.layerCount,
				.additionalUsage = ImageUsage::SAMPLED | ImageUsage::STORAGE,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Intermediate lightmap atlas",
				#endif
			},
			0
		);
	}

	ref<Image> Lightmapper::createLightmapAtlasImage(const ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) const {
		return dilationRenderPass->createColorAttachmentImage(
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

	ref<ImageView> Lightmapper::createIntermediateLightmapAtlasImageView() {
		return intermediateLightmapAtlasImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Intermediate lightmap atlas view",
			#endif
		});
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

	ref<Framebuffer> Lightmapper::createRasterizationFramebuffer() const {
		const Extent3D extent = intermediateLightmapAtlasImage->getExtent();

		return Renderer::createFramebuffer({
			.renderPass = rasterizationRenderPass,
			.imageViews = {intermediateLightmapAtlasImageView},
			.extent = {extent.width, extent.height},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Rasterization",
			#endif
		});
	}

	ref<Framebuffer> Lightmapper::createDilationFramebuffer() const {
		const Extent3D extent = lightmapAtlasImage->getExtent();

		return Renderer::createFramebuffer({
			.renderPass = dilationRenderPass,
			.imageViews = {lightmapAtlasImageView},
			.extent = {extent.width, extent.height},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Dilation",
			#endif
		});
	}

	ref<DescriptorSetLayout> Lightmapper::createRasterizationDescriptorSetLayout() const {
		return Renderer::createDescriptorSetLayout({
			// Trace rays argument
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},

			// Luxel
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

			// Luxel
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

			// Intermediate lightmap atlas
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

	ref<DescriptorSetLayout> Lightmapper::createDilationDescriptorSetLayout() const {
		return Renderer::createDescriptorSetLayout({
			// Intermediate lightmap atlas
			{
				.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},
		});
	}

	ref<GraphicsPipeline> Lightmapper::createRasterizationPipeline() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>();

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		return Renderer::createGraphicsPipeline({
			.pipelineLayout = rasterizationPipelineLayout,
			.vertexInputBindings = {vertexInputBinding},
			.vertexShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Rasterization vertex",
					#endif
				},
				"Lightmapper/Rasterization.vert"
			),
			.fragmentShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Rasterization fragment",
					#endif
				},
				"Lightmapper/Rasterization.frag"
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
			.renderPass = rasterizationRenderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Rasterization",
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

	ref<Buffer> Lightmapper::createLightmapCoordinateBuffer(uint64 size) const {
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(size, minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap coordinate",
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

	ref<Buffer> Lightmapper::createLuxelBuffer() const {
		const Extent3D lightmapAtlasImageExtent = intermediateLightmapAtlasImage->getExtent();
		const uint64 size = lightmapAtlasImageExtent.width * lightmapAtlasImageExtent.height * sizeof(Luxel);
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(size, minStorageBufferOffsetAlignment),
			.usage = BufferUsage::READ_WRITE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Luxel",
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

	ref<Sampler> Lightmapper::createIntermediateLightmapAtlasSampler() const {
		return Renderer::createSampler({
			.magFilter = Filter::NEAREST,
			.minFilter = Filter::NEAREST,
			.addressModeU = AddressMode::REPEAT,
			.addressModeV = AddressMode::REPEAT,
			.addressModeW = AddressMode::REPEAT,
		});
	}

	void Lightmapper::encodeOneTimeCommands(const StaticScene3D& scene) const {
		const ref<Device> device = Renderer::getDevice();
		const ref<Buffer> stagingBuffer = Renderer::createDynamicBuffer({
			.size = lightmapCoordinateBuffer->getSize() + traceRaysArgumentBuffer->getSize() + triangleBuffer->getSize() + vertexComponentBuffer->getSize() + normalComponentBuffer->getSize(),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const ref<BufferView<svec2<float32>>> lightmapCoordinateStagingView = stagingBuffer->createView<svec2<float32>>(0, lightmapCoordinateBuffer->getSize());
		const ref<BufferView<TraceRaysArguments>> traceRaysArgumentStagingView = stagingBuffer->createView<TraceRaysArguments>(lightmapCoordinateStagingView->getOffset() + lightmapCoordinateStagingView->getSize(), traceRaysArgumentBuffer->getSize());
		const ref<BufferView<StaticTriangle3D>> triangleStagingView = stagingBuffer->createView<StaticTriangle3D>(traceRaysArgumentStagingView->getOffset() + traceRaysArgumentStagingView->getSize(), triangleBuffer->getSize());
		const ref<BufferView<svec3<float32>>> vertexComponentStagingView = stagingBuffer->createView<svec3<float32>>(triangleStagingView->getOffset() + triangleStagingView->getSize(), vertexComponentBuffer->getSize());
		const ref<BufferView<svec3<float32>>> normalComponentStagingView = stagingBuffer->createView<svec3<float32>>(vertexComponentStagingView->getOffset() + vertexComponentStagingView->getSize(), normalComponentBuffer->getSize());

		svec2<float32>* lightmapCoordinateMappedRange = lightmapCoordinateStagingView->map();
		uint32 lightmapCoordinateIndex = 0;

		for (const StaticTriangle3D& triangle : scene.triangles) {
			lightmapCoordinateMappedRange[lightmapCoordinateIndex + 0] = triangle.vertices[0].lightmapCoordinate;
			lightmapCoordinateMappedRange[lightmapCoordinateIndex + 1] = triangle.vertices[1].lightmapCoordinate;
			lightmapCoordinateMappedRange[lightmapCoordinateIndex + 2] = triangle.vertices[2].lightmapCoordinate;

			lightmapCoordinateIndex += 3;
		}

		TraceRaysArguments* traceRaysArguments = traceRaysArgumentStagingView->map();
		traceRaysArguments->atlasExtent = atlasExtent;
		traceRaysArguments->luxelCount = 0;
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
		const ref<BufferView<Luxel>> luxelView = luxelBuffer->createView<Luxel>(0, luxelBuffer->getSize());
		const ref<BufferView<StaticTriangle3D>> triangleView = triangleBuffer->createView<StaticTriangle3D>(0, triangleBuffer->getSize());
		const ref<BufferView<float32>> vertexComponentView = vertexComponentBuffer->createView<float32>(0, vertexComponentBuffer->getSize());
		const ref<BufferView<float32>> normalComponentView = normalComponentBuffer->createView<float32>(0, normalComponentBuffer->getSize());

		const DescriptorImage intermediateLightmapAtlasDescriptorImage = {
			.imageView = intermediateLightmapAtlasImageView,
			.imageLayout = ImageLayout::GENERAL,
			.sampler = nullptr,
		};
		const DescriptorImage sampledIntermediateLightmapAtlasDescriptorImage = {
			.imageView = intermediateLightmapAtlasImageView,
			.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
			.sampler = intermediateLightmapAtlasSampler,
		};

		const sequence<WriteDescriptorSet> writeDescriptorSets = {
			{
				.descriptorSet = rasterizationDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = traceRaysArgumentView,
			},
			{
				.descriptorSet = rasterizationDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = luxelView,
			},
			{
				.descriptorSet = traceRaysDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = traceRaysArgumentView,
			},
			{
				.descriptorSet = traceRaysDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = luxelView,
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
				.descriptorImages = intermediateLightmapAtlasDescriptorImage,
			},
			{
				.descriptorSet = postProcessingDescriptorSet,
				.bindingIndex = 0,
				.descriptorImages = intermediateLightmapAtlasDescriptorImage,
			},
			{
				.descriptorSet = dilationDescriptorSet,
				.bindingIndex = 0,
				.descriptorImages = sampledIntermediateLightmapAtlasDescriptorImage,
			},
		};

		device->updateDescriptorSets(writeDescriptorSets);
	}

	void Lightmapper::generateLightmaps(uint32 vertexCount) const {
		const ref<Device> device = Renderer::getDevice();
		const Extent2D extent = atlasExtent;
		const float32 width = static_cast<float32>(extent.width);
		const float32 height = static_cast<float32>(extent.height);
		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Generate lightmaps",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->beginRenderPass({
			.framebuffer = rasterizationFramebuffer,
			.clearValues = {{0.0f, 0.0f, 0.0f, 0.0f}},
		});
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, extent);
		commandEncoder->bindGraphicsPipeline(rasterizationPipeline);
		commandEncoder->bindVertexBuffer(0, lightmapCoordinateBuffer, 0, sizeof(svec2<float32>));
		commandEncoder->bindDescriptorSets(rasterizationPipelineLayout, 0, rasterizationDescriptorSet);
		commandEncoder->draw(vertexCount, 1, 0, 0);
		commandEncoder->endRenderPass();

		ImageMemoryBarrier imageMemoryBarrier;

		// TODO(graphics/dx12/end-render-pass): Automatically transition the attachments to their final layout.
		if (Renderer::getGraphicsApi() == GraphicsAPI::DX12) {
			imageMemoryBarrier = {
				.image = intermediateLightmapAtlasImage,
				.newLayout = ImageLayout::GENERAL,
				.newAccess = Access::SHADER_WRITE,
			};

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
		}

		commandEncoder->bindComputePipeline(traceRaysPipeline);
		commandEncoder->bindDescriptorSets(traceRaysPipelineLayout, 0, traceRaysDescriptorSet);

		// TODO(graphics/dx12/dispatch): Fix artifacts on the lightmap atlas due to not enough threads being dispatched.
		commandEncoder->dispatchIndirect(traceRaysArgumentBuffer, offsetof(TraceRaysArguments, TraceRaysArguments::traceRaysCommand));

		const uint32 workGroupX = ceil(extent.width / 16, 16u);
		const uint32 workGroupY = ceil(extent.height / 16, 16u);

		// TODO(graphics/dx12/vulkan): This barrier's layout is the same as the current layout but Vulkan needs a sync point after the raytracing.
		imageMemoryBarrier = {
			.image = intermediateLightmapAtlasImage,
			.newLayout = ImageLayout::GENERAL,
			.newAccess = Access::SHADER_READ | Access::SHADER_WRITE,
		};

		// TODO(graphics/command-encoder): Add support for pipeline barriers.
		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

		commandEncoder->bindComputePipeline(intermediateDilationPipeline);
		commandEncoder->bindDescriptorSets(postProcessingPipelineLayout, 0, postProcessingDescriptorSet);
		commandEncoder->dispatch(workGroupX, workGroupY, 1);

		// TODO(graphics/command-encoder): Add support for pipeline barriers.
		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

		commandEncoder->bindComputePipeline(horizontalBilateralFilterPipeline);
		commandEncoder->bindDescriptorSets(postProcessingPipelineLayout, 0, postProcessingDescriptorSet);
		commandEncoder->dispatch(workGroupX, workGroupY, 1);

		// TODO(graphics/command-encoder): Add support for pipeline barriers.
		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

		commandEncoder->bindComputePipeline(verticalBilateralFilterPipeline);
		commandEncoder->bindDescriptorSets(postProcessingPipelineLayout, 0, postProcessingDescriptorSet);
		commandEncoder->dispatch(workGroupX, workGroupY, 1);

		imageMemoryBarrier = {
			.image = intermediateLightmapAtlasImage,
			.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
			.newAccess = Access::SHADER_READ,
		};

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

		commandEncoder->beginRenderPass({
			.framebuffer = dilationFramebuffer,
			.clearValues = {{0.0f, 0.0f, 0.0f, 0.0f}},
		});
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, extent);
		commandEncoder->bindGraphicsPipeline(dilationPipeline);
		commandEncoder->bindDescriptorSets(dilationPipelineLayout, 0, dilationDescriptorSet);
		commandEncoder->draw(3, 1, 0, 0);
		commandEncoder->endRenderPass();

		// TODO(graphics/dx12/end-render-pass): Automatically transition the attachments to their final layout.
		if (Renderer::getGraphicsApi() == GraphicsAPI::DX12) {
			imageMemoryBarrier = {
				.image = lightmapAtlasImage,
				.newLayout = ImageLayout::TRANSFER_SRC_OPTIMAL,
				.newAccess = Access::TRANSFER_READ,
			};

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
		}

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
	}
}