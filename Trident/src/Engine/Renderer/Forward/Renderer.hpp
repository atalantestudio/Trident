// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/3D/PerspectiveCamera3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/MaterialComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace Trident {
	#define FORWARD_SAMPLE_COUNT 4

	class ForwardRenderer : public SceneRenderer<Scene3D> {
		private:
			struct Vertex {
				uint32 positionIndex;
				uint32 normalIndex;
				uint32 textureCoordinateIndex;
			};

			struct Camera {
				mat4x4<float32> projectionView;
				vec3<float32> position;
			};

			struct Instance {
				mat4x4<float32> world;
				mat3x3<float32> normal;
			};

		public:
			static constexpr index DEBUG_MAX_CONTACTS = 16;

		public:
			// Called on startup and on window resize.
			void initialize() {
				#if FORWARD_SAMPLE_COUNT > 1
					colorImage = createColorImage();
					colorImageView = createColorImageView();
				#endif

				depthStencilImage = createDepthStencilImage();
				depthStencilImageView = createDepthStencilImageView();

				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor = {
					.renderPass = renderPass,
					#if FORWARD_SAMPLE_COUNT > 1
						.imageViews = {colorImageView, depthStencilImageView, nullptr},
					#else
						.imageViews = {nullptr, depthStencilImageView},
					#endif
					.extent = swapchain->getExtent(),
				};

				#if defined(DEBUG_OBJECT_NAME)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

				while (framebufferIndex < framebuffers.count()) {
					#if defined(DEBUG_OBJECT_NAME)
						name = Logger::format("In flight framebuffer []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					#if FORWARD_SAMPLE_COUNT > 1
						framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);
					#else
						framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);
					#endif

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			void build() override {
				renderPass = createRenderPass();

				initialize();

				staticDescriptorSetLayout = createStaticDescriptorSetLayout();
				vertexDescriptorSetLayout = createVertexDescriptorSetLayout();

				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(sequence<ref<DescriptorSetLayout>>{
					staticDescriptorSetLayout,
					vertexDescriptorSetLayout,
				});

				staticDescriptorSet = descriptorSets[0];
				vertexDescriptorSet = descriptorSets[1];

				debugHullWireframePipelineLayout = createDebugHullWireframePipelineLayout();
				debugHullWireframePipeline = createDebugHullWireframePipeline();

				debugContactManifoldPipelineLayout = createDebugContactManifoldPipelineLayout();
				debugContactManifoldPipeline = createDebugContactManifoldPipeline();

				cameraBuffer = createCameraBuffer();
				debugContactManifoldBuffer = createDebugContactManifoldBuffer();

				const ref<BufferView<Camera>> cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());
				const ref<BufferView<svec3<float32>>> debugContactManifoldView = debugContactManifoldBuffer->createView<svec3<float32>>(0, debugContactManifoldBuffer->getSize());

				cameraMappedRange = cameraView->map();
				debugContactRange = debugContactManifoldView->map();

				imageSampler = createImageSampler();
			}

			void postSetScene() override {
				const ref<Scene3DAsset> sceneAsset = scene->getAsset();
				const ref<Device> device = getDevice();

				createMaterialObjects();

				// Write index buffer.
				const view<uint8> sourceDataBuffer = scene->getDataBuffer();
				const range<uint64, uint64> vertexPositionIndexRange = scene->getVertexPositionIndexRange();
				const range<uint64, uint64> vertexNormalIndexRange = scene->getVertexNormalIndexRange();
				const range<uint64, uint64> vertexTextureCoordinateIndexRange = scene->getVertexTextureCoordinateIndexRange();
				const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

				// TODO(geometry/3d/vertex-position-indices): Separe hull line indices and vertex position indices for TRIANGLE_LIST.
				// DEBUG_ASSERT(vertexPositionIndexRange.count == vertexNormalIndexRange.count && vertexPositionIndexRange.count == vertexTextureCoordinateIndexRange.count);
				DEBUG_ASSERT(vertexNormalIndexRange.count == vertexTextureCoordinateIndexRange.count);

				if (scene->getIndexCount() > 0) {
					vertexBuffer = createVertexBuffer(scene->getIndexCount() * sizeof(ForwardRenderer::Vertex));

					dataBuffer = createDynamicBuffer({
						.size = vertexPositionIndexRange.offset,
						.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
						.memoryProperties = MemoryProperty::DEVICE_LOCAL,
						.persistent = false,
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Data",
						#endif
					});
				}

				instanceBuffer = createInstanceBuffer(rigidBodyComponentPool.componentCount * sizeof(Instance));

				if (scene->getHullLineIndexCount() > 0) {
					debugHullWireframeBuffer = createDebugHullWireframeBuffer(scene->getHullLineIndexCount() * sizeof(uint16));
				}

				const ref<BufferView<Instance>> instanceView = instanceBuffer->createView<Instance>(0, instanceBuffer->getSize());

				instanceMappedRange = instanceView->map();

				uint32 componentIndex = 0;

				while (componentIndex < rigidBodyComponentPool.componentCount) {
					const RigidBody3DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

					Instance& instance = instanceMappedRange[componentIndex];
					instance.world = rigidBodyComponent.world;
					instance.normal = rigidBodyComponent.calculateNormal();

					componentIndex += 1;
				}

				uint64 stagingBufferSize = 0;

				if (sceneAsset->getStaticTriangles().count() > 0) {
					staticBuffer = createStaticBuffer();

					staticMaterialPipelineLayout = createStaticMaterialPipelineLayout();
					staticMaterialPipeline = createStaticMaterialPipeline();

					const ref<ImageAsset> lightmapAtlasImageAsset = sceneAsset->getLightmapAtlasImageAsset();

					stagingBufferSize += lightmapAtlasImageAsset->getSize();
				}

				if (scene->getIndexCount() > 0) {
					// TODO: As of now the hull line indices are included twice.
					stagingBufferSize += sourceDataBuffer.count();
					stagingBufferSize += scene->getHullLineIndexCount() * sizeof(uint16);
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

				ref<BufferView<uint8>> lightmapAtlasView;
				ref<BufferView<uint8>> componentView;
				ref<BufferView<uint8>> hullLineIndexView;
				ref<BufferView<Vertex>> vertexView;
				uint64 viewOffset = 0;
				BufferCopyRegion bufferCopyRegion;
				ImageMemoryBarrier imageMemoryBarrier;

				if (sceneAsset->getStaticTriangles().count() > 0) {
					const ref<ImageAsset> lightmapAtlasImageAsset = sceneAsset->getLightmapAtlasImageAsset();

					lightmapAtlasView = stagingBuffer->createView<uint8>(viewOffset, lightmapAtlasImageAsset->getSize());
					viewOffset += lightmapAtlasView->getSize();

					lightmapAtlasImage = createLightmapAtlasImage(lightmapAtlasImageAsset);
					lightmapAtlasImageView = createLightmapAtlasImageView();

					std::copy_n(&lightmapAtlasImageAsset->getData()[0], lightmapAtlasView->getSize(), lightmapAtlasView->map());
				}

				if (scene->getIndexCount() > 0) {
					componentView = stagingBuffer->createView<uint8>(viewOffset, vertexPositionIndexRange.offset);
					viewOffset += componentView->getSize();

					vertexView = stagingBuffer->createView<Vertex>(viewOffset, scene->getIndexCount() * sizeof(Vertex));
					viewOffset += vertexView->getSize();

					std::copy_n(sourceDataBuffer.begin(), componentView->getSize(), componentView->map());

					Vertex* vertexMappedRange = vertexView->map();
					index vertexIndex = 0;

					while (vertexIndex < scene->getIndexCount()) {
						Vertex& vertex = vertexMappedRange[vertexIndex];
						vertex.positionIndex = *pointer_cast<const uint32*>(&sourceDataBuffer[vertexPositionIndexRange.offset + vertexIndex * sizeof(uint32)]);
						vertex.normalIndex = *pointer_cast<const uint32*>(&sourceDataBuffer[vertexNormalIndexRange.offset + vertexIndex * sizeof(uint32)]);
						vertex.textureCoordinateIndex = *pointer_cast<const uint32*>(&sourceDataBuffer[vertexTextureCoordinateIndexRange.offset + vertexIndex * sizeof(uint32)]);

						vertexIndex += 1;
					}
				}

				if (scene->getHullLineIndexCount() > 0) {
					hullLineIndexView = stagingBuffer->createView<uint8>(viewOffset, scene->getHullLineIndexCount() * sizeof(uint16));
					viewOffset += hullLineIndexView->getSize();

					std::copy_n(&sourceDataBuffer[scene->getHullLineIndexByteOffset()], hullLineIndexView->getSize(), hullLineIndexView->map());
				}

				stagingBuffer->unmap();

				ref<Queue> queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
				ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Copy resources",
					#endif
				});
				ref<CommandBuffer> commandBuffer;

				commandEncoder->begin();

				if (sceneAsset->getStaticTriangles().count() > 0) {
					imageMemoryBarrier = {
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
				}

				if (scene->getIndexCount() > 0) {
					bufferCopyRegion = {
						.sourceOffset = componentView->getOffset(),
						.destinationOffset = 0,
						.size = componentView->getSize(),
					};

					commandEncoder->copyBufferToBuffer(stagingBuffer, dataBuffer, bufferCopyRegion);

					bufferCopyRegion = {
						.sourceOffset = vertexView->getOffset(),
						.destinationOffset = 0,
						.size = vertexView->getSize(),
					};

					commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegion);
				}

				if (scene->getHullLineIndexCount() > 0) {
					bufferCopyRegion = {
						.sourceOffset = hullLineIndexView->getOffset(),
						.destinationOffset = 0,
						.size = hullLineIndexView->getSize(),
					};

					commandEncoder->copyBufferToBuffer(stagingBuffer, debugHullWireframeBuffer, bufferCopyRegion);
				}

				commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();

				if (sceneAsset->getStaticTriangles().count() > 0) {
					queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
					commandEncoder = queue->createCommandEncoder({
						.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Transition image",
						#endif
					});

					imageMemoryBarrier = {
						.image = lightmapAtlasImage,
						.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.newAccess = Access::SHADER_READ,
					};

					commandEncoder->begin();
					commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

					commandBuffer = commandEncoder->end();

					queue->submit(commandBuffer);
					queue->waitIdle();
				}

				updateDescriptorSets();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffers[getSwapchainImageIndex()],
					.clearValues = {
						{1.0f, 0.2f, 0.0f, 1.0f},
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
				commandEncoder->setScissor(0, 0, swapchainExtent);

				if (scene->getAsset()->getStaticTriangles().count() > 0) {
					renderStaticObjects(commandEncoder);
				}

				// TODO(graphics/gl/bind-vertex-buffers): Allow vertex buffers to be bound before setting a graphics pipeline.

				const ComponentPool<GeometryComponent>& geometryComponentPool = scene->ecs.getComponentPool<GeometryComponent>();
				const ComponentPool<MaterialComponent>& materialComponentPool = scene->ecs.getComponentPool<MaterialComponent>();
				const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

				if (scene->getIndexCount() > 0 && draw) {
					sequence<ref<DescriptorSet>> descriptorSets = {vertexDescriptorSet, nullptr};
					uint32 rigidBodyComponentIndex = 0;

					while (rigidBodyComponentIndex < rigidBodyComponentPool.componentCount) {
						const Entity entity = rigidBodyComponentPool.componentIndexToEntity[rigidBodyComponentIndex];
						const GeometryComponent geometryComponent = geometryComponentPool.components[geometryComponentPool.entityToComponentIndex[entity]];
						const Geometry3D& geometry = scene->geometries[geometryComponent.geometryIndex];
						const MaterialComponent materialComponent = materialComponentPool.components[materialComponentPool.entityToComponentIndex[entity]];

						// NOTE: Material is incremented by 1 for visibility rendering.
						const uint32 materialShaderIndex = scene->getMaterialShaderIndex(materialComponent.materialIndex - 1);

						const range<uint32, uint32> indexRange = geometry.getVertexPositionIndexRange();

						descriptorSets[1] = materialDescriptorSets[materialShaderIndex];

						commandEncoder->bindGraphicsPipeline(materialPipelines[materialShaderIndex]);
						commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
						commandEncoder->bindVertexBuffer(1, instanceBuffer, 0, sizeof(Instance));
						commandEncoder->bindDescriptorSets(materialPipelineLayouts[materialShaderIndex], 0, descriptorSets);
						commandEncoder->draw(indexRange.count, 1, indexRange.offset, rigidBodyComponentIndex);

						rigidBodyComponentIndex += 1;
					}
				}

				if (scene->getHullLineIndexCount() > 0 && debugHullWireframe) {
					commandEncoder->bindGraphicsPipeline(debugHullWireframePipeline);
					commandEncoder->bindDescriptorSets(debugHullWireframePipelineLayout, 0, vertexDescriptorSet);
					commandEncoder->bindVertexBuffer(0, instanceBuffer, 0, sizeof(Instance));
					commandEncoder->bindIndexBuffer<uint16>(debugHullWireframeBuffer, 0);

					uint32 rigidBodyComponentIndex = 0;

					while (rigidBodyComponentIndex < rigidBodyComponentPool.componentCount) {
						const Entity entity = rigidBodyComponentPool.componentIndexToEntity[rigidBodyComponentIndex];
						const GeometryComponent geometryComponent = geometryComponentPool.components[geometryComponentPool.entityToComponentIndex[entity]];
						const Geometry3D& geometry = scene->geometries[geometryComponent.geometryIndex];

						if (geometry.hullIndex >= scene->hulls.count()) {
							rigidBodyComponentIndex += 1;

							continue;
						}

						const Hull3D& hull = scene->hulls[geometry.hullIndex];

						// TODO: DX12 doesn't pass the BaseVertexOffset to the vertex shader so indexing the vertex buffer doesn't return the same values as OpenGL/Vulkan.
						commandEncoder->drawIndexed(hull.lineIndexRange.count, 1, hull.lineIndexRange.offset, geometry.getVertexPositionComponentRange().offset / 3, rigidBodyComponentIndex);

						rigidBodyComponentIndex += 1;
					}
				}

				if (debugContactManifolds) {
					commandEncoder->bindGraphicsPipeline(debugContactManifoldPipeline);
					commandEncoder->bindDescriptorSets(debugContactManifoldPipelineLayout, 0, vertexDescriptorSet);
					commandEncoder->bindVertexBuffer(0, debugContactManifoldBuffer, 0, sizeof(svec3<float32>));
					commandEncoder->draw(contactCount, 1, 0, 0);
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				initialize();
				updateCamera();
			}

			void updateCamera() {
				const PerspectiveCamera3D& camera = getCameraController<PerspectiveCamera3D>().getCamera();

				cameraMappedRange->projectionView = camera.getProjectionView();
				cameraMappedRange->position = camera.position;
			}

			void setWorld(Entity entity, const mat4x4<float32>& world) const {
				instanceMappedRange[entity].world = world;
			}

		private:
			ref<RenderPass> createRenderPass() const;

			ref<DescriptorSetLayout> createStaticDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createVertexDescriptorSetLayout() const;

			ref<PipelineLayout> createDebugHullWireframePipelineLayout() const;
			ref<PipelineLayout> createDebugContactManifoldPipelineLayout() const;
			ref<PipelineLayout> createStaticMaterialPipelineLayout() const;

			ref<GraphicsPipeline> createDebugHullWireframePipeline() const;
			ref<GraphicsPipeline> createDebugContactManifoldPipeline() const;
			ref<GraphicsPipeline> createStaticMaterialPipeline() const;

			ref<Image> createColorImage() const;
			ref<Image> createDepthStencilImage() const;
			ref<Image> createLightmapAtlasImage(ref<ImageAsset> imageAsset) const;

			ref<ImageView> createColorImageView() const;
			ref<ImageView> createDepthStencilImageView() const;
			ref<ImageView> createLightmapAtlasImageView() const;

			ref<Buffer> createCameraBuffer() const;
			ref<Buffer> createInstanceBuffer(uint64 size) const;
			ref<Buffer> createVertexBuffer(uint64 size) const;
			ref<Buffer> createDebugHullWireframeBuffer(uint64 size) const;
			ref<Buffer> createDebugContactManifoldBuffer() const;
			ref<Buffer> createStaticBuffer() const;

			ref<Sampler> createImageSampler() const;

			void updateDescriptorSets() const {
				const ref<Device> device = getDevice();
				const ref<Scene3DAsset> sceneAsset = scene->getAsset();
				const uint64 minStorageBufferOffsetAlignment = device->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
				const uint64 triangleRangeSize = sceneAsset->getStaticTriangles().count() * sizeof(StaticTriangle3D);
				const uint64 vertexRangeSize = sceneAsset->getStaticVertices().count() * sizeof(svec3<float32>);
				const uint64 normalRangeSize = sceneAsset->getStaticNormals().count() * sizeof(svec3<float32>);
				const uint64 textureCoordinateRangeSize = sceneAsset->getStaticTextureCoordinates().count() * sizeof(svec2<float32>);
				const uint64 lightmapCoordinateRangeSize = sceneAsset->getStaticLightmapCoordinates().count() * sizeof(svec2<float32>);
				const uint64 triangleAlignedRangeSize = ceil(triangleRangeSize, minStorageBufferOffsetAlignment);
				const uint64 vertexAlignedRangeSize = ceil(vertexRangeSize, minStorageBufferOffsetAlignment);
				const uint64 normalAlignedRangeSize = ceil(normalRangeSize, minStorageBufferOffsetAlignment);
				const uint64 textureCoordinateAlignedRangeSize = ceil(textureCoordinateRangeSize, minStorageBufferOffsetAlignment);

				const sequence<ref<BaseBufferView>> descriptorBufferViews = {
					// Camera
					cameraBuffer->createView<Camera>(0, cameraBuffer->getSize()),

					// Static
					staticBuffer->createView<StaticTriangle3D>(0, triangleRangeSize),
					staticBuffer->createView<float32>(triangleAlignedRangeSize, vertexRangeSize),
					staticBuffer->createView<float32>(triangleAlignedRangeSize + vertexAlignedRangeSize, normalRangeSize),
					staticBuffer->createView<float32>(triangleAlignedRangeSize + vertexAlignedRangeSize + normalAlignedRangeSize, textureCoordinateRangeSize),
					staticBuffer->createView<float32>(triangleAlignedRangeSize + vertexAlignedRangeSize + normalAlignedRangeSize + textureCoordinateAlignedRangeSize, lightmapCoordinateRangeSize),

					// Vertex
					dataBuffer->createView<float32>(scene->getVertexPositionComponentRange().offset, scene->getVertexPositionComponentRange().count),
					dataBuffer->createView<float32>(scene->getVertexNormalComponentRange().offset, scene->getVertexNormalComponentRange().count),
					dataBuffer->createView<float32>(scene->getVertexTextureCoordinateComponentRange().offset, scene->getVertexTextureCoordinateComponentRange().count),
					dataBuffer->createView<float32>(scene->getTriangleTangentComponentRange().offset, scene->getTriangleTangentComponentRange().count),
				};

				uint64 writeDescriptorSetCount = 0;

				if (scene->getAsset()->getStaticTriangles().count() > 0) {
					writeDescriptorSetCount += staticDescriptorSetLayout->bindings.count();
				}

				if (scene->getIndexCount() > 0) {
					writeDescriptorSetCount += vertexDescriptorSetLayout->bindings.count();
				}

				sequence<WriteDescriptorSet> writeDescriptorSets(writeDescriptorSetCount);
				uint32 writeDescriptorSetOffset = 0;

				if (scene->getAsset()->getStaticTriangles().count() > 0) {
					// Camera
					writeDescriptorSets[writeDescriptorSetOffset] = {
						.descriptorSet = staticDescriptorSet,
						.bindingIndex = 0,
						.descriptorBufferViews = descriptorBufferViews[0],
					};

					uint32 writeDescriptorSetIndex = 1;

					while (writeDescriptorSetIndex < staticDescriptorSetLayout->bindings.count() - 1) {
						writeDescriptorSets[writeDescriptorSetOffset + writeDescriptorSetIndex] = {
							.descriptorSet = staticDescriptorSet,
							.bindingIndex = writeDescriptorSetIndex,
							.descriptorBufferViews = descriptorBufferViews[writeDescriptorSetOffset + writeDescriptorSetIndex],
						};

						writeDescriptorSetIndex += 1;
					}

					const DescriptorImage lightmapAtlasDescriptorImage = {
						.imageView = lightmapAtlasImageView,
						.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.sampler = imageSampler,
					};

					writeDescriptorSets[writeDescriptorSetOffset + writeDescriptorSetIndex] = {
						.descriptorSet = staticDescriptorSet,
						.bindingIndex = writeDescriptorSetIndex,
						.descriptorImages = lightmapAtlasDescriptorImage,
					};

					writeDescriptorSetIndex += 1;
					writeDescriptorSetOffset += static_cast<uint32>(staticDescriptorSetLayout->bindings.count());
				}

				if (scene->getIndexCount() > 0) {
					// Camera
					writeDescriptorSets[writeDescriptorSetOffset] = {
						.descriptorSet = vertexDescriptorSet,
						.bindingIndex = 0,
						.descriptorBufferViews = descriptorBufferViews[0],
					};

					uint32 writeDescriptorSetIndex = 1;

					while (writeDescriptorSetIndex < vertexDescriptorSetLayout->bindings.count()) {
						writeDescriptorSets[writeDescriptorSetOffset + writeDescriptorSetIndex] = {
							.descriptorSet = vertexDescriptorSet,
							.bindingIndex = writeDescriptorSetIndex,
							.descriptorBufferViews = descriptorBufferViews[writeDescriptorSetIndex + 5],
						};

						writeDescriptorSetIndex += 1;
					}

					writeDescriptorSetOffset += static_cast<uint32>(vertexDescriptorSetLayout->bindings.count());
				}

				device->updateDescriptorSets(writeDescriptorSets);
			}

			void postSetCameraController() override {}

			void createMaterialObjects();

			void renderStaticObjects(ref<CommandEncoder> commandEncoder) const;

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> staticDescriptorSetLayout;
			ref<DescriptorSetLayout> vertexDescriptorSetLayout;
			sequence<ref<DescriptorSetLayout>> materialDescriptorSetLayouts;

			ref<DescriptorSet> staticDescriptorSet;
			ref<DescriptorSet> vertexDescriptorSet;
			sequence<ref<DescriptorSet>> materialDescriptorSets;

			sequence<ref<PipelineLayout>> materialPipelineLayouts;
			ref<PipelineLayout> debugHullWireframePipelineLayout;
			ref<PipelineLayout> debugContactManifoldPipelineLayout;

			// TODO: Use the same system as for the dynamic objects.
			ref<PipelineLayout> staticMaterialPipelineLayout;

			sequence<ref<GraphicsPipeline>> materialPipelines;
			ref<GraphicsPipeline> debugHullWireframePipeline;
			ref<GraphicsPipeline> debugContactManifoldPipeline;

			// TODO: Use the same system as for the dynamic objects.
			ref<GraphicsPipeline> staticMaterialPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> dataBuffer;
			ref<Buffer> cameraBuffer;
			ref<Buffer> debugHullWireframeBuffer;
			ref<Buffer> debugContactManifoldBuffer;
			ref<Buffer> staticBuffer;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<Image> colorImage;
			#endif

			ref<Image> depthStencilImage;
			sequence<sequence<ref<Image>>> materialImages;
			ref<Image> lightmapAtlasImage;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<ImageView> colorImageView;
			#endif

			ref<ImageView> depthStencilImageView;
			sequence<ref<ImageView>> materialReferenceBindingImageViews;
			ref<ImageView> lightmapAtlasImageView;

			ref<Sampler> imageSampler;

			Instance* instanceMappedRange = nullptr;
			Camera* cameraMappedRange = nullptr;

		public:
			svec3<float32>* debugContactRange = nullptr;
			uint32 contactCount = 0;

		private:
			bool draw = false;
			bool debugHullWireframe = true;
			bool debugContactManifolds = false;
	};
}