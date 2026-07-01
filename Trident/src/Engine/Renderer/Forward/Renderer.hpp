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
				cameraMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());

				debugContactManifoldBuffer = createDebugContactManifoldBuffer();
				debugContactRange = debugContactManifoldBuffer->map(0, debugContactManifoldBuffer->getSize());

				imageSampler = createImageSampler();
			}

			void postSetScene() override {
				createMaterialObjects();

				if (scene->getAsset()->getStaticTriangles().count() > 0) {
					staticBuffer = createStaticBuffer();

					staticMaterialPipelineLayout = createStaticMaterialPipelineLayout();
					staticMaterialPipeline = createStaticMaterialPipeline();
				}

				// Write index buffer.
				const view<uint8> sourceDataBuffer = scene->getDataBuffer();
				const range<uint64, uint64> vertexPositionIndexRange = scene->getVertexPositionIndexRange();
				const range<uint64, uint64> vertexNormalIndexRange = scene->getVertexNormalIndexRange();
				const range<uint64, uint64> vertexTextureCoordinateIndexRange = scene->getVertexTextureCoordinateIndexRange();

				// TODO(geometry/3d/vertex-position-indices): Separe hull line indices and vertex position indices for TRIANGLE_LIST.
				// DEBUG_ASSERT(vertexPositionIndexRange.count == vertexNormalIndexRange.count && vertexPositionIndexRange.count == vertexTextureCoordinateIndexRange.count);
				DEBUG_ASSERT(vertexNormalIndexRange.count == vertexTextureCoordinateIndexRange.count);

				if (scene->getIndexCount() > 0) {
					vertexBuffer = createVertexBuffer();
				}

				instanceBuffer = createInstanceBuffer();
				instanceMappedRange = instanceBuffer->map(0, instanceBuffer->getSize());

				if (scene->getHullLineIndexCount() > 0) {
					debugHullWireframeBuffer = createDebugHullWireframeBuffer();
				}

				const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

				uint32 componentIndex = 0;

				while (componentIndex < rigidBodyComponentPool.componentCount) {
					const RigidBody3DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

					Instance& instance = instanceMappedRange[componentIndex];
					instance.world = rigidBodyComponent.world;
					instance.normal = rigidBodyComponent.calculateNormal();

					componentIndex += 1;
				}

				ref<Buffer<uint8>> dataStagingBuffer;

				if (scene->getIndexCount() > 0) {
					// TODO: As of now the hull line indices are included twice.
					dataStagingBuffer = createStagingBuffer<uint8>(sourceDataBuffer.count() + scene->getHullLineIndexCount() * sizeof(uint16));

					uint8* vertexMappedRange = dataStagingBuffer->map(0, dataStagingBuffer->getSize());

					std::copy_n(sourceDataBuffer.begin(), vertexPositionIndexRange.offset, vertexMappedRange);
					std::copy_n(&sourceDataBuffer[scene->getHullLineIndexByteOffset()], scene->getHullLineIndexCount() * sizeof(uint16), vertexMappedRange + vertexPositionIndexRange.offset);

					index vertexIndex = 0;

					while (vertexIndex < scene->getIndexCount()) {
						Vertex& vertex = *pointer_cast<Vertex*>(vertexMappedRange + vertexPositionIndexRange.offset + scene->getHullLineIndexCount() * sizeof(uint16) + vertexIndex * sizeof(Vertex));
						vertex.positionIndex = *pointer_cast<const uint32*>(&sourceDataBuffer[vertexPositionIndexRange.offset + vertexIndex * sizeof(uint32)]);
						vertex.normalIndex = *pointer_cast<const uint32*>(&sourceDataBuffer[vertexNormalIndexRange.offset + vertexIndex * sizeof(uint32)]);
						vertex.textureCoordinateIndex = *pointer_cast<const uint32*>(&sourceDataBuffer[vertexTextureCoordinateIndexRange.offset + vertexIndex * sizeof(uint32)]);

						vertexIndex += 1;
					}

					dataStagingBuffer->unmap();

					dataBuffer = createBuffer<uint8>({
						.size = vertexPositionIndexRange.offset,
						.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
						.memoryProperties = MemoryProperty::DEVICE_LOCAL,
						.persistent = false,
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Data",
						#endif
					});
				}

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Copy buffers",
					#endif
				});

				commandEncoder->begin();

				BufferCopyRegion copyRegion;

				if (scene->getIndexCount() > 0) {
					copyRegion = {
						.sourceOffset = 0,
						.destinationOffset = 0,
						.size = dataBuffer->getSize(),
					};

					commandEncoder->copyBufferToBuffer(dataStagingBuffer, dataBuffer, copyRegion);
				}

				if (scene->getHullLineIndexCount() > 0) {
					copyRegion = {
						.sourceOffset = dataBuffer->getSize(),
						.destinationOffset = 0,
						.size = debugHullWireframeBuffer->getSize(),
					};

					commandEncoder->copyBufferToBuffer(dataStagingBuffer, debugHullWireframeBuffer, copyRegion);
				}

				if (scene->getIndexCount() > 0) {
					copyRegion = {
						.sourceOffset = vertexPositionIndexRange.offset + scene->getHullLineIndexCount() * sizeof(uint16),
						.destinationOffset = 0,
						.size = vertexBuffer->getSize(),
					};

					commandEncoder->copyBufferToBuffer(dataStagingBuffer, vertexBuffer, copyRegion);
				}

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();

				updateDescriptorSets();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffers[getSwapchainImageIndex()],
					.clearValues = {
						{0.0f, 0.0f, 0.0f, 1.0f},
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

			ref<ImageView> createColorImageView() const;
			ref<ImageView> createDepthStencilImageView() const;

			ref<Buffer<Camera>> createCameraBuffer() const;
			ref<Buffer<Instance>> createInstanceBuffer() const;
			ref<Buffer<Vertex>> createVertexBuffer() const;
			ref<Buffer<uint16>> createDebugHullWireframeBuffer() const;
			ref<Buffer<svec3<float32>>> createDebugContactManifoldBuffer() const;
			ref<Buffer<uint8>> createStaticBuffer() const;

			ref<Sampler> createImageSampler() const;

			void updateDescriptorSets() const {
				const ref<Device> device = getDevice();
				const ref<Scene3DAsset> sceneAsset = scene->getAsset();
				const uint64 minStorageBufferOffsetAlignment = device->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
				const uint64 triangleRangeSize = sceneAsset->getStaticTriangles().count() * sizeof(StaticTriangle3D);
				const uint64 vertexRangeSize = sceneAsset->getStaticVertices().count() * sizeof(svec3<float32>);
				const uint64 normalRangeSize = sceneAsset->getStaticNormals().count() * sizeof(svec3<float32>);
				const uint64 textureCoordinateRangeSize = sceneAsset->getStaticTextureCoordinates().count() * sizeof(svec2<float32>);
				const uint64 triangleAlignedRangeSize = ceil(triangleRangeSize, minStorageBufferOffsetAlignment);
				const uint64 vertexAlignedRangeSize = ceil(vertexRangeSize, minStorageBufferOffsetAlignment);
				const uint64 normalAlignedRangeSize = ceil(normalRangeSize, minStorageBufferOffsetAlignment);

				const sequence<ref<BaseBufferView>> descriptorBufferViews = {
					// Camera
					cameraBuffer->createView<Camera>(0, cameraBuffer->getSize()),

					// Vertex
					dataBuffer->createView<float32>(scene->getVertexPositionComponentRange().offset, scene->getVertexPositionComponentRange().count),
					dataBuffer->createView<float32>(scene->getVertexNormalComponentRange().offset, scene->getVertexNormalComponentRange().count),
					dataBuffer->createView<float32>(scene->getVertexTextureCoordinateComponentRange().offset, scene->getVertexTextureCoordinateComponentRange().count),
					dataBuffer->createView<float32>(scene->getTriangleTangentComponentRange().offset, scene->getTriangleTangentComponentRange().count),

					// Static
					staticBuffer->createView<StaticTriangle3D>(0, triangleRangeSize),
					staticBuffer->createView<float32>(triangleAlignedRangeSize, vertexRangeSize),
					staticBuffer->createView<float32>(triangleAlignedRangeSize + vertexAlignedRangeSize, normalRangeSize),
					staticBuffer->createView<float32>(triangleAlignedRangeSize + vertexAlignedRangeSize + normalAlignedRangeSize, textureCoordinateRangeSize),
				};

				uint64 writeDescriptorSetCount = 0;

				if (scene->getIndexCount() > 0) {
					writeDescriptorSetCount += 5;
				}

				if (scene->getAsset()->getStaticTriangles().count() > 0) {
					writeDescriptorSetCount += 5;
				}

				sequence<WriteDescriptorSet> writeDescriptorSets(writeDescriptorSetCount);
				uint32 writeDescriptorSetOffset = 0;

				if (scene->getIndexCount() > 0) {
					constexpr uint32 descriptorBufferOffset = 0;

					writeDescriptorSets[writeDescriptorSetOffset] = {
						.descriptorSet = vertexDescriptorSet,
						.bindingIndex = 0,
						.descriptorBufferViews = descriptorBufferViews[0],
					};

					uint32 writeDescriptorSetIndex = 1;

					while (writeDescriptorSetIndex < 5) {
						writeDescriptorSets[writeDescriptorSetOffset + writeDescriptorSetIndex] = {
							.descriptorSet = vertexDescriptorSet,
							.bindingIndex = writeDescriptorSetIndex,
							.descriptorBufferViews = descriptorBufferViews[writeDescriptorSetOffset + writeDescriptorSetIndex],
						};

						writeDescriptorSetIndex += 1;
					}

					writeDescriptorSetOffset += 5;
				}

				if (scene->getAsset()->getStaticTriangles().count() > 0) {
					constexpr uint32 descriptorBufferOffset = 4;

					writeDescriptorSets[writeDescriptorSetOffset] = {
						.descriptorSet = staticDescriptorSet,
						.bindingIndex = 0,
						.descriptorBufferViews = descriptorBufferViews[0],
					};

					uint32 writeDescriptorSetIndex = 1;

					while (writeDescriptorSetIndex < 5) {
						writeDescriptorSets[writeDescriptorSetOffset + writeDescriptorSetIndex] = {
							.descriptorSet = staticDescriptorSet,
							.bindingIndex = writeDescriptorSetIndex,
							.descriptorBufferViews = descriptorBufferViews[descriptorBufferOffset + writeDescriptorSetIndex],
						};

						writeDescriptorSetIndex += 1;
					}

					writeDescriptorSetOffset += 5;
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

			ref<Buffer<Vertex>> vertexBuffer;
			ref<Buffer<Instance>> instanceBuffer;
			ref<Buffer<uint8>> dataBuffer;
			ref<Buffer<Camera>> cameraBuffer;
			ref<Buffer<uint16>> debugHullWireframeBuffer;
			ref<Buffer<svec3<float32>>> debugContactManifoldBuffer;
			ref<Buffer<uint8>> staticBuffer;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<Image> colorImage;
			#endif

			ref<Image> depthStencilImage;
			sequence<sequence<ref<Image>>> materialImages;

			#if FORWARD_SAMPLE_COUNT > 1
				ref<ImageView> colorImageView;
			#endif

			ref<ImageView> depthStencilImageView;
			sequence<ref<ImageView>> materialReferenceBindingImageViews;

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