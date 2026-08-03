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
		vec3<float32> position;
	};

	class ForwardRenderer : public BaseSceneRenderer<ForwardCamera> {
		public:
			static constexpr index DEBUG_MAX_CONTACTS = 16;

		public:
			void postBuild() override {
				renderPass = createRenderPass();

				createFrameObjects();

				staticGeometryDescriptorSetLayout = createStaticGeometryDescriptorSetLayout();
				forwardDescriptorSetLayout = createForwardDescriptorSetLayout();

				const ref<DescriptorSetLayout> descriptorSetLayouts[] = {
					staticGeometryDescriptorSetLayout,
					forwardDescriptorSetLayout,
				};

				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(view<ref<DescriptorSetLayout>>(descriptorSetLayouts, 2));

				staticGeometryDescriptorSet = descriptorSets[0];
				forwardDescriptorSet = descriptorSets[1];

				debugHullWireframePipelineLayout = createDebugHullWireframePipelineLayout();
				debugHullWireframePipeline = createDebugHullWireframePipeline();

				debugContactManifoldPipelineLayout = createDebugContactManifoldPipelineLayout();
				debugContactManifoldPipeline = createDebugContactManifoldPipeline();

				debugContactManifoldBuffer = createDebugContactManifoldBuffer();

				const ref<BufferView<svec3<float32>>> debugContactManifoldView = debugContactManifoldBuffer->createView<svec3<float32>>(0, debugContactManifoldBuffer->getSize());

				debugContactRange = debugContactManifoldView->map();

				lightmapAtlasSampler = createLightmapAtlasSampler();
				materialSampler = createMaterialSampler();
			}

			void encodeSceneOneTimeCommands() override {
				createMaterials();

				const ref<Scene3DAsset> sceneAsset = scene->getAsset();

				vertexBuffer = createVertexBuffer();
				lightmapCoordinateBuffer = createLightmapCoordinateBuffer();
				instanceBuffer = createInstanceBuffer();

				// TODO(geometry/3d/vertex-position-indices): Separate hull line indices and vertex position indices for TRIANGLE_LIST.
				/* if (sceneAsset->hullLineIndexCount > 0) {
					debugHullWireframeBuffer = createDebugHullWireframeBuffer(sceneAsset->hullLineIndexCount * sizeof(uint16));
				} */

				QueueFamilyCapability queueFamily = QueueFamilyCapability::TRANSFER;
				uint64 stagingBufferSize = sceneAsset->geometryRanges[0].count;

				if (sceneAsset->lightmapAtlasImageAsset != nullptr) {
					lightmapAtlasImage = createLightmapAtlasImage(sceneAsset->lightmapAtlasImageAsset);
					lightmapAtlasImageView = createLightmapAtlasImageView();

					queueFamily = QueueFamilyCapability::GRAPHICS;
					stagingBufferSize += sceneAsset->geometryRanges[4].count + lightmapAtlasImage->getSize();
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

				const ref<BufferView<uint8>> vertexView = stagingBuffer->createView<uint8>(0, sceneAsset->geometryRanges[0].count);

				std::copy_n(&sceneAsset->geometryData[sceneAsset->geometryRanges[0].offset], vertexView->getSize(), vertexView->map());

				ref<BufferView<uint8>> lightmapCoordinateView;
				ref<BufferView<uint8>> lightmapAtlasView;

				if (sceneAsset->lightmapAtlasImageAsset != nullptr) {
					lightmapCoordinateView = stagingBuffer->createView<uint8>(vertexView->getOffset() + vertexView->getSize(), sceneAsset->geometryRanges[4].count);

					std::copy_n(&sceneAsset->geometryData[sceneAsset->geometryRanges[4].offset], lightmapCoordinateView->getSize(), lightmapCoordinateView->map());

					lightmapAtlasView = stagingBuffer->createView<uint8>(lightmapCoordinateView->getOffset() + lightmapCoordinateView->getSize(), lightmapAtlasImage->getSize());

					const view<uint8> data = sceneAsset->lightmapAtlasImageAsset->getData();

					std::copy(data.begin(), data.end(), lightmapAtlasView->map());
				}

				ref<BufferView<uint8>> hullLinevertexView;

				/* if (sceneAsset->hullLineIndexCount > 0) {
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

				BufferCopyRegion bufferCopyRegion = {
					.sourceOffset = vertexView->getOffset(),
					.destinationOffset = 0,
					.size = vertexView->getSize(),
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegion);

				if (sceneAsset->lightmapAtlasImageAsset != nullptr) {
					bufferCopyRegion = {
						.sourceOffset = lightmapCoordinateView->getOffset(),
						.destinationOffset = 0,
						.size = lightmapCoordinateView->getSize(),
					};

					commandEncoder->copyBufferToBuffer(stagingBuffer, lightmapCoordinateBuffer, bufferCopyRegion);

					ImageMemoryBarrier imageMemoryBarrier = {
						.image = lightmapAtlasImage,
						.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
						.newAccess = Access::TRANSFER_WRITE,
					};

					commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

					const BufferImageCopyRegion bufferImageCopyRegion = {
						.bufferOffset = lightmapAtlasView->getOffset(),
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
					const ref<BufferView<Instance3D>> instanceView = instanceBuffer->createView<Instance3D>(0, instanceBuffer->getSize());

					instanceMappedRange = instanceView->map();

					std::copy(sceneAsset->instances.begin(), sceneAsset->instances.end(), instanceMappedRange);
				}

				updateGeometryDescriptorSets();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const ref<Framebuffer> framebuffer = framebuffers[getSwapchainImageIndex()];
				const Extent2D extent = framebuffer->getExtent();
				const float32 width = static_cast<float32>(extent.width);
				const float32 height = static_cast<float32>(extent.height);
				const float32* skyColor = scene->getAsset()->skyColor;

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffer,
					.clearValues = {
						{skyColor[0], skyColor[1], skyColor[2], 1.0f},
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
			ref<DescriptorSetLayout> createForwardDescriptorSetLayout() const;

			ref<PipelineLayout> createDebugHullWireframePipelineLayout() const;
			ref<PipelineLayout> createDebugContactManifoldPipelineLayout() const;

			ref<GraphicsPipeline> createDebugHullWireframePipeline() const;
			ref<GraphicsPipeline> createDebugContactManifoldPipeline() const;

			ref<Image> createColorImage() const;
			ref<Image> createDepthStencilImage() const;
			ref<Image> createLightmapAtlasImage(ref<ImageAsset> imageAsset) const;

			ref<ImageView> createColorImageView() const;
			ref<ImageView> createDepthStencilImageView() const;
			ref<ImageView> createLightmapAtlasImageView() const;

			ref<Buffer> createVertexBuffer() const;
			ref<Buffer> createLightmapCoordinateBuffer() const;
			ref<Buffer> createInstanceBuffer() const;
			ref<Buffer> createDebugHullWireframeBuffer(uint64 size) const;
			ref<Buffer> createDebugContactManifoldBuffer() const;

			ref<Sampler> createMaterialSampler() const;
			ref<Sampler> createLightmapAtlasSampler() const;

			GraphicsPipelineDescriptor getStaticPipelineDescriptor() const;
			GraphicsPipelineDescriptor getDynamicPipelineDescriptor() const;

			void updateGeometryDescriptorSets() const;

			void createMaterialObjects(
				uint32 materialShaderIndex,
				PipelineLayoutDescriptor& pipelineLayoutDescriptor,
				GraphicsPipelineDescriptor& pipelineDescriptor,
				AssetLibrary& assetLibrary,
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
			ref<DescriptorSetLayout> forwardDescriptorSetLayout;

			ref<DescriptorSet> staticGeometryDescriptorSet;
			ref<DescriptorSet> forwardDescriptorSet;
			sequence<ref<DescriptorSet>> materialDescriptorSets;

			sequence<ref<PipelineLayout>> materialPipelineLayouts;
			ref<PipelineLayout> debugHullWireframePipelineLayout;
			ref<PipelineLayout> debugContactManifoldPipelineLayout;

			sequence<ref<GraphicsPipeline>> materialPipelines;
			ref<GraphicsPipeline> debugHullWireframePipeline;
			ref<GraphicsPipeline> debugContactManifoldPipeline;

			/// Contains vertices for static and dynamic geometry.
			ref<Buffer> vertexBuffer;

			/// Contains lightmap coordinates for static geometry.
			ref<Buffer> lightmapCoordinateBuffer;

			/// Contains world and normal transforms for static and dynamic geometry.
			ref<Buffer> instanceBuffer;

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