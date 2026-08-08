// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Lightmapper/ArgumentBuffer.hpp"
#include "AssetBuilder/API/Lightmapper/BilateralFilterPipelines.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferDescriptorSetLayout.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferFramebuffer.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferPipeline.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferPositionImage.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferRenderPass.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferTriangleNormalImage.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferVertexNormalImage.hpp"
#include "AssetBuilder/API/Lightmapper/IndexBuffer.hpp"
#include "AssetBuilder/API/Lightmapper/IntermediateDilationPipeline.hpp"
#include "AssetBuilder/API/Lightmapper/IntermediateLightmapAtlasImage.hpp"
#include "AssetBuilder/API/Lightmapper/IntermediateLightmapAtlasSampler.hpp"
#include "AssetBuilder/API/Lightmapper/LightmapAtlasImage.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"
#include "AssetBuilder/API/Lightmapper/PostProcessingDescriptorSetLayout.hpp"
#include "AssetBuilder/API/Lightmapper/TraceRaysDescriptorSetLayout.hpp"
#include "AssetBuilder/API/Lightmapper/TraceRaysPipeline.hpp"
#include "AssetBuilder/API/Lightmapper/VertexBuffer.hpp"

namespace Trident {
	void Lightmapper::initialize() {
		gBufferRenderPass = createGBufferRenderPass();
		dilationRenderPass = createDilationRenderPass();

		descriptorSetLayouts[0] = createGBufferDescriptorSetLayout();
		descriptorSetLayouts[1] = createTraceRaysDescriptorSetLayout();
		descriptorSetLayouts[2] = createPostProcessingDescriptorSetLayout();
		descriptorSetLayouts[3] = createDilationDescriptorSetLayout();

		gBufferPipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[0]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer",
			#endif
		});
		traceRaysPipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[1]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Trace rays",
			#endif
		});
		postProcessingPipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[2]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Post processing",
			#endif
		});
		dilationPipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[3]},
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

		descriptorSets = Renderer::getDevice()->allocateDescriptorSets(view<ref<DescriptorSetLayout>>(descriptorSetLayouts, 4));

		intermediateLightmapAtlasSampler = createIntermediateLightmapAtlasSampler();

		argumentBuffer = createArgumentBuffer();

		initialized = true;
	}

	void Lightmapper::generateAtlas(ImageAssetDescriptor& imageAssetDescriptor, const LightmapperDescriptor& descriptor) {
		DEBUG_ASSERT(descriptor.lightmapCoordinates.count() == descriptor.vertices.count());

		this->imageAssetDescriptor = imageAssetDescriptor;
		lighting = descriptor.lighting;

		if (!initialized) {
			initialize();
		}

		const uint32 vertexCount = static_cast<uint32>(descriptor.vertices.count());
		const uint32 indexCount = static_cast<uint32>(descriptor.indices.count());

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

		context.vertexBuffer = createVertexBuffer(vertexCount);
		context.indexBuffer = createIndexBuffer(indexCount);

		encodeOneTimeCommands(descriptor.vertices, descriptor.lightmapCoordinates, descriptor.indices);

		context.lightmapAtlasStagingBuffer = Renderer::createDynamicBuffer({
			.size = context.lightmapAtlasImage->getSize(),
			.usage = BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas staging",
			#endif
		});

		generateLightmaps(indexCount);

		const ref<BufferView<uint8>> lightmapAtlasView = context.lightmapAtlasStagingBuffer->createView<uint8>(0, context.lightmapAtlasStagingBuffer->getSize());

		std::copy_n(lightmapAtlasView->map(), imageAssetDescriptor.data.count(), &imageAssetDescriptor.data[0]);

		context.lightmapAtlasStagingBuffer->unmap();
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

	ref<DescriptorSetLayout> Lightmapper::createDilationDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding intermediateLightmapAtlasBinding = {
			.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::FRAGMENT,
		};

		return Renderer::createDescriptorSetLayout(intermediateLightmapAtlasBinding);
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

	void Lightmapper::encodeOneTimeCommands(view<BuildVertex3D> vertices, view<svec2<float32>> lightmapCoordinates, view<uint32> indices) const {
		const ref<Device> device = Renderer::getDevice();
		const ref<Buffer> stagingBuffer = Renderer::createDynamicBuffer({
			.size = argumentBuffer->getSize() + context.vertexBuffer->getSize() + context.indexBuffer->getSize(),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const ref<BufferView<Arguments>> argumentStagingView = stagingBuffer->createView<Arguments>(0, argumentBuffer->getSize());
		const ref<BufferView<BuildVertex3D>> vertexStagingView = stagingBuffer->createView<BuildVertex3D>(argumentStagingView->getOffset() + argumentStagingView->getSize(), context.vertexBuffer->getSize());
		const ref<BufferView<uint32>> indexStagingView = stagingBuffer->createView<uint32>(vertexStagingView->getOffset() + vertexStagingView->getSize(), context.indexBuffer->getSize());

		Arguments* arguments = argumentStagingView->map();
		arguments->atlasExtent = {imageAssetDescriptor.extent.width, imageAssetDescriptor.extent.height};
		arguments->triangleCount = static_cast<uint32>(indices.count() / 3);
		arguments->skyColor = lighting.skyColor;
		arguments->sunDirection = lighting.sunDirection;
		arguments->sunColor = lighting.sunColor;
		arguments->sunIntensity = lighting.sunIntensity;

		BuildVertex3D* vertexMappedRange = vertexStagingView->map();
		uint32 vertexIndex = 0;

		while (vertexIndex < vertices.count()) {
			vertexMappedRange[vertexIndex] = vertices[vertexIndex];
			vertexMappedRange[vertexIndex].textureCoordinate = lightmapCoordinates[vertexIndex];

			vertexIndex += 1;
		}

		std::copy(indices.begin(), indices.end(), indexStagingView->map());

		stagingBuffer->unmap();

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "One-time commands",
			#endif
		});

		commandEncoder->begin();

		const BufferCopyRegion bufferCopyRegions[3] = {
			{
				.sourceOffset = argumentStagingView->getOffset(),
				.destinationOffset = 0,
				.size = argumentStagingView->getSize(),
			},
			{
				.sourceOffset = vertexStagingView->getOffset(),
				.destinationOffset = 0,
				.size = vertexStagingView->getSize(),
			},
			{
				.sourceOffset = indexStagingView->getOffset(),
				.destinationOffset = 0,
				.size = indexStagingView->getSize(),
			},
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, argumentBuffer, bufferCopyRegions[0]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, context.vertexBuffer, bufferCopyRegions[1]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, context.indexBuffer, bufferCopyRegions[2]);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		const ref<BaseBufferView> bufferViews[3] = {
			argumentBuffer->createView<Arguments>(0, argumentBuffer->getSize()),
			context.vertexBuffer->createView<BuildVertex3D>(0, context.vertexBuffer->getSize()),
			context.indexBuffer->createView<uint32>(0, context.indexBuffer->getSize()),
		};

		const DescriptorImage descriptorImages[5] = {
			{
				.imageView = context.gBufferPositionImageView,
				.imageLayout = ImageLayout::GENERAL,
				.sampler = nullptr,
			},
			{
				.imageView = context.gBufferTriangleNormalImageView,
				.imageLayout = ImageLayout::GENERAL,
				.sampler = nullptr,
			},
			{
				.imageView = context.gBufferVertexNormalImageView,
				.imageLayout = ImageLayout::GENERAL,
				.sampler = nullptr,
			},
			{
				.imageView = context.intermediateLightmapAtlasImageView,
				.imageLayout = ImageLayout::GENERAL,
				.sampler = nullptr,
			},
			{
				.imageView = context.intermediateLightmapAtlasImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = intermediateLightmapAtlasSampler,
			},
		};

		const WriteDescriptorSet writeDescriptorSets[10] = {
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 0,
				.descriptorBufferViews = bufferViews[1],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 0,
				.descriptorBufferViews = bufferViews[0],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 1,
				.descriptorBufferViews = bufferViews[1],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 2,
				.descriptorBufferViews = bufferViews[2],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 3,
				.descriptorImages = descriptorImages[0],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 4,
				.descriptorImages = descriptorImages[1],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 5,
				.descriptorImages = descriptorImages[2],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 6,
				.descriptorImages = descriptorImages[3],
			},
			{
				.descriptorSet = descriptorSets[2],
				.bindingIndex = 0,
				.descriptorImages = descriptorImages[3],
			},
			{
				.descriptorSet = descriptorSets[3],
				.bindingIndex = 0,
				.descriptorImages = descriptorImages[4],
			},
		};

		device->updateDescriptorSets(view<WriteDescriptorSet>(writeDescriptorSets, 10));
	}

	void Lightmapper::generateLightmaps(uint32 indexCount) const {
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
		commandEncoder->bindVertexBuffer(0, context.indexBuffer, 0);
		commandEncoder->bindDescriptorSets(gBufferPipelineLayout, 0, descriptorSets[0]);
		commandEncoder->draw(indexCount, 25, 0, 0);
		commandEncoder->endRenderPass();

		// TODO(graphics/render-pass/subpass): Add support for subpass dependencies.
		{
			const ImageMemoryBarrier imageMemoryBarriers[4] = {
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

			commandEncoder->insertMemoryBarriers({}, view<ImageMemoryBarrier>(imageMemoryBarriers, 4));
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