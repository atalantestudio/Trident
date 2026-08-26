// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/3D/PerspectiveCamera3D.hpp"
#include "Engine/API/Geometry/Index.hpp"
#include "Engine/Renderer/Scene/BaseSceneRenderer.hpp"
#include "Engine/Renderer/VisualizationMode.hpp"

namespace Trident {
	#define FORWARD_SAMPLE_COUNT 4

	struct ForwardCamera {
		mat4x4<float32> projectionView;
		svec3<float32> position;
		uint32 pointLightCount;
		svec3<float32> ambientColor;
		uint32 _0;
		svec3<float32> sunColor;
		uint32 _1;
		mat4x4<float32> skyboxProjectionView;
	};

	class ForwardRenderer : public BaseSceneRenderer<ForwardCamera> {
		private:
			struct DirectionalLight {
				mat4x4<float32> projectionView;
			};

		public:
			static constexpr Extent2D DIRECTIONAL_SHADOW_MAP_EXTENT = {1024, 1024};
			static constexpr index DEBUG_MAX_CONTACTS = 16;

		public:
			void postBuild() override {
				renderPass = createRenderPass();
				directionalShadowMapRenderPass = createDirectionalShadowMapRenderPass();
				skyboxDescriptorSetLayout = createSkyboxDescriptorSetLayout();
				sceneDescriptorSetLayout = createSceneDescriptorSetLayout();
				lightmapAtlasDescriptorSetLayout = createLightmapAtlasDescriptorSetLayout();
				directionalShadowMapDescriptorSetLayout = createDirectionalShadowMapDescriptorSetLayout();

				const ref<DescriptorSetLayout> descriptorSetLayouts[] = {
					skyboxDescriptorSetLayout,
					sceneDescriptorSetLayout,
					lightmapAtlasDescriptorSetLayout,
					directionalShadowMapDescriptorSetLayout,
				};

				// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayouts);

				skyboxDescriptorSet = descriptorSets[0];
				sceneDescriptorSet = descriptorSets[1];
				lightmapAtlasDescriptorSet = descriptorSets[2];
				directionalShadowMapDescriptorSet = descriptorSets[3];

				skyboxPipelineLayout = createSkyboxPipelineLayout();
				skyboxPipeline = createSkyboxPipeline();
				skyboxImage = createSkyboxImage();
				skyboxImageView = createSkyboxImageView();

				directionalShadowMapPipelineLayout = createDirectionalShadowMapPipelineLayout();
				directionalShadowMapPipeline = createDirectionalShadowMapPipeline();
				directionalLightBuffer = createDirectionalLightBuffer();
				directionalShadowMapImage = createDirectionalShadowMapImage();
				directionalShadowMapImageView = createDirectionalShadowMapImageView();

				// TODO(forward): Restore debug modes.
				/* debugHullWireframePipelineLayout = createDebugHullWireframePipelineLayout();
				debugHullWireframePipeline = createDebugHullWireframePipeline();

				debugContactManifoldPipelineLayout = createDebugContactManifoldPipelineLayout();
				debugContactManifoldPipeline = createDebugContactManifoldPipeline();

				debugContactManifoldBuffer = createDebugContactManifoldBuffer();

				const BufferView<svec3<float32>> debugContactManifoldView = debugContactManifoldBuffer->createView<svec3<float32>>(0, debugContactManifoldBuffer->getSize());

				debugContactRange = debugContactManifoldView->map(); */

				lightmapAtlasSampler = createLightmapAtlasSampler();
				materialSampler = createMaterialSampler();
				directionalShadowMapSampler = createDirectionalShadowMapSampler();

				createFrameObjects();
				createDirectionalShadowMapFramebuffers();

				const BufferView<DirectionalLight> directionalLightView = directionalLightBuffer->createView<DirectionalLight>(0, sizeof(DirectionalLight));

				directionalLightMappedRange = directionalLightView.map();
			}

			void encodeSceneOneTimeCommands() override {
				createMaterials();

				const ref<Scene3DAsset> sceneAsset = scene->getAsset();

				vertexBuffer = createVertexBuffer();
				indexBuffer = createIndexBuffer();
				instanceBuffer = createInstanceBuffer();
				pointLightBuffer = createPointLightBuffer();

				// TODO(geometry/3d/vertex-position-indices): Separate hull line indices and vertex position indices for TRIANGLE_LIST.
				/* if (sceneAsset->hullLineIndexCount > 0) {
					debugHullWireframeBuffer = createDebugHullWireframeBuffer(sceneAsset->hullLineIndexCount * sizeof(uint16));
				} */

				uint64 stagingBufferSize = (
					skyboxImage->getSize() +
					sceneAsset->geometryRanges[0].count +
					sceneAsset->geometryRanges[1].count +
					sceneAsset->geometryRanges[2].count +
					sceneAsset->pointLights.count() * sizeof(PointLight)
				);

				if (sceneAsset->lightmapAtlasImageAsset != nullptr) {
					lightmapCoordinateBuffer = createLightmapCoordinateBuffer();
					lightmapAtlasImage = createLightmapAtlasImage(sceneAsset->lightmapAtlasImageAsset);
					lightmapAtlasImageView = createLightmapAtlasImageView();

					stagingBufferSize += lightmapAtlasImage->getSize();
				}

				const ref<Buffer> stagingBuffer = createDynamicBuffer({
					.size = stagingBufferSize,
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});

				uint64 offset = 0;

				const BufferView<uint8> skyboxView = stagingBuffer->createView<uint8>(0, skyboxImage->getSize());
				const ref<ImageAsset> skyboxImageAsset = getLayer()->getApplication()->getAssetLibrary()->getAsset<ImageAsset>("evening_road_01_2k");
				const view<uint8> skyboxData = skyboxImageAsset->getData();

				if (skyboxImageAsset->getFormat() == ImageFormat::R8G8B8_UNORM) {
					convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(skyboxData, skyboxView.map(), skyboxView.getSize());
				}
				else {
					std::copy(skyboxData.begin(), skyboxData.end(), skyboxView.map());
				}

				const BufferView<uint8> vertexView = stagingBuffer->createView<uint8>(skyboxView.getOffset() + skyboxView.getSize(), sceneAsset->geometryRanges[0].count);

				std::copy_n(&sceneAsset->geometryData[sceneAsset->geometryRanges[0].offset], vertexView.getSize(), vertexView.map());

				const BufferView<uint8> indexView = stagingBuffer->createView<uint8>(vertexView.getOffset() + vertexView.getSize(), sceneAsset->geometryRanges[1].count);

				std::copy_n(&sceneAsset->geometryData[sceneAsset->geometryRanges[1].offset], indexView.getSize(), indexView.map());

				offset = indexView.getOffset() + indexView.getSize();

				BufferView<PointLight> pointLightView;

				if (sceneAsset->pointLights.count() > 0) {
					pointLightView = stagingBuffer->createView<PointLight>(offset, sceneAsset->pointLights.count() * sizeof(PointLight));

					std::copy(sceneAsset->pointLights.begin(), sceneAsset->pointLights.end(), pointLightView.map());

					offset = pointLightView.getOffset() + pointLightView.getSize();
				}

				BufferView<uint8> lightmapCoordinateView;
				BufferView<uint8> lightmapAtlasView;

				if (sceneAsset->lightmapAtlasImageAsset != nullptr) {
					lightmapCoordinateView = stagingBuffer->createView<uint8>(offset, sceneAsset->geometryRanges[2].count);

					std::copy_n(&sceneAsset->geometryData[sceneAsset->geometryRanges[2].offset], lightmapCoordinateView.getSize(), lightmapCoordinateView.map());

					lightmapAtlasView = stagingBuffer->createView<uint8>(lightmapCoordinateView.getOffset() + lightmapCoordinateView.getSize(), lightmapAtlasImage->getSize());

					const view<uint8> data = sceneAsset->lightmapAtlasImageAsset->getData();

					std::copy(data.begin(), data.end(), lightmapAtlasView.map());
				}

				/* BufferView<uint8> hullLineIndexView;

				if (sceneAsset->hullLineIndexCount > 0) {
					hullLineIndexView = stagingBuffer->createView<uint8>(viewOffset, sceneAsset->hullLineIndexCount * sizeof(uint16));
					viewOffset += hullLineIndexView->getSize();

					std::copy_n(&sourceDataBuffer[scene->getHullLineIndexByteOffset()], hullLineIndexView->getSize(), hullLineIndexView->map());
				} */

				stagingBuffer->unmap();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Initialize scene resources",
					#endif
				});

				commandEncoder->begin();

				const BufferImageCopyRegion skyboxImageCopyRegion = {
					.bufferOffset = skyboxView.getOffset(),
					.imageOffsetX = 0,
					.imageOffsetY = 0,
					.imageOffsetZ = 0,
					.imageExtent = skyboxImage->getExtent(),
					.imageMipLevel = skyboxImageView->getMipLevelOffset(),
					.imageLayerOffset = skyboxImageView->getLayerOffset(),
					.imageLayerCount = skyboxImageView->getLayerCount(),
				};

				ImageMemoryBarrier skyboxImageMemoryBarrier = {
					.image = skyboxImage,
					.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
					.newAccess = Access::TRANSFER_WRITE,
				};

				commandEncoder->insertMemoryBarriers({}, skyboxImageMemoryBarrier);
				commandEncoder->copyBufferToImage(stagingBuffer, skyboxImage, skyboxImageCopyRegion);

				skyboxImageMemoryBarrier = {
					.image = skyboxImage,
					.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({}, skyboxImageMemoryBarrier);

				const BufferCopyRegion bufferCopyRegions[] = {
					{
						.sourceOffset = vertexView.getOffset(),
						.destinationOffset = 0,
						.size = vertexView.getSize(),
					},
					{
						.sourceOffset = indexView.getOffset(),
						.destinationOffset = 0,
						.size = indexView.getSize(),
					},
					{
						.sourceOffset = pointLightView.getOffset(),
						.destinationOffset = 0,
						.size = pointLightView.getSize(),
					},
					{
						.sourceOffset = lightmapCoordinateView.getOffset(),
						.destinationOffset = 0,
						.size = lightmapCoordinateView.getSize(),
					},
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegions[0]);
				commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, bufferCopyRegions[1]);

				if (sceneAsset->pointLights.count() > 0) {
					commandEncoder->copyBufferToBuffer(stagingBuffer, pointLightBuffer, bufferCopyRegions[2]);
				}

				if (sceneAsset->lightmapAtlasImageAsset != nullptr) {
					commandEncoder->copyBufferToBuffer(stagingBuffer, lightmapCoordinateBuffer, bufferCopyRegions[3]);

					ImageMemoryBarrier imageMemoryBarrier = {
						.image = lightmapAtlasImage,
						.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
						.newAccess = Access::TRANSFER_WRITE,
					};

					commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

					const BufferImageCopyRegion bufferImageCopyRegion = {
						.bufferOffset = lightmapAtlasView.getOffset(),
						.imageOffsetX = 0,
						.imageOffsetY = 0,
						.imageOffsetZ = 0,
						.imageExtent = lightmapAtlasImage->getExtent(),
						.imageMipLevel = lightmapAtlasImageView->getMipLevelOffset(),
						.imageLayerOffset = lightmapAtlasImageView->getLayerOffset(),
						.imageLayerCount = lightmapAtlasImageView->getLayerCount(),
					};

					commandEncoder->copyBufferToImage(stagingBuffer, lightmapAtlasImage, bufferImageCopyRegion);

					imageMemoryBarrier = {
						.image = lightmapAtlasImage,
						.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.newAccess = Access::SHADER_READ,
					};

					commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
				}

				/* if (sceneAsset->hullLineIndexCount > 0) {
					bufferCopyRegion = {
						.sourceOffset = hullLineIndexView->getOffset(),
						.destinationOffset = 0,
						.size = hullLineIndexView->getSize(),
					};

					commandEncoder->copyBufferToBuffer(stagingBuffer, debugHullWireframeBuffer, bufferCopyRegion);
				} */

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();

				{
					const BufferView<Instance3D> instanceView = instanceBuffer->createView<Instance3D>(0, instanceBuffer->getSize());

					instanceMappedRange = instanceView.map();

					std::copy(sceneAsset->instances.begin(), sceneAsset->instances.end(), instanceMappedRange);
				}

				updateGeometryDescriptorSets();

				cameraMappedRange->pointLightCount = static_cast<uint32>(sceneAsset->pointLights.count());
				cameraMappedRange->ambientColor = sceneAsset->skyColor;
				cameraMappedRange->sunColor = sceneAsset->sunColor;

				initializeLightProjectionView();
			}

			void render(ref<CommandEncoder> commandEncoder) const override;

			void setInstanceWorldTransform(Entity entity, const mat4x4<float32>& worldTransform) const {
				instanceMappedRange[entity].worldTransform = worldTransform;
			}

			void setInstanceNormalTransform(Entity entity, const mat3x3<float32>& normalTransform) const {
				instanceMappedRange[entity].normalTransform = normalTransform;
			}

			void createFrameObjects() override;

			void updateCamera() override;

		private:
			ref<RenderPass> createRenderPass() const;
			ref<RenderPass> createDirectionalShadowMapRenderPass() const;

			void createFramebuffers();
			void createDirectionalShadowMapFramebuffers();

			ref<DescriptorSetLayout> createSkyboxDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createSceneDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createLightmapAtlasDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createDirectionalShadowMapDescriptorSetLayout() const;

			ref<PipelineLayout> createSkyboxPipelineLayout() const;
			ref<PipelineLayout> createDebugHullWireframePipelineLayout() const;
			ref<PipelineLayout> createDebugContactManifoldPipelineLayout() const;
			ref<PipelineLayout> createDirectionalShadowMapPipelineLayout() const;

			ref<GraphicsPipeline> createSkyboxPipeline() const;
			ref<GraphicsPipeline> createDebugHullWireframePipeline() const;
			ref<GraphicsPipeline> createDebugContactManifoldPipeline() const;
			ref<GraphicsPipeline> createDirectionalShadowMapPipeline() const;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<Image> createColorImage() const;
			#endif

			ref<Image> createSkyboxImage() const;
			ref<Image> createDepthStencilImage() const;
			ref<Image> createLightmapAtlasImage(ref<ImageAsset> imageAsset) const;
			ref<Image> createDirectionalShadowMapImage() const;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<ImageView> createColorImageView() const;
			#endif

			ref<ImageView> createSkyboxImageView() const;
			ref<ImageView> createDepthStencilImageView() const;
			ref<ImageView> createLightmapAtlasImageView() const;
			ref<ImageView> createDirectionalShadowMapImageView() const;

			ref<Buffer> createVertexBuffer() const;
			ref<Buffer> createIndexBuffer() const;
			ref<Buffer> createLightmapCoordinateBuffer() const;
			ref<Buffer> createInstanceBuffer() const;
			ref<Buffer> createPointLightBuffer() const;
			ref<Buffer> createDirectionalLightBuffer() const;
			ref<Buffer> createDebugHullWireframeBuffer(uint64 size) const;
			ref<Buffer> createDebugContactManifoldBuffer() const;

			ref<Sampler> createMaterialSampler() const;
			ref<Sampler> createLightmapAtlasSampler() const;
			ref<Sampler> createDirectionalShadowMapSampler() const;

			GraphicsPipelineDescriptor getStaticPipelineDescriptor(VertexInputBinding* vertexInputBindings) const;
			GraphicsPipelineDescriptor getDynamicPipelineDescriptor(VertexInputBinding* vertexInputBindings) const;

			void updateGeometryDescriptorSets() const;

			void createMaterialObjects(
				uint32 materialShaderIndex,
				PipelineLayoutDescriptor& pipelineLayoutDescriptor,
				GraphicsPipelineDescriptor& pipelineDescriptor,
				AssetLibrary& assetLibrary,
				sequence<ref<DescriptorSetLayout>>& uniqueMaterialDescriptorSetLayouts,
				sequence<ref<DescriptorSetLayout>>& materialDescriptorSetLayouts,
				uint32& writeDescriptorSetCount
			);

			ref<Image> createMaterialImage(ref<ImageAsset> imageAsset, ImageDescriptor& imageDescriptor) const;

			void createMaterials();

			void initializeLightProjectionView() const;

			void encodeDirectionalShadowMapPass(ref<CommandEncoder> commandEncoder) const;
			void encodeSkyboxPass(ref<CommandEncoder> commandEncoder) const;

		private:
			ref<RenderPass> renderPass;
			ref<RenderPass> directionalShadowMapRenderPass;

			sequence<ref<Framebuffer>> framebuffers;
			sequence<ref<Framebuffer>> directionalShadowMapFramebuffers;

			ref<DescriptorSetLayout> skyboxDescriptorSetLayout;
			ref<DescriptorSetLayout> sceneDescriptorSetLayout;
			ref<DescriptorSetLayout> lightmapAtlasDescriptorSetLayout;
			ref<DescriptorSetLayout> directionalShadowMapDescriptorSetLayout;

			ref<DescriptorSet> skyboxDescriptorSet;
			ref<DescriptorSet> sceneDescriptorSet;
			ref<DescriptorSet> lightmapAtlasDescriptorSet;
			sequence<ref<DescriptorSet>> materialDescriptorSets;
			ref<DescriptorSet> directionalShadowMapDescriptorSet;

			ref<PipelineLayout> skyboxPipelineLayout;
			sequence<ref<PipelineLayout>> materialPipelineLayouts;
			ref<PipelineLayout> debugHullWireframePipelineLayout;
			ref<PipelineLayout> debugContactManifoldPipelineLayout;
			ref<PipelineLayout> directionalShadowMapPipelineLayout;

			ref<GraphicsPipeline> skyboxPipeline;
			sequence<ref<GraphicsPipeline>> materialPipelines;
			ref<GraphicsPipeline> debugHullWireframePipeline;
			ref<GraphicsPipeline> debugContactManifoldPipeline;
			ref<GraphicsPipeline> directionalShadowMapPipeline;

			/// Contains vertices for static and dynamic geometry.
			ref<Buffer> vertexBuffer;

			/// Contains index for static and dynamic geometry.
			ref<Buffer> indexBuffer;

			/// Contains lightmap coordinates for static geometry.
			ref<Buffer> lightmapCoordinateBuffer;

			/// Contains world and normal transforms for static and dynamic geometry.
			ref<Buffer> instanceBuffer;

			ref<Buffer> pointLightBuffer;
			ref<Buffer> directionalLightBuffer;
			ref<Buffer> debugHullWireframeBuffer;
			ref<Buffer> debugContactManifoldBuffer;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<Image> colorImage;
			#endif

			ref<Image> skyboxImage;
			ref<Image> depthStencilImage;
			ref<Image> lightmapAtlasImage;
			sequence<ref<Image>> materialImages;
			ref<Image> directionalShadowMapImage;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<ImageView> colorImageView;
			#endif

			ref<ImageView> skyboxImageView;
			ref<ImageView> depthStencilImageView;
			ref<ImageView> lightmapAtlasImageView;
			sequence<ref<ImageView>> materialImageViews;
			ref<ImageView> directionalShadowMapImageView;

			ref<Sampler> lightmapAtlasSampler;
			ref<Sampler> materialSampler;
			ref<Sampler> directionalShadowMapSampler;

			DirectionalLight* directionalLightMappedRange = nullptr;
			Instance3D* instanceMappedRange = nullptr;

		public:
			svec3<float32>* debugContactRange = nullptr;
			uint32 contactCount = 0;

			VisualizationMode visualizationMode;
	};
}