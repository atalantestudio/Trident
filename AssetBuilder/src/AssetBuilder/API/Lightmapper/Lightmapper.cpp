// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Lightmapper/ArgumentBuffer.hpp"
#include "AssetBuilder/API/Lightmapper/BilateralFilterPipelines.hpp"
#include "AssetBuilder/API/Lightmapper/BLAS.hpp"
#include "AssetBuilder/API/Lightmapper/Dilation.hpp"
#include "AssetBuilder/API/Lightmapper/DilationDescriptorSetLayout.hpp"
#include "AssetBuilder/API/Lightmapper/DilationPipelineLayout.hpp"
#include "AssetBuilder/API/Lightmapper/EnvironmentMap.hpp"
#include "AssetBuilder/API/Lightmapper/EnvironmentMapSampler.hpp"
#include "AssetBuilder/API/Lightmapper/GBuffer.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferDescriptorSetLayout.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferFramebuffer.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferPipeline.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferPipelineLayout.hpp"
#include "AssetBuilder/API/Lightmapper/GBufferRenderPass.hpp"
#include "AssetBuilder/API/Lightmapper/IntermediateDilationPipeline.hpp"
#include "AssetBuilder/API/Lightmapper/IntermediateLightmapAtlasImage.hpp"
#include "AssetBuilder/API/Lightmapper/IntermediateLightmapAtlasSampler.hpp"
#include "AssetBuilder/API/Lightmapper/LightmapAtlasImage.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"
#include "AssetBuilder/API/Lightmapper/MaterialBuffer.hpp"
#include "AssetBuilder/API/Lightmapper/PostProcessingDescriptorSetLayout.hpp"
#include "AssetBuilder/API/Lightmapper/PostProcessingPipelineLayout.hpp"
#include "AssetBuilder/API/Lightmapper/RayTracingDescriptorSetLayout.hpp"
#include "AssetBuilder/API/Lightmapper/RayTracingPipeline.hpp"
#include "AssetBuilder/API/Lightmapper/RayTracingPipelineLayout.hpp"
#include "AssetBuilder/API/Lightmapper/ShaderBindingTableBuffer.hpp"
#include "AssetBuilder/API/Lightmapper/ShaderModule.hpp"
#include "AssetBuilder/API/Lightmapper/TLAS.hpp"
#include "AssetBuilder/API/Lightmapper/UpdateDescriptorSets.hpp"

namespace Trident {
	void Lightmapper::initialize(const GraphicsAdapter* adapter) {
		this->adapter = adapter;

		instance = adapter->createInstance({
			.projectName = "Lightmapper",
			.projectVersion = {0, 0, 1},
		});
		physicalDevice = instance->selectPhysicalDevice(false);
		limits = &physicalDevice->getLimits();
		device = physicalDevice->createDevice<EXT_hardware_ray_tracing>();
		queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		commandEncoder = queue->createCommandEncoder<EXT_hardware_ray_tracing>({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "One-time commands",
			#endif
		});

		gBufferRenderPass = createGBufferRenderPass();
		gBufferDescriptorSetLayout = createGBufferDescriptorSetLayout();
		rayTracingDescriptorSetLayout = createRayTracingDescriptorSetLayout();
		postProcessingDescriptorSetLayout = createPostProcessingDescriptorSetLayout();
		dilationDescriptorSetLayout = createDilationDescriptorSetLayout();
		gBufferPipelineLayout = createGBufferPipelineLayout();
		rayTracingPipelineLayout = createRayTracingPipelineLayout();
		postProcessingPipelineLayout = createPostProcessingPipelineLayout();
		dilationPipelineLayout = createDilationPipelineLayout();
		gBufferPipelines[0] = createGBufferPipeline(PolygonMode::FILL);
		gBufferPipelines[1] = createGBufferPipeline(PolygonMode::LINE);
		rayTracingPipeline = createRayTracingPipeline();
		intermediateDilationPipeline = createIntermediateDilationPipeline();
		horizontalBilateralFilterPipeline = createHorizontalBilateralFilterPipeline();
		verticalBilateralFilterPipeline = createVerticalBilateralFilterPipeline();
		argumentBuffer = createArgumentBuffer();
		intermediateLightmapAtlasSampler = createIntermediateLightmapAtlasSampler();

		descriptorSets = device->allocateDescriptorSets(descriptorSetLayouts);

		createShaderBindingTableBuffer();
	}

	Lightmapper::~Lightmapper() {
		gBufferDescriptorSetLayout.~ref();
		rayTracingDescriptorSetLayout.~ref();
		postProcessingDescriptorSetLayout.~ref();
		dilationDescriptorSetLayout.~ref();
	}

	void Lightmapper::generateLightmapAtlas(const LightmapperDescriptor& descriptor) {
		// TODO(asset-builder/lightmapper): Don't use an image asset descriptor to initialize the lightmapper.
		imageAssetDescriptor = &descriptor.scene->staticGeometry.lightmapAtlasImageAssetDescriptor;

		if (!initialized) {
			dilationRenderPass = createDilationRenderPass();
			dilationPipeline = createDilationPipeline();

			initialized = true;
		}

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

		const BuildScene3D& scene = *descriptor.scene;

		vertexBuffer = device->createDynamicBuffer({
			.size = ceilPowerOf2((scene.staticGeometry.bakedVertexCount + scene.staticGeometry.externalVertexCount) * sizeof(LightmapVertex), limits->minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::READ_ONLY_ACCELERATION_STRUCTURE_BUILD_INPUT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex",
			#endif
		});
		lightmapCoordinateBuffer = device->createDynamicBuffer({
			.size = ceilPowerOf2(scene.staticGeometry.lightmapCoordinates.count() * sizeof(vec2<float32>), limits->minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap coordinate",
			#endif
		});
		triangleBuffer = device->createDynamicBuffer({
			.size = ceilPowerOf2(descriptor.triangles.count() * sizeof(BuildTriangle3D), limits->minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Emissive triangle",
			#endif
		});
		emissiveTriangleIndexBuffer = device->createDynamicBuffer({
			.size = ceilPowerOf2(descriptor.emissiveTriangleIndices.count() * sizeof(uint32), limits->minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Emissive triangle index",
			#endif
		});
		indexBuffer = device->createDynamicBuffer({
			.size = ceilPowerOf2((scene.staticGeometry.bakedIndexCount + scene.staticGeometry.externalIndexCount) * sizeof(uint32), limits->minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::READ_ONLY_ACCELERATION_STRUCTURE_BUILD_INPUT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Index",
			#endif
		});
		instanceBuffer = device->createDynamicBuffer({
			.size = ceilPowerOf2(scene.staticGeometry.instances.count() * sizeof(LightmapInstance), limits->minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Instance",
			#endif
		});
		materialBuffer = createMaterialBuffer(descriptor.materials.count());
		tlasInstanceBuffer = device->createDynamicBuffer({
			.size = ceilPowerOf2(scene.staticGeometry.instances.count() * sizeof(AccelerationStructureInstance), 256ull),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::READ_ONLY_ACCELERATION_STRUCTURE_BUILD_INPUT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "TLAS instance",
			#endif
		});
		lightmapAtlasStagingBuffer = device->createDynamicBuffer({
			.size = context.lightmapAtlasImage->getSize(),
			.usage = BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas staging",
			#endif
		});

		if (scene.environmentMapImageAsset != nullptr) {
			environmentMapImage = createEnvironmentMapImage(scene.environmentMapImageAsset->getExtent());
		}
		else {
			environmentMapImage = createEnvironmentMapImage({1, 1, 1});
		}

		environmentMapImageView = createEnvironmentMapImageView();
		environmentMapSampler = createEnvironmentMapSampler();

		createBottomLevelAccelerationStructures(scene);
		createTopLevelAccelerationStructure(scene);

		encodeOneTimeCommands(descriptor);
		updateDescriptorSets();
		generateLightmaps(scene);

		const BufferView<uint8> lightmapAtlasView = lightmapAtlasStagingBuffer->createView<uint8>(0, lightmapAtlasStagingBuffer->getSize());

		std::copy_n(lightmapAtlasView.map(), imageAssetDescriptor->data.count(), &imageAssetDescriptor->data[0]);

		lightmapAtlasStagingBuffer->unmap();
	}

	void Lightmapper::encodeOneTimeCommands(const LightmapperDescriptor& descriptor) const {
		const BuildScene3D& scene = *descriptor.scene;
		const uint64 argumentStagingViewSize = sizeof(Arguments);
		const uint64 vertexStagingViewSize = (scene.staticGeometry.bakedVertexCount + scene.staticGeometry.externalVertexCount) * sizeof(LightmapVertex);
		const uint64 lightmapCoordinateStagingViewSize = scene.staticGeometry.lightmapCoordinates.count() * sizeof(vec2<float32>);
		const uint64 indexStagingViewSize = (scene.staticGeometry.bakedIndexCount + scene.staticGeometry.externalIndexCount) * sizeof(uint32);
		const uint64 emissiveTriangleIndexStagingViewSize = descriptor.emissiveTriangleIndices.count() * sizeof(uint32);
		const uint64 triangleStagingViewSize = descriptor.triangles.count() * sizeof(BuildTriangle3D);
		const uint64 instanceStagingViewSize = scene.staticGeometry.instances.count() * sizeof(LightmapInstance);
		const uint64 materialStagingViewSize = materialBuffer->getSize();
		const uint64 tlasInstanceStagingViewSize = scene.staticGeometry.instances.count() * sizeof(AccelerationStructureInstance);
		const uint64 environmentMapStagingViewSize = environmentMapImage->getSize();

		const ref<Buffer> stagingBuffer = device->createDynamicBuffer({
			.size = (
				argumentStagingViewSize +
				vertexStagingViewSize +
				lightmapCoordinateStagingViewSize +
				indexStagingViewSize +
				emissiveTriangleIndexStagingViewSize +
				triangleStagingViewSize +
				instanceStagingViewSize +
				materialStagingViewSize +
				tlasInstanceStagingViewSize +
				environmentMapStagingViewSize
			),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});

		uint64 viewOffset = 0;

		const BufferView<Arguments> argumentStagingView = stagingBuffer->createView<Arguments>(viewOffset, argumentStagingViewSize);
		viewOffset += argumentStagingViewSize;

		const BufferView<LightmapVertex> vertexStagingView = stagingBuffer->createView<LightmapVertex>(viewOffset, vertexStagingViewSize);
		viewOffset += vertexStagingViewSize;

		const BufferView<vec2<float32>> lightmapCoordinateStagingView = stagingBuffer->createView<vec2<float32>>(viewOffset, lightmapCoordinateStagingViewSize);
		viewOffset += lightmapCoordinateStagingViewSize;

		const BufferView<uint32> indexStagingView = stagingBuffer->createView<uint32>(viewOffset, indexStagingViewSize);
		viewOffset += indexStagingViewSize;

		const BufferView<uint32> emissiveTriangleIndexStagingView = stagingBuffer->createView<uint32>(viewOffset, emissiveTriangleIndexStagingViewSize);
		viewOffset += emissiveTriangleIndexStagingViewSize;

		const BufferView<BuildTriangle3D> triangleStagingView = stagingBuffer->createView<BuildTriangle3D>(viewOffset, triangleStagingViewSize);
		viewOffset += triangleStagingViewSize;

		const BufferView<LightmapInstance> instanceStagingView = stagingBuffer->createView<LightmapInstance>(viewOffset, instanceStagingViewSize);
		viewOffset += instanceStagingViewSize;

		const BufferView<LightmapMaterial> materialStagingView = stagingBuffer->createView<LightmapMaterial>(viewOffset, materialStagingViewSize);
		viewOffset += materialStagingViewSize;

		const BufferView<AccelerationStructureInstance> tlasInstanceStagingView = stagingBuffer->createView<AccelerationStructureInstance>(viewOffset, tlasInstanceStagingViewSize);
		viewOffset += tlasInstanceStagingViewSize;

		const BufferView<uint8> environmentMapStagingView = stagingBuffer->createView<uint8>(viewOffset, environmentMapStagingViewSize);
		viewOffset += environmentMapStagingViewSize;

		Arguments* arguments = argumentStagingView.map();
		arguments->atlasSize = {imageAssetDescriptor->extent.x, imageAssetDescriptor->extent.y};
		arguments->emissiveTriangleCount = static_cast<uint32>(descriptor.emissiveTriangleIndices.count() - 1);

		LightmapVertex* vertexMappedRange = vertexStagingView.map();
		uint32 vertexIndex = 0;

		while (vertexIndex < scene.staticGeometry.bakedVertexCount + scene.staticGeometry.externalVertexCount) {
			const BuildVertex3D& vertex = scene.vertices[vertexIndex];

			LightmapVertex& lightmapVertex = vertexMappedRange[vertexIndex];
			lightmapVertex.position = vertex.position;
			lightmapVertex.normal = vertex.normal;

			vertexIndex += 1;
		}

		std::copy(scene.staticGeometry.lightmapCoordinates.begin(), scene.staticGeometry.lightmapCoordinates.end(), lightmapCoordinateStagingView.map());
		std::copy_n(&scene.indices[0], scene.staticGeometry.bakedIndexCount + scene.staticGeometry.externalIndexCount, indexStagingView.map());
		std::copy(descriptor.emissiveTriangleIndices.begin(), descriptor.emissiveTriangleIndices.end(), emissiveTriangleIndexStagingView.map());
		std::copy(descriptor.triangles.begin(), descriptor.triangles.end(), triangleStagingView.map());

		// Copy instances.
		{
			LightmapInstance* instanceMappedRange = instanceStagingView.map();
			AccelerationStructureInstance* tlasInstanceMappedRange = tlasInstanceStagingView.map();
			uint32 materialShaderIndex = 0;

			while (materialShaderIndex < scene.dynamicGeometry.materialShaderOffset) {
				const BuildMaterialShader& materialShader = scene.materialShaders[materialShaderIndex];

				uint32 materialIndex = 0;

				while (materialIndex < materialShader.materialRange.count) {
					const BuildMaterial& material = scene.materials[materialShader.materialRange.offset + materialIndex];

					uint32 materialGeometryIndex = 0;

					while (materialGeometryIndex < material.materialGeometryRange.count) {
						const MaterialGeometry& materialGeometry = scene.materialGeometries[material.materialGeometryRange.offset + materialGeometryIndex];
						const BuildGeometry& geometry = scene.geometries[materialGeometry.geometryIndex];
						const uint64 blasAddress = blas[materialGeometry.geometryIndex]->getDeviceAddress();

						uint32 triangleOffset = geometry.indexRange.offset / 3;
						uint32 lightmapCoordinateOffset = 0;
						uint32 instanceOffset = 0;

						while (instanceOffset < materialGeometry.instanceRange.count) {
							const uint32 instanceIndex = materialGeometry.instanceRange.offset + instanceOffset;

							const BuildTransform3D& transform = scene.staticGeometry.instances[instanceIndex].transform;
							const quat<float32> orientation = normalized(
								axisAngle<float32>(UP, transform.rotation[1]) *
								axisAngle<float32>(RIGHT, transform.rotation[0]) *
								axisAngle<float32>(FORWARD, transform.rotation[2])
							);
							const mat4x4<float32> world = transpose(
								// TODO(lyah/uniform-scaling)
								scaling<mat4x4<float32>>(vec3<float32>(transform.scale)) *

								mat4x4<float32>(orientation) *
								translation<mat4x4<float32>>(transform.translation)
							);

							instanceMappedRange[instanceIndex] = {
								.transform = {
									{world[0][0], world[0][1], world[0][2], world[0][3]},
									{world[1][0], world[1][1], world[1][2], world[1][3]},
									{world[2][0], world[2][1], world[2][2], world[2][3]},
								},
								.geometryTriangleOffset = geometry.indexRange.offset / 3,
								.deduplicatedTriangleOffset = triangleOffset,
								.lightmapCoordinateOffset = lightmapCoordinateOffset,
							};

							tlasInstanceMappedRange[instanceIndex] = {
								.transform = {
									{world[0][0], world[0][1], world[0][2], world[0][3]},
									{world[1][0], world[1][1], world[1][2], world[1][3]},
									{world[2][0], world[2][1], world[2][2], world[2][3]},
								},
								.index = instanceIndex,
								.mask = 1,
								.shaderBindingTableRecordOffset = 0,
								.flags = InstanceFlags::FORCE_OPAQUE,
								.accelerationStructureAddress = blasAddress,
							};

							triangleOffset += geometry.indexRange.count / 3;
							lightmapCoordinateOffset += geometry.vertexRange.count;
							instanceOffset += 1;
						}

						materialGeometryIndex += 1;
					}

					materialIndex += 1;
				}

				materialShaderIndex += 1;
			}
		}

		std::copy(descriptor.materials.begin(), descriptor.materials.end(), materialStagingView.map());

		if (scene.environmentMapImageAsset != nullptr) {
			const view<uint8> data = scene.environmentMapImageAsset->getData();

			if (scene.environmentMapImageAsset->getFormat() == ImageFormat::R8G8B8_UNORM) {
				convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(data, environmentMapStagingView.map(), environmentMapStagingView.getSize());
			}
			else {
				std::copy(data.begin(), data.end(), environmentMapStagingView.map());
			}
		}
		else {
			const uint32 color = (
				static_cast<uint32>(scene.skyColor.x * 255.0f) |
				(static_cast<uint32>(scene.skyColor.y * 255.0f) << 8) |
				(static_cast<uint32>(scene.skyColor.z * 255.0f) << 16) |
				(255 << 24)
			);

			std::fill_n(pointer_cast<uint32*>(environmentMapStagingView.map()), 6, color);
		}

		stagingBuffer->unmap();

		commandEncoder->reset();
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
				.sourceOffset = lightmapCoordinateStagingView.getOffset(),
				.destinationOffset = 0,
				.size = lightmapCoordinateStagingView.getSize(),
			},
			{
				.sourceOffset = indexStagingView.getOffset(),
				.destinationOffset = 0,
				.size = indexStagingView.getSize(),
			},
			{
				.sourceOffset = emissiveTriangleIndexStagingView.getOffset(),
				.destinationOffset = 0,
				.size = emissiveTriangleIndexStagingView.getSize(),
			},
			{
				.sourceOffset = triangleStagingView.getOffset(),
				.destinationOffset = 0,
				.size = triangleStagingView.getSize(),
			},
			{
				.sourceOffset = instanceStagingView.getOffset(),
				.destinationOffset = 0,
				.size = instanceStagingView.getSize(),
			},
			{
				.sourceOffset = materialStagingView.getOffset(),
				.destinationOffset = 0,
				.size = materialStagingView.getSize(),
			},
			{
				.sourceOffset = tlasInstanceStagingView.getOffset(),
				.destinationOffset = 0,
				.size = tlasInstanceStagingView.getSize(),
			},
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, argumentBuffer, bufferCopyRegions[0]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegions[1]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, lightmapCoordinateBuffer, bufferCopyRegions[2]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, bufferCopyRegions[3]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, emissiveTriangleIndexBuffer, bufferCopyRegions[4]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, triangleBuffer, bufferCopyRegions[5]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, bufferCopyRegions[6]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, materialBuffer, bufferCopyRegions[7]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, tlasInstanceBuffer, bufferCopyRegions[8]);

		ImageMemoryBarrier imageMemoryBarriers[] = {
			// Environment map
			{
				.image = environmentMapImage,
				.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
				.newAccess = Access::TRANSFER_WRITE,
			},

			// Intermediate lightmap atlas
			{
				.image = context.intermediateLightmapAtlasImage,
				.newLayout = ImageLayout::GENERAL,
				.newAccess = Access::SHADER_WRITE,
			},
		};

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

		const BufferImageCopyRegion bufferImageCopyRegions[] = {
			// Environment map
			{
				.bufferOffset = environmentMapStagingView.getOffset(),
				.imageOffsetX = 0,
				.imageOffsetY = 0,
				.imageOffsetZ = 0,
				.imageExtent = environmentMapImage->getExtent(),
				.imageMipLevel = environmentMapImageView->getMipLevelOffset(),
				.imageLayerOffset = environmentMapImageView->getLayerOffset(),
				.imageLayerCount = environmentMapImageView->getLayerCount(),
			},
		};

		commandEncoder->copyBufferToImage(stagingBuffer, environmentMapImage, bufferImageCopyRegions[0]);

		imageMemoryBarriers[0].newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
		imageMemoryBarriers[0].newAccess = Access::SHADER_READ;

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers[0]);

		queue->submit(commandEncoder->end());
		queue->waitIdle();

		commandEncoder->reset();
		commandEncoder->begin();
		commandEncoder->buildAccelerationStructures(blas, blasScratchViews);

		queue->submit(commandEncoder->end());
		queue->waitIdle();

		commandEncoder->reset();
		commandEncoder->begin();
		commandEncoder->buildAccelerationStructures(tlas, tlasScratchView);

		queue->submit(commandEncoder->end());
		queue->waitIdle();
	}

	void Lightmapper::encodeGBufferCommands(const BuildScene3D& scene) const {
		commandEncoder->bindDescriptorSets(gBufferPipelineLayout, 0, descriptorSets[0]);

		uint32 materialShaderIndex = 0;

		while (materialShaderIndex < scene.dynamicGeometry.materialShaderOffset) {
			const BuildMaterialShader& materialShader = scene.materialShaders[materialShaderIndex];

			uint32 materialIndex = 0;

			while (materialIndex < materialShader.materialRange.count) {
				const BuildMaterial material = scene.materials[materialShader.materialRange.offset + materialIndex];

				uint32 materialGeometryIndex = 0;

				while (materialGeometryIndex < material.materialGeometryRange.count) {
					const MaterialGeometry materialGeometry = scene.materialGeometries[material.materialGeometryRange.offset + materialGeometryIndex];
					const BuildGeometry& geometry = scene.geometries[materialGeometry.geometryIndex];

					commandEncoder->draw(geometry.indexRange.count, materialGeometry.instanceRange.count, geometry.indexRange.offset, materialGeometry.instanceRange.offset);

					materialGeometryIndex += 1;
				}

				materialIndex += 1;
			}

			materialShaderIndex += 1;
		}
	}

	void Lightmapper::generateLightmaps(const BuildScene3D& scene) const {
		const float32 width = static_cast<float32>(imageAssetDescriptor->extent.x);
		const float32 height = static_cast<float32>(imageAssetDescriptor->extent.y);

		commandEncoder->reset();
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
		commandEncoder->bindGraphicsPipeline(gBufferPipelines[0]);

		encodeGBufferCommands(scene);

		commandEncoder->bindGraphicsPipeline(gBufferPipelines[1]);

		encodeGBufferCommands(scene);

		// TODO(graphics/dx12/render-pass/final-layout): Automatically transition render pass attachments to their final layout.
		if (adapter->getApi() == GraphicsAPI::DX12) {
			const ImageMemoryBarrier imageMemoryBarriers[] = {
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
			};

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);
		}

		commandEncoder->endRenderPass();

		queue->submit(commandEncoder->end());
		queue->waitIdle();

		commandEncoder->reset();
		commandEncoder->begin();
		commandEncoder->bindPipeline(rayTracingPipeline);
		commandEncoder->bindDescriptorSets(rayTracingPipelineLayout, 0, descriptorSets[1]);
		commandEncoder->traceRays(rayGenerationShaderBindingTable, rayMissShaderBindingTable, rayHitShaderBindingTable, rayCallableShaderBindingTable, imageAssetDescriptor->extent);

		queue->submit(commandEncoder->end());
		queue->waitIdle();

		commandEncoder->reset();
		commandEncoder->begin();

		ImageMemoryBarrier imageMemoryBarrier = {
			.image = context.intermediateLightmapAtlasImage,
			.newLayout = ImageLayout::GENERAL,
			.newAccess = Access::SHADER_READ | Access::SHADER_WRITE,
		};

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

		const uint32 workGroupX = ceilPowerOf2(imageAssetDescriptor->extent.x / 16, 16u);
		const uint32 workGroupY = ceilPowerOf2(imageAssetDescriptor->extent.y / 16, 16u);

		#if defined(DEBUG_LIGHTMAPPER_ENABLE_DILATION)
			commandEncoder->bindComputePipeline(intermediateDilationPipeline);
			commandEncoder->bindDescriptorSets(postProcessingPipelineLayout, 0, descriptorSets[2]);
			commandEncoder->dispatch(workGroupX, workGroupY, 1);

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
		#endif

		#if defined(DEBUG_LIGHTMAPPER_ENABLE_BILATERAL_FILTER)
			commandEncoder->bindComputePipeline(horizontalBilateralFilterPipeline);
			commandEncoder->bindDescriptorSets(postProcessingPipelineLayout, 0, descriptorSets[2]);
			commandEncoder->dispatch(workGroupX, workGroupY, 1);

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

			commandEncoder->bindComputePipeline(verticalBilateralFilterPipeline);
			commandEncoder->bindDescriptorSets(postProcessingPipelineLayout, 0, descriptorSets[2]);
			commandEncoder->dispatch(workGroupX, workGroupY, 1);
		#endif

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

		// TODO(graphics/dx12/render-pass/final-layout): Automatically transition render pass attachments to their final layout.
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

		commandEncoder->copyImageToBuffer(context.lightmapAtlasImage, lightmapAtlasStagingBuffer, imageCopyRegion);

		queue->submit(commandEncoder->end());
		queue->waitIdle();
	}
}