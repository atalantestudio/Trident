// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

#include "AssetBuilder/API/Lightmapper/AccelerationStructure.hpp"
#include "AssetBuilder/API/Lightmapper/ArgumentBuffer.hpp"
#include "AssetBuilder/API/Lightmapper/BilateralFilterPipelines.hpp"
#include "AssetBuilder/API/Lightmapper/EmissiveTriangleIndexBuffer.hpp"
#include "AssetBuilder/API/Lightmapper/GBuffer.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferDescriptorSetLayout.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferFramebuffer.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferPipeline.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferRenderPass.hpp"
#include "AssetBuilder/API/Lightmapper/IntermediateDilationPipeline.hpp"
#include "AssetBuilder/API/Lightmapper/IntermediateLightmapAtlasImage.hpp"
#include "AssetBuilder/API/Lightmapper/IntermediateLightmapAtlasSampler.hpp"
#include "AssetBuilder/API/Lightmapper/LightmapAtlasImage.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"
#include "AssetBuilder/API/Lightmapper/MaterialBuffer.hpp"
#include "AssetBuilder/API/Lightmapper/PostProcessingDescriptorSetLayout.hpp"
#include "AssetBuilder/API/Lightmapper/RayTracingDescriptorSetLayout.hpp"
#include "AssetBuilder/API/Lightmapper/RayTracingPipeline.hpp"
#include "AssetBuilder/API/Lightmapper/ShaderBindingTableBuffer.hpp"

namespace Trident {
	const GraphicsAdapter* Lightmapper::adapter;

	Lightmapper::~Lightmapper() {
		gBufferDescriptorSetLayout.~ref();
		rayTracingDescriptorSetLayout.~ref();
		postProcessingDescriptorSetLayout.~ref();
		dilationDescriptorSetLayout.~ref();
	}

	void Lightmapper::initialize() {
		instance = adapter->createInstance({
			.projectName = "Lightmapper",
			.projectVersion = {0, 0, 1},
		});
		physicalDevice = instance->selectPhysicalDevice(false);
		limits = &physicalDevice->getLimits();
		device = physicalDevice->createDevice<EXT_hardware_ray_tracing>();

		gBufferRenderPass = createGBufferRenderPass();
		dilationRenderPass = createDilationRenderPass();

		gBufferDescriptorSetLayout = createGBufferDescriptorSetLayout();
		rayTracingDescriptorSetLayout = createRayTracingDescriptorSetLayout();
		postProcessingDescriptorSetLayout = createPostProcessingDescriptorSetLayout();
		dilationDescriptorSetLayout = createDilationDescriptorSetLayout();

		gBufferPipelineLayout = device->createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[0]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer",
			#endif
		});
		rayTracingPipelineLayout = device->createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[1]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Ray tracing",
			#endif
		});
		postProcessingPipelineLayout = device->createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[2]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Post processing",
			#endif
		});
		dilationPipelineLayout = device->createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[3]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Dilation",
			#endif
		});

		gBufferPipeline = createGBufferPipeline();
		rayTracingPipeline = createRayTracingPipeline();
		intermediateDilationPipeline = createIntermediateDilationPipeline();
		horizontalBilateralFilterPipeline = createHorizontalBilateralFilterPipeline();
		verticalBilateralFilterPipeline = createVerticalBilateralFilterPipeline();
		dilationPipeline = createDilationPipeline();

		descriptorSets = device->allocateDescriptorSets(descriptorSetLayouts);

		intermediateLightmapAtlasSampler = createIntermediateLightmapAtlasSampler();

		argumentBuffer = createArgumentBuffer();

		createShaderBindingTableBuffer();

		initialized = true;
	}

	ref<ShaderModule> Lightmapper::loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) const {
		std::string pathString = "Asset/Shader/";
		pathString += path;

		switch (adapter->getApi()) {
			case GraphicsAPI::DX12: {
				pathString += ".hlsl";

				break;
			}
			case GraphicsAPI::GL: {
				#if defined(GL_SOURCE_FROM_SPIRV)
					pathString += ".spv";
				#else
					pathString += ".glsl";
				#endif

				break;
			}
			case GraphicsAPI::VK: {
				pathString += ".spv";

				break;
			}
			default: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
		}

		path = view<char8>(&pathString[0], pathString.size());

		const sequence<char8> code = Filesystem::openTextBuffer(&path[0]);

		return device->createShaderModule(descriptor, view<char8>(&code[0], code.count()));
	}

	void Lightmapper::generateAtlas(ImageAssetDescriptor& imageAssetDescriptor, const LightmapperDescriptor& descriptor) {
		DEBUG_ASSERT(descriptor.lightmapCoordinates.count() == descriptor.vertices.count());

		this->imageAssetDescriptor = imageAssetDescriptor;
		sceneDescriptor = descriptor.sceneDescriptor;

		if (!initialized) {
			initialize();
		}

		buildAccelerationStructures(*descriptor.scene);

		const uint64 indexCount = descriptor.indices.count();
		const uint64 materialCount = descriptor.materials.count();
		const uint64 emissiveTriangleIndexCount = descriptor.emissiveTriangleIndices.count();

		context = {};
		context.gBufferPositionImage = createGBufferPositionImage();
		context.gBufferSmoothPositionImage = createGBufferSmoothPositionImage();
		context.gBufferTriangleNormalImage = createGBufferTriangleNormalImage();
		context.gBufferVertexNormalImage = createGBufferVertexNormalImage();
		context.intermediateLightmapAtlasImage = createIntermediateLightmapAtlasImage();
		context.lightmapAtlasImage = createLightmapAtlasImage();
		context.gBufferPositionImageView = createGBufferImageView(context.gBufferPositionImage, "G-Buffer position");
		context.gBufferSmoothPositionImageView = createGBufferImageView(context.gBufferSmoothPositionImage, "G-Buffer smooth position");
		context.gBufferTriangleNormalImageView = createGBufferImageView(context.gBufferTriangleNormalImage, "G-Buffer triangle normal");
		context.gBufferVertexNormalImageView = createGBufferImageView(context.gBufferVertexNormalImage, "G-Buffer vertex normal");
		context.intermediateLightmapAtlasImageView = createIntermediateLightmapAtlasImageView();
		context.lightmapAtlasImageView = createLightmapAtlasImageView();
		context.gBufferFramebuffer = createGBufferFramebuffer();
		context.dilationFramebuffer = createDilationFramebuffer();
		context.materialBuffer = createMaterialBuffer(materialCount);
		context.emissiveTriangleIndexBuffer = createEmissiveTriangleIndexBuffer(emissiveTriangleIndexCount);
		context.lightmapAtlasStagingBuffer = device->createDynamicBuffer({
			.size = context.lightmapAtlasImage->getSize(),
			.usage = BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas staging",
			#endif
		});

		encodeOneTimeCommands(descriptor);

		generateLightmaps(static_cast<uint32>(indexCount));

		const BufferView<uint8> lightmapAtlasView = context.lightmapAtlasStagingBuffer->createView<uint8>(0, context.lightmapAtlasStagingBuffer->getSize());

		std::copy_n(lightmapAtlasView.map(), imageAssetDescriptor.data.count(), &imageAssetDescriptor.data[0]);

		context.lightmapAtlasStagingBuffer->unmap();
	}

	ref<RenderPass> Lightmapper::createDilationRenderPass() const {
		return device->createRenderPass({
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
		return device->createFramebuffer({
			.renderPass = dilationRenderPass,
			.imageViews = {context.lightmapAtlasImageView},
			.extent = {imageAssetDescriptor.extent.width, imageAssetDescriptor.extent.height},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Dilation",
			#endif
		});
	}

	ref<DescriptorSetLayout> Lightmapper::createDilationDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Intermediate lightmap atlas
			{
				.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::FRAGMENT,
			},
		};

		return device->createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Dilation",
			#endif
		});
	}

	ref<GraphicsPipeline> Lightmapper::createDilationPipeline() const {
		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = false,
			},
		};

		return device->createGraphicsPipeline({
			.pipelineLayout = dilationPipelineLayout,
			.vertexInputBindings = {},
			.vertexShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Dilation vertex",
					#endif
				},
				"Lightmapper/Dilation.vert"
			),
			.fragmentShaderModule = loadShaderModule(
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

	void Lightmapper::encodeOneTimeCommands(const LightmapperDescriptor& descriptor) const {
		const ref<Buffer> stagingBuffer = device->createDynamicBuffer({
			.size = argumentBuffer->getSize() + vertexBuffer->getSize() + indexBuffer->getSize() + context.materialBuffer->getSize() + context.emissiveTriangleIndexBuffer->getSize(),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const BufferView<Arguments> argumentStagingView = stagingBuffer->createView<Arguments>(0, argumentBuffer->getSize());
		const BufferView<BuildVertex3D> vertexStagingView = stagingBuffer->createView<BuildVertex3D>(argumentStagingView.getOffset() + argumentStagingView.getSize(), vertexBuffer->getSize());
		const BufferView<uint32> indexStagingView = stagingBuffer->createView<uint32>(vertexStagingView.getOffset() + vertexStagingView.getSize(), indexBuffer->getSize());
		const BufferView<LightmapMaterial> materialStagingView = stagingBuffer->createView<LightmapMaterial>(indexStagingView.getOffset() + indexStagingView.getSize(), context.materialBuffer->getSize());
		const BufferView<uint32> emissiveTriangleIndexStagingView = stagingBuffer->createView<uint32>(materialStagingView.getOffset() + materialStagingView.getSize(), context.emissiveTriangleIndexBuffer->getSize());

		Arguments* arguments = argumentStagingView.map();
		arguments->atlasExtent = {imageAssetDescriptor.extent.width, imageAssetDescriptor.extent.height};
		arguments->triangleCount = static_cast<uint32>(descriptor.indices.count() / 3);
		arguments->emissiveTriangleCount = static_cast<uint32>(descriptor.emissiveTriangleIndices.count());
		arguments->skyColor = descriptor.sceneDescriptor.skyColor;

		if (descriptor.sceneDescriptor.flags & Scene3DBuildFlags::LIGHTMAP_SUN_CONTRIBUTION) {
			arguments->sunDirection = descriptor.sceneDescriptor.sunDirection;
			arguments->sunColor = descriptor.sceneDescriptor.sunColor;
			arguments->sunIntensity = descriptor.sceneDescriptor.sunIntensity;
		}

		BuildVertex3D* vertexMappedRange = vertexStagingView.map();
		uint32 vertexIndex = 0;

		while (vertexIndex < descriptor.vertices.count()) {
			vertexMappedRange[vertexIndex] = descriptor.vertices[vertexIndex];
			vertexMappedRange[vertexIndex].textureCoordinate = descriptor.lightmapCoordinates[vertexIndex];

			vertexIndex += 1;
		}

		std::copy(descriptor.indices.begin(), descriptor.indices.end(), indexStagingView.map());
		std::copy(descriptor.materials.begin(), descriptor.materials.end(), materialStagingView.map());
		std::copy(descriptor.emissiveTriangleIndices.begin(), descriptor.emissiveTriangleIndices.end(), emissiveTriangleIndexStagingView.map());

		stagingBuffer->unmap();

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "One-time commands",
			#endif
		});

		commandEncoder->begin();

		const BufferCopyRegion bufferCopyRegions[] = {
			{
				.sourceOffset = argumentStagingView.getOffset(),
				.destinationOffset = 0,
				.size = argumentStagingView.getSize(),
			},
			{
				.sourceOffset = vertexStagingView.getOffset(),
				.destinationOffset = 0,
				.size = vertexStagingView.getSize(),
			},
			{
				.sourceOffset = indexStagingView.getOffset(),
				.destinationOffset = 0,
				.size = indexStagingView.getSize(),
			},
			{
				.sourceOffset = materialStagingView.getOffset(),
				.destinationOffset = 0,
				.size = materialStagingView.getSize(),
			},
			{
				.sourceOffset = emissiveTriangleIndexStagingView.getOffset(),
				.destinationOffset = 0,
				.size = emissiveTriangleIndexStagingView.getSize(),
			},
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, argumentBuffer, bufferCopyRegions[0]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegions[1]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, bufferCopyRegions[2]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, context.materialBuffer, bufferCopyRegions[3]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, context.emissiveTriangleIndexBuffer, bufferCopyRegions[4]);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		const BaseBufferView bufferViews[] = {
			argumentBuffer->createView<Arguments>(0, argumentBuffer->getSize()),
			vertexBuffer->createView<BuildVertex3D>(0, vertexBuffer->getSize()),
			indexBuffer->createView<uint32>(0, indexBuffer->getSize()),
			context.materialBuffer->createView<LightmapMaterial>(0, context.materialBuffer->getSize()),
			context.emissiveTriangleIndexBuffer->createView<uint32>(0, context.emissiveTriangleIndexBuffer->getSize()),
		};

		const DescriptorImage descriptorImages[] = {
			{
				.imageView = context.gBufferPositionImageView,
				.imageLayout = ImageLayout::GENERAL,
				.sampler = nullptr,
			},
			{
				.imageView = context.gBufferSmoothPositionImageView,
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

		const WriteDescriptorSet writeDescriptorSets[] = {
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 0,
				.descriptorBufferViews = bufferViews[1],
			},
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 1,
				.descriptorBufferViews = bufferViews[2],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 0,
				.accelerationStructures = topLevelAccelerationStructure,
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 1,
				.descriptorBufferViews = bufferViews[0],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 2,
				.descriptorBufferViews = bufferViews[3],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 3,
				.descriptorBufferViews = bufferViews[4],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 4,
				.descriptorImages = descriptorImages[0],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 5,
				.descriptorImages = descriptorImages[1],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 6,
				.descriptorImages = descriptorImages[2],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 7,
				.descriptorImages = descriptorImages[3],
			},
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 8,
				.descriptorImages = descriptorImages[4],
			},
			{
				.descriptorSet = descriptorSets[2],
				.bindingIndex = 0,
				.descriptorImages = descriptorImages[4],
			},
			{
				.descriptorSet = descriptorSets[3],
				.bindingIndex = 0,
				.descriptorImages = descriptorImages[5],
			},
		};

		device->updateDescriptorSets(writeDescriptorSets);
	}

	void Lightmapper::generateLightmaps(uint32 indexCount) const {
		const float32 width = static_cast<float32>(imageAssetDescriptor.extent.width);
		const float32 height = static_cast<float32>(imageAssetDescriptor.extent.height);
		const uint32 workGroupX = ceil(imageAssetDescriptor.extent.width / 16, 16u);
		const uint32 workGroupY = ceil(imageAssetDescriptor.extent.height / 16, 16u);

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<ExtendedCommandEncoder<EXT_hardware_ray_tracing>> commandEncoder = queue->createCommandEncoder<EXT_hardware_ray_tracing>({
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
				{0.0f, 0.0f, 0.0f, 0.0f},
			},
		});
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, context.gBufferFramebuffer->getExtent());
		commandEncoder->bindGraphicsPipeline(gBufferPipeline);
		commandEncoder->bindDescriptorSets(gBufferPipelineLayout, 0, descriptorSets[0]);
		commandEncoder->draw(indexCount, 25, 0, 0);
		commandEncoder->endRenderPass();

		// TODO(graphics/render-pass/subpass): Add support for subpass dependencies.
		{
			const ImageMemoryBarrier imageMemoryBarriers[5] = {
				{
					.image = context.gBufferPositionImage,
					.newLayout = ImageLayout::GENERAL,
					.newAccess = Access::SHADER_READ,
				},
				{
					.image = context.gBufferSmoothPositionImage,
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

			commandEncoder->insertMemoryBarriers({}, view<ImageMemoryBarrier>(imageMemoryBarriers, 5));
		}

		commandEncoder->bindPipeline(rayTracingPipeline);
		commandEncoder->bindDescriptorSets(rayTracingPipelineLayout, 0, descriptorSets[1]);
		commandEncoder->traceRays(rayGenerationShaderBindingTable, rayMissShaderBindingTable, rayHitShaderBindingTable, rayCallableShaderBindingTable, imageAssetDescriptor.extent);

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
		if (adapter->getApi() == GraphicsAPI::DX12) {
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