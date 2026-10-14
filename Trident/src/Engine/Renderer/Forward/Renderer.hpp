// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/3D/PerspectiveCamera3D.hpp"
#include "Engine/API/Geometry/Instance.hpp"
#include "Engine/Renderer/Scene/BaseSceneRenderer.hpp"
#include "Engine/Renderer/VisualizationMode.hpp"

namespace Trident {
	#define FORWARD_SAMPLE_COUNT 4
	// #define FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP
	#define FORWARD_ENABLE_VERTEX_COLOR_GI

	struct ForwardCamera {
		mat4x4<float32> projectionView;
		vec3<float32> position;
		uint32 pointLightCount;
		vec3<float32> ambientColor;
		float32 farPlane;
		vec3<float32> sunColor;
		uint32 _0;
		mat4x4<float32> skyboxProjectionView;
		mat4x4<float32> view;
	};

	class ForwardRenderer : public BaseSceneRenderer<ForwardCamera> {
		private:
			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				struct GIVertex {
					vec3<float32> position;
					uint32 _;
				};
			#endif

		private:
			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				static inline const vec4<float32> FRUSTUM_VERTICES[8] = {
					{-1.0f, -1.0f, -1.0f,  1.0f},
					{-1.0f, -1.0f,  1.0f,  1.0f},
					{-1.0f,  1.0f, -1.0f,  1.0f},
					{-1.0f,  1.0f,  1.0f,  1.0f},
					{ 1.0f, -1.0f, -1.0f,  1.0f},
					{ 1.0f, -1.0f,  1.0f,  1.0f},
					{ 1.0f,  1.0f, -1.0f,  1.0f},
					{ 1.0f,  1.0f,  1.0f,  1.0f},
				};
			#endif

		public:
			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				static constexpr Extent2D DIRECTIONAL_SHADOW_MAP_EXTENT = {4096, 4096};
				static constexpr uint32 DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT = 3;
				static constexpr float32 DIRECTIONAL_SHADOW_MAP_CASCADE_FAR_PLANES[DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT] = {0.2f, 0.5f, 1.0f};
			#endif

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				static constexpr uint32 MAX_GI_VERTEX_COUNT = 32;
			#endif

			static constexpr index DEBUG_MAX_CONTACTS = 16;

		private:
			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				struct DirectionalLight {
					mat4x4<float32> projectionViews[DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT];
				};
			#endif

		public:
			void postBuild() override {
				renderPass = createRenderPass();

				#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
					directionalShadowMapRenderPass = createDirectionalShadowMapRenderPass();
				#endif

				skyboxDescriptorSetLayout = createSkyboxDescriptorSetLayout();
				sceneDescriptorSetLayout = createSceneDescriptorSetLayout();
				lightmapAtlasDescriptorSetLayout = createLightmapAtlasDescriptorSetLayout();

				#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
					directionalShadowMapDescriptorSetLayout = createDirectionalShadowMapDescriptorSetLayout();
				#endif

				#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
					giVertexGenerationDescriptorSetLayout = createGIVertexGenerationDescriptorSetLayout();
				#endif

				const ref<DescriptorSetLayout> descriptorSetLayouts[] = {
					skyboxDescriptorSetLayout,
					sceneDescriptorSetLayout,
					lightmapAtlasDescriptorSetLayout,

					#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
						directionalShadowMapDescriptorSetLayout,
						directionalShadowMapDescriptorSetLayout,
						directionalShadowMapDescriptorSetLayout,
					#endif

					#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
						giVertexGenerationDescriptorSetLayout,
					#endif
				};

				// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayouts);

				skyboxDescriptorSet = descriptorSets[0];
				sceneDescriptorSet = descriptorSets[1];
				lightmapAtlasDescriptorSet = descriptorSets[2];

				#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
					directionalShadowMapCascadeDescriptorSets[0] = descriptorSets[3];
					directionalShadowMapCascadeDescriptorSets[1] = descriptorSets[4];
					directionalShadowMapCascadeDescriptorSets[2] = descriptorSets[5];
				#endif

				#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
					giVertexGenerationDescriptorSet = descriptorSets[3];
				#endif

				skyboxPipelineLayout = createSkyboxPipelineLayout();
				skyboxPipeline = createSkyboxPipeline();

				#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
					directionalShadowMapPipelineLayout = createDirectionalShadowMapPipelineLayout();
					directionalShadowMapPipeline = createDirectionalShadowMapPipeline();
					directionalLightBuffer = createDirectionalLightBuffer();
					directionalShadowMapImage = createDirectionalShadowMapImage();
					directionalShadowMapImageView = createDirectionalShadowMapImageView();

					uint32 cascadeIndex = 0;

					while (cascadeIndex < DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT) {
						directionalShadowMapCascadeImageViews[cascadeIndex] = createDirectionalShadowMapCascadeImageView(cascadeIndex);

						cascadeIndex += 1;
					}
				#endif

				#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
					giVertexGenerationPipelineLayout = createGIVertexGenerationPipelineLayout();
					giVertexGenerationPipeline = createGIVertexGenerationPipeline();
					giVertexBuffer = createGIVertexBuffer();

					giShadingPipelineLayout = createGIShadingPipelineLayout();
					giShadingPipeline = createGIShadingPipeline();
				#endif

				// TODO(renderer/forward): Restore debug visualization modes.
				/* debugHullWireframePipelineLayout = createDebugHullWireframePipelineLayout();
				debugHullWireframePipeline = createDebugHullWireframePipeline();

				debugContactManifoldPipelineLayout = createDebugContactManifoldPipelineLayout();
				debugContactManifoldPipeline = createDebugContactManifoldPipeline();

				debugContactManifoldBuffer = createDebugContactManifoldBuffer();

				const BufferView<vec3<float32>> debugContactManifoldView = debugContactManifoldBuffer->createView<vec3<float32>>(0, debugContactManifoldBuffer->getSize());

				debugContactRange = debugContactManifoldView->map(); */

				defaultSampler = createDefaultSampler();
				lightmapAtlasSampler = createLightmapAtlasSampler();
				materialSampler = createMaterialSampler();

				createFrameObjects();

				#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
					createDirectionalShadowMapFramebuffers();
				#endif

				#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
					const BufferView<DirectionalLight> directionalLightView = directionalLightBuffer->createView<DirectionalLight>(0, sizeof(DirectionalLight));

					directionalLightMappedRange = directionalLightView.map();
				#endif
			}

			void encodeSceneOneTimeCommands() override {
				createMaterials();

				const ref<Scene3DAsset> sceneAsset = scene->getAsset();

				vertexBuffer = createVertexBuffer();
				indexBuffer = createIndexBuffer();
				instanceBuffer = createInstanceBuffer();
				pointLightBuffer = createPointLightBuffer();

				pointLightBufferView = pointLightBuffer->createView<PointLight>(0, pointLightBuffer->getSize());

				// TODO(geometry/3d/vertex-position-indices): Separate hull line indices and vertex position indices for TRIANGLE_LIST.
				/* if (sceneAsset->hullLineIndexCount > 0) {
					debugHullWireframeBuffer = createDebugHullWireframeBuffer(sceneAsset->hullLineIndexCount * sizeof(uint16));
				} */

				uint64 stagingBufferSize = (
					sceneAsset->vertexRange.count +
					sceneAsset->indexRange.count +
					sceneAsset->lightmapCoordinateRange.count +
					sceneAsset->pointLights.count() * sizeof(PointLight)
				);

				if (sceneAsset->skyboxImageAsset != nullptr) {
					skyboxImage = createSkyboxImage(sceneAsset->skyboxImageAsset);
					skyboxImageView = createSkyboxImageView();

					stagingBufferSize += skyboxImage->getSize();
				}

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

				const BufferView<uint8> vertexView = stagingBuffer->createView<uint8>(offset, sceneAsset->vertexRange.count);

				std::copy_n(&sceneAsset->geometryData[sceneAsset->vertexRange.offset], vertexView.getSize(), vertexView.map());

				const BufferView<uint8> indexView = stagingBuffer->createView<uint8>(vertexView.getOffset() + vertexView.getSize(), sceneAsset->indexRange.count);

				std::copy_n(&sceneAsset->geometryData[sceneAsset->indexRange.offset], indexView.getSize(), indexView.map());

				offset = indexView.getOffset() + indexView.getSize();

				BufferView<PointLight> pointLightView;

				if (sceneAsset->pointLights.count() > 0) {
					pointLightView = stagingBuffer->createView<PointLight>(offset, sceneAsset->pointLights.count() * sizeof(PointLight));

					std::copy(sceneAsset->pointLights.begin(), sceneAsset->pointLights.end(), pointLightView.map());

					offset = pointLightView.getOffset() + pointLightView.getSize();
				}

				BufferView<uint8> skyboxView;

				if (sceneAsset->skyboxImageAsset != nullptr) {
					skyboxView = stagingBuffer->createView<uint8>(offset, skyboxImage->getSize());

					const view<uint8> data = sceneAsset->skyboxImageAsset->getData();

					if (sceneAsset->skyboxImageAsset->getFormat() == ImageFormat::R8G8B8_UNORM) {
						convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(data, skyboxView.map(), skyboxView.getSize());
					}
					else {
						std::copy(data.begin(), data.end(), skyboxView.map());
					}

					offset = skyboxView.getOffset() + skyboxView.getSize();
				}

				BufferView<uint8> lightmapCoordinateView;
				BufferView<uint8> lightmapAtlasView;

				if (sceneAsset->lightmapAtlasImageAsset != nullptr) {
					lightmapCoordinateView = stagingBuffer->createView<uint8>(offset, sceneAsset->lightmapCoordinateRange.count);

					std::copy_n(&sceneAsset->geometryData[sceneAsset->lightmapCoordinateRange.offset], lightmapCoordinateView.getSize(), lightmapCoordinateView.map());

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
						.name = "One-time commands",
					#endif
				});

				commandEncoder->begin();

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

				if (sceneAsset->skyboxImageAsset != nullptr) {
					ImageMemoryBarrier imageMemoryBarrier = {
						.image = skyboxImage,
						.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
						.newAccess = Access::TRANSFER_WRITE,
					};

					commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

					const BufferImageCopyRegion bufferImageCopyRegion = {
						.bufferOffset = skyboxView.getOffset(),
						.imageOffset = {0, 0, 0},
						.imageExtent = skyboxImage->getExtent(),
						.imageMipLevel = skyboxImageView->getMipLevelOffset(),
						.imageLayerOffset = skyboxImageView->getLayerOffset(),
						.imageLayerCount = skyboxImageView->getLayerCount(),
					};

					commandEncoder->copyBufferToImage(stagingBuffer, skyboxImage, bufferImageCopyRegion);

					imageMemoryBarrier = {
						.image = skyboxImage,
						.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.newAccess = Access::SHADER_READ,
					};

					commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
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
						.imageOffset = {0, 0, 0},
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

				#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
					updateGIDescriptorSets();
				#endif

				cameraMappedRange->pointLightCount = static_cast<uint32>(sceneAsset->pointLights.count());
				cameraMappedRange->ambientColor = sceneAsset->skyColor;
				cameraMappedRange->sunColor = sceneAsset->sunColor;
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

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<RenderPass> createDirectionalShadowMapRenderPass() const;
			#endif

			void createFramebuffers();

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				void createDirectionalShadowMapFramebuffers();
			#endif

			ref<DescriptorSetLayout> createSkyboxDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createSceneDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createLightmapAtlasDescriptorSetLayout() const;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<DescriptorSetLayout> createDirectionalShadowMapDescriptorSetLayout() const;
			#endif

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				ref<DescriptorSetLayout> createGIVertexGenerationDescriptorSetLayout() const;
			#endif

			ref<PipelineLayout> createSkyboxPipelineLayout() const;
			ref<PipelineLayout> createDebugHullWireframePipelineLayout() const;
			ref<PipelineLayout> createDebugContactManifoldPipelineLayout() const;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<PipelineLayout> createDirectionalShadowMapPipelineLayout() const;
			#endif

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				ref<PipelineLayout> createGIVertexGenerationPipelineLayout() const;
				ref<PipelineLayout> createGIShadingPipelineLayout() const;
			#endif

			ref<GraphicsPipeline> createSkyboxPipeline() const;
			ref<GraphicsPipeline> createDebugHullWireframePipeline() const;
			ref<GraphicsPipeline> createDebugContactManifoldPipeline() const;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<GraphicsPipeline> createDirectionalShadowMapPipeline() const;
			#endif

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				ref<ComputePipeline> createGIVertexGenerationPipeline() const;
				ref<GraphicsPipeline> createGIShadingPipeline() const;
			#endif

			#if FORWARD_SAMPLE_COUNT > 1
				ref<Image> createColorImage() const;
			#endif

			ref<Image> createSkyboxImage(ref<ImageAsset> imageAsset) const;
			ref<Image> createDepthStencilImage() const;
			ref<Image> createLightmapAtlasImage(ref<ImageAsset> imageAsset) const;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<Image> createDirectionalShadowMapImage() const;
			#endif

			#if FORWARD_SAMPLE_COUNT > 1
				ref<ImageView> createColorImageView() const;
			#endif

			ref<ImageView> createSkyboxImageView() const;
			ref<ImageView> createDepthStencilImageView() const;
			ref<ImageView> createLightmapAtlasImageView() const;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<ImageView> createDirectionalShadowMapImageView() const;
				ref<ImageView> createDirectionalShadowMapCascadeImageView(uint32 layerOffset) const;
			#endif

			ref<Buffer> createVertexBuffer() const;
			ref<Buffer> createIndexBuffer() const;
			ref<Buffer> createLightmapCoordinateBuffer() const;
			ref<Buffer> createInstanceBuffer() const;
			ref<Buffer> createPointLightBuffer() const;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<Buffer> createDirectionalLightBuffer() const;
			#endif

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				ref<Buffer> createGIVertexBuffer() const;
			#endif

			ref<Buffer> createDebugHullWireframeBuffer(uint64 size) const;
			ref<Buffer> createDebugContactManifoldBuffer() const;

			ref<Sampler> createDefaultSampler() const;
			ref<Sampler> createMaterialSampler() const;
			ref<Sampler> createLightmapAtlasSampler() const;

			GraphicsPipelineDescriptor getStaticPipelineDescriptor(VertexInputBinding* vertexInputBindings) const;
			GraphicsPipelineDescriptor getDynamicPipelineDescriptor(VertexInputBinding* vertexInputBindings) const;

			void updateGeometryDescriptorSets() const;

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				void updateGIDescriptorSets() const;
			#endif

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

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				void initializeLightProjectionViews() const;
			#endif

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				void encodeDirectionalShadowMapPass(ref<CommandEncoder> commandEncoder) const;
			#endif

			void encodeSkyboxPass(ref<CommandEncoder> commandEncoder) const;

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				void encodeGIVertexGenerationPass(ref<CommandEncoder> commandEncoder) const;
				void encodeGIShadingPass(ref<CommandEncoder> commandEncoder) const;
			#endif

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				/// Calculates world-space frustum vertices for the shadow map cascades.
				void calculateFrustum(vec4<float32>* vertices, const mat4x4<float32>& inverseProjectionView) const;
			#endif

		private:
			ref<RenderPass> renderPass;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<RenderPass> directionalShadowMapRenderPass;
			#endif

			sequence<ref<Framebuffer>> framebuffers;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				sequence<ref<Framebuffer>> directionalShadowMapFramebuffers;
			#endif

			ref<DescriptorSetLayout> skyboxDescriptorSetLayout;
			ref<DescriptorSetLayout> sceneDescriptorSetLayout;
			ref<DescriptorSetLayout> lightmapAtlasDescriptorSetLayout;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<DescriptorSetLayout> directionalShadowMapDescriptorSetLayout;
			#endif

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				ref<DescriptorSetLayout> giVertexGenerationDescriptorSetLayout;
			#endif

			ref<DescriptorSet> skyboxDescriptorSet;
			ref<DescriptorSet> sceneDescriptorSet;
			ref<DescriptorSet> lightmapAtlasDescriptorSet;
			sequence<ref<DescriptorSet>> materialDescriptorSets;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<DescriptorSet> directionalShadowMapCascadeDescriptorSets[DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT];
			#endif

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				ref<DescriptorSet> giVertexGenerationDescriptorSet;
			#endif

			ref<PipelineLayout> skyboxPipelineLayout;
			sequence<ref<PipelineLayout>> materialPipelineLayouts;
			ref<PipelineLayout> debugHullWireframePipelineLayout;
			ref<PipelineLayout> debugContactManifoldPipelineLayout;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<PipelineLayout> directionalShadowMapPipelineLayout;
			#endif

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				ref<PipelineLayout> giVertexGenerationPipelineLayout;
				ref<PipelineLayout> giShadingPipelineLayout;
			#endif

			ref<GraphicsPipeline> skyboxPipeline;
			sequence<ref<GraphicsPipeline>> materialPipelines;
			ref<GraphicsPipeline> debugHullWireframePipeline;
			ref<GraphicsPipeline> debugContactManifoldPipeline;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<GraphicsPipeline> directionalShadowMapPipeline;
			#endif

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				ref<ComputePipeline> giVertexGenerationPipeline;
				ref<GraphicsPipeline> giShadingPipeline;
			#endif

			/// Contains vertices for static and dynamic geometry.
			ref<Buffer> vertexBuffer;

			/// Contains index for static and dynamic geometry.
			ref<Buffer> indexBuffer;

			/// Contains lightmap coordinates for static geometry.
			ref<Buffer> lightmapCoordinateBuffer;

			/// Contains world and normal transforms for static and dynamic geometry.
			ref<Buffer> instanceBuffer;

			ref<Buffer> pointLightBuffer;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<Buffer> directionalLightBuffer;
			#endif

			#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
				ref<Buffer> giVertexBuffer;
			#endif

			ref<Buffer> debugHullWireframeBuffer;
			ref<Buffer> debugContactManifoldBuffer;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<Image> colorImage;
			#endif

			ref<Image> skyboxImage;
			ref<Image> depthStencilImage;
			ref<Image> lightmapAtlasImage;
			sequence<ref<Image>> materialImages;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<Image> directionalShadowMapImage;
			#endif

			#if FORWARD_SAMPLE_COUNT > 1
				ref<ImageView> colorImageView;
			#endif

			ref<ImageView> skyboxImageView;
			ref<ImageView> depthStencilImageView;
			ref<ImageView> lightmapAtlasImageView;
			sequence<ref<ImageView>> materialImageViews;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				ref<ImageView> directionalShadowMapImageView;
				ref<ImageView> directionalShadowMapCascadeImageViews[DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT];
			#endif

			ref<Sampler> defaultSampler;
			ref<Sampler> lightmapAtlasSampler;
			ref<Sampler> materialSampler;

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				DirectionalLight* directionalLightMappedRange = nullptr;
			#endif

			BufferView<PointLight> pointLightBufferView;

			Instance3D* instanceMappedRange = nullptr;

		public:
			vec3<float32>* debugContactRange = nullptr;
			uint32 contactCount = 0;
			VisualizationMode visualizationMode;
	};
}