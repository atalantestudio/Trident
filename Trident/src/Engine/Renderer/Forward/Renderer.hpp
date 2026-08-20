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
	};

	class ForwardRenderer : public BaseSceneRenderer<ForwardCamera> {
		public:
			static constexpr index DEBUG_MAX_CONTACTS = 16;

		public:
			void postBuild() override {
				renderPass = createRenderPass();

				createFrameObjects();

				staticGeometryDescriptorSetLayout = createStaticGeometryDescriptorSetLayout();
				dynamicDescriptorSetLayout = createDynamicDescriptorSetLayout();

				const ref<DescriptorSetLayout> descriptorSetLayouts[] = {
					staticGeometryDescriptorSetLayout,
					dynamicDescriptorSetLayout,
				};

				// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayouts);

				staticGeometryDescriptorSet = descriptorSets[0];
				dynamicDescriptorSet = descriptorSets[1];

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

				QueueFamilyCapability queueFamily = QueueFamilyCapability::TRANSFER;
				uint64 stagingBufferSize = sceneAsset->geometryRanges[0].count + sceneAsset->geometryRanges[1].count + sceneAsset->geometryRanges[2].count + sceneAsset->pointLights.count() * sizeof(PointLight);

				if (sceneAsset->lightmapAtlasImageAsset != nullptr) {
					lightmapCoordinateBuffer = createLightmapCoordinateBuffer();
					lightmapAtlasImage = createLightmapAtlasImage(sceneAsset->lightmapAtlasImageAsset);
					lightmapAtlasImageView = createLightmapAtlasImageView();

					queueFamily = QueueFamilyCapability::GRAPHICS;
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

				const BufferView<uint8> vertexView = stagingBuffer->createView<uint8>(0, sceneAsset->geometryRanges[0].count);

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

				const ref<Queue> queue = getDevice()->getQueue(queueFamily);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Initialize scene resources",
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

				cameraMappedRange->pointLightCount = static_cast<uint32>(scene->getAsset()->pointLights.count());
				cameraMappedRange->ambientColor = scene->getAsset()->skyColor;
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const ref<Framebuffer> framebuffer = framebuffers[getSwapchainImageIndex()];
				const Extent2D extent = framebuffer->getExtent();
				const float32 width = static_cast<float32>(extent.width);
				const float32 height = static_cast<float32>(extent.height);
				const svec3<float32> skyColor = scene->getAsset()->skyColor;

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffer,
					.clearValues = {
						{skyColor.x, skyColor.y, skyColor.z, 1.0f},
						{
							.depth = 0.0f,
							.stencil = 0,
						},
						#if FORWARD_SAMPLE_COUNT > 1
							{0.0f, 0.0f, 0.0f, 1.0f},
						#endif
					},
				});
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);

				renderScene(commandEncoder);

				commandEncoder->endRenderPass();
			}

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

			void createFramebuffers();

			ref<DescriptorSetLayout> createStaticGeometryDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createDynamicDescriptorSetLayout() const;

			ref<PipelineLayout> createDebugHullWireframePipelineLayout() const;
			ref<PipelineLayout> createDebugContactManifoldPipelineLayout() const;

			ref<GraphicsPipeline> createDebugHullWireframePipeline() const;
			ref<GraphicsPipeline> createDebugContactManifoldPipeline() const;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<Image> createColorImage() const;
			#endif

			ref<Image> createDepthStencilImage() const;
			ref<Image> createLightmapAtlasImage(ref<ImageAsset> imageAsset) const;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<ImageView> createColorImageView() const;
			#endif

			ref<ImageView> createDepthStencilImageView() const;
			ref<ImageView> createLightmapAtlasImageView() const;

			ref<Buffer> createVertexBuffer() const;
			ref<Buffer> createIndexBuffer() const;
			ref<Buffer> createLightmapCoordinateBuffer() const;
			ref<Buffer> createInstanceBuffer() const;
			ref<Buffer> createPointLightBuffer() const;
			ref<Buffer> createDebugHullWireframeBuffer(uint64 size) const;
			ref<Buffer> createDebugContactManifoldBuffer() const;

			ref<Sampler> createMaterialSampler() const;
			ref<Sampler> createLightmapAtlasSampler() const;

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

			void renderScene(ref<CommandEncoder> commandEncoder) const;

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> staticGeometryDescriptorSetLayout;
			ref<DescriptorSetLayout> dynamicDescriptorSetLayout;

			ref<DescriptorSet> staticGeometryDescriptorSet;
			ref<DescriptorSet> dynamicDescriptorSet;
			sequence<ref<DescriptorSet>> materialDescriptorSets;

			sequence<ref<PipelineLayout>> materialPipelineLayouts;
			ref<PipelineLayout> debugHullWireframePipelineLayout;
			ref<PipelineLayout> debugContactManifoldPipelineLayout;

			sequence<ref<GraphicsPipeline>> materialPipelines;
			ref<GraphicsPipeline> debugHullWireframePipeline;
			ref<GraphicsPipeline> debugContactManifoldPipeline;

			/// Contains vertices for static and dynamic geometry.
			ref<Buffer> vertexBuffer;

			/// Contains index for static and dynamic geometry.
			ref<Buffer> indexBuffer;

			/// Contains lightmap coordinates for static geometry.
			ref<Buffer> lightmapCoordinateBuffer;

			/// Contains world and normal transforms for static and dynamic geometry.
			ref<Buffer> instanceBuffer;

			ref<Buffer> pointLightBuffer;
			ref<Buffer> debugHullWireframeBuffer;
			ref<Buffer> debugContactManifoldBuffer;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<Image> colorImage;
			#endif

			ref<Image> depthStencilImage;
			ref<Image> lightmapAtlasImage;
			sequence<ref<Image>> materialImages;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<ImageView> colorImageView;
			#endif

			ref<ImageView> depthStencilImageView;
			ref<ImageView> lightmapAtlasImageView;
			sequence<ref<ImageView>> materialImageViews;

			ref<Sampler> lightmapAtlasSampler;
			ref<Sampler> materialSampler;

			Instance3D* instanceMappedRange = nullptr;

		public:
			svec3<float32>* debugContactRange = nullptr;
			uint32 contactCount = 0;

			VisualizationMode visualizationMode;
	};
}