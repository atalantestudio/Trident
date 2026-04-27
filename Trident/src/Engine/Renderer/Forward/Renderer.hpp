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
			void build() override {
				_createRenderPass();

				colorImage = createColorImage();
				depthStencilImage = createDepthStencilImage();

				colorImageView = createColorImageView();
				depthStencilImageView = createDepthStencilImageView();

				createFramebuffers();

				vertexDescriptorSetLayout = createVertexDescriptorSetLayout();

				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(vertexDescriptorSetLayout);

				vertexDescriptorSet = descriptorSets[0];

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

				// Write index buffer.
				const view<uint8> sourceDataBuffer = scene->getDataBuffer();
				const range<uint64, uint64> vertexPositionIndexRange = scene->getVertexPositionIndexRange();
				const range<uint64, uint64> vertexNormalIndexRange = scene->getVertexNormalIndexRange();
				const range<uint64, uint64> vertexTextureCoordinateIndexRange = scene->getVertexTextureCoordinateIndexRange();

				// TODO(geometry/3d/vertex-position-indices): Separe hull line indices and vertex position indices for TRIANGLE_LIST.
				// DEBUG_ASSERT(vertexPositionIndexRange.count == vertexNormalIndexRange.count && vertexPositionIndexRange.count == vertexTextureCoordinateIndexRange.count);
				DEBUG_ASSERT(vertexNormalIndexRange.count == vertexTextureCoordinateIndexRange.count);

				vertexBuffer = createVertexBuffer();

				instanceBuffer = createInstanceBuffer();
				instanceMappedRange = instanceBuffer->map(0, instanceBuffer->getSize());

				debugHullWireframeBuffer = createDebugHullWireframeBuffer();

				const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

				uint32 componentIndex = 0;

				while (componentIndex < rigidBodyComponentPool.componentCount) {
					const RigidBody3DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

					Instance& instance = instanceMappedRange[componentIndex];
					instance.world = rigidBodyComponent.world;
					instance.normal = rigidBodyComponent.calculateNormal();

					componentIndex += 1;
				}

				// TODO: As of now the hull line indices are included twice.
				const ref<Buffer<uint8>> dataStagingBuffer = createStagingBuffer<uint8>(sourceDataBuffer.count() + scene->getHullLineIndexCount() * sizeof(uint16));

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

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Copy buffers",
					#endif
				});

				commandEncoder->begin();

				BufferCopyRegion copyRegion = {
					.sourceOffset = 0,
					.destinationOffset = 0,
					.size = dataBuffer->getSize(),
				};

				commandEncoder->copyBufferToBuffer(dataStagingBuffer, dataBuffer, copyRegion);

				copyRegion = {
					.sourceOffset = dataBuffer->getSize(),
					.destinationOffset = 0,
					.size = debugHullWireframeBuffer->getSize(),
				};

				commandEncoder->copyBufferToBuffer(dataStagingBuffer, debugHullWireframeBuffer, copyRegion);

				copyRegion = {
					.sourceOffset = vertexPositionIndexRange.offset + scene->getHullLineIndexCount() * sizeof(uint16),
					.destinationOffset = 0,
					.size = vertexBuffer->getSize(),
				};

				commandEncoder->copyBufferToBuffer(dataStagingBuffer, vertexBuffer, copyRegion);

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
							.depth = 1.0f,
							.stencil = 0,
						},
						{0.0f, 0.0f, 0.0f, 1.0f},
					},
				});
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				// TODO(graphics/gl/bind-vertex-buffers): Allow vertex buffers to be bound before setting a graphics pipeline.

				const ComponentPool<GeometryComponent>& geometryComponentPool = scene->ecs.getComponentPool<GeometryComponent>();
				const ComponentPool<MaterialComponent>& materialComponentPool = scene->ecs.getComponentPool<MaterialComponent>();
				const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

				if (draw) {
					sequence<ref<DescriptorSet>> descriptorSets = {vertexDescriptorSet, nullptr};
					uint32 rigidBodyComponentIndex = 0;

					while (rigidBodyComponentIndex < rigidBodyComponentPool.componentCount) {
						const Entity entity = rigidBodyComponentPool.componentIndexToEntity[rigidBodyComponentIndex];

						// TODO(debug): Don't render the player entity.
						if (entity == playerEntity) {
							rigidBodyComponentIndex += 1;

							continue;
						}

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

				if (debugHullWireframe) {
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
				colorImage = createColorImage();
				depthStencilImage = createDepthStencilImage();

				colorImageView = createColorImageView();
				depthStencilImageView = createDepthStencilImageView();

				createFramebuffers();
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
			void _createRenderPass() {
				constexpr uint32 SAMPLE_COUNT = 4;

				renderPass = createRenderPass({
					.attachments = {
						// Color
						{
							.format = getSurface()->getFormat(),
							.sampleCount = SAMPLE_COUNT,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::UNDEFINED,
							.finalLayout = ImageLayout::PRESENT_SRC,
						},

						// Depth stencil
						{
							.format = ImageFormat::D32_SFLOAT,
							.sampleCount = SAMPLE_COUNT,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::UNDEFINED,
							.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
						},
					},
					.subpasses = {
						createSubpass({
							.colorAttachmentReferences = {
								{
									.index = 0,
									.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
								},
							},
							.depthStencilAttachmentReference = {
								.index = 1,
								.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
							},
						}),
					},
				});
			}

			void createFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor = {
					.renderPass = renderPass,
					.imageViews = {colorImageView, depthStencilImageView, nullptr},
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

					framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			ref<PipelineLayout> createDebugHullWireframePipelineLayout() const;
			ref<PipelineLayout> createDebugContactManifoldPipelineLayout() const;

			ref<GraphicsPipeline> createDebugHullWireframePipeline() const;
			ref<GraphicsPipeline> createDebugContactManifoldPipeline() const;

			ref<Image> createColorImage() const {
				const Extent2D extent = getSwapchain()->getExtent();

				return renderPass->createColorAttachmentImage(
					{
						.type = ImageType::TEXTURE_2D,
						.extent = {extent.width, extent.height, 1},
						.mipLevelCount = 1,
						.layerCount = 1,
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Color",
						#endif
					},
					0
				);
			}

			ref<Image> createDepthStencilImage() const {
				const Extent2D extent = getSwapchain()->getExtent();

				return renderPass->createDepthStencilAttachmentImage(
					{
						.type = ImageType::TEXTURE_2D,
						.extent = {extent.width, extent.height, 1},
						.mipLevelCount = 1,
						.layerCount = 1,
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Depth stencil",
						#endif
					},
					0
				);
			}

			ref<ImageView> createColorImageView() const {
				return colorImage->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Color",
					#endif
				});
			}

			ref<ImageView> createDepthStencilImageView() const {
				return depthStencilImage->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Depth stencil",
					#endif
				});
			}

			ref<DescriptorSetLayout> createVertexDescriptorSetLayout() const {
				return createDescriptorSetLayout({
					// Camera
					{
						.descriptorType = DescriptorType::UNIFORM_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
					},

					// Vertex position component
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},

					// Vertex normal component
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},

					// Vertex texture coordinate component
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},

					// Triangle tangent component
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},
				});
			}

			ref<Buffer<uint16>> createDebugHullWireframeBuffer() const;
			ref<Buffer<svec3<float32>>> createDebugContactManifoldBuffer() const;

			void createMaterialObjects();

			ref<Buffer<Vertex>> createVertexBuffer() const {
				return createBuffer<Vertex>({
					.size = scene->getIndexCount() * sizeof(Vertex),
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

			ref<Buffer<Instance>> createInstanceBuffer() const {
				const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

				return createBuffer<Instance>({
					.size = rigidBodyComponentPool.componentCount * sizeof(Instance),
					.usage = BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
					.persistent = true,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Instance",
					#endif
				});
			}

			ref<Buffer<Camera>> createCameraBuffer() const {
				return createBuffer<Camera>({
					.size = sizeof(Camera),
					.usage = BufferUsage::UNIFORM,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
					.persistent = true,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Camera",
					#endif
				});
			}

			ref<Sampler> createImageSampler() const {
				return createSampler({
					.magFilter = SamplerFilter::LINEAR,
					.minFilter = SamplerFilter::LINEAR,
				});
			}

			void updateDescriptorSets() const {
				const sequence<DescriptorBuffer> descriptorBuffers = {
					{
						.buffer = cameraBuffer,
						.offset = 0,
						.size = cameraBuffer->getSize(),
					},
					{
						.buffer = dataBuffer,
						.offset = scene->getVertexPositionComponentRange().offset,
						.size = scene->getVertexPositionComponentRange().count,
					},
					{
						.buffer = dataBuffer,
						.offset = scene->getVertexNormalComponentRange().offset,
						.size = scene->getVertexNormalComponentRange().count,
					},
					{
						.buffer = dataBuffer,
						.offset = scene->getVertexTextureCoordinateComponentRange().offset,
						.size = scene->getVertexTextureCoordinateComponentRange().count,
					},
					{
						.buffer = dataBuffer,
						.offset = scene->getTriangleTangentComponentRange().offset,
						.size = scene->getTriangleTangentComponentRange().count,
					},
				};

				sequence<WriteDescriptorSet> writeDescriptorSets(descriptorBuffers.count());
				index writeDescriptorSetIndex = 0;

				while (writeDescriptorSetIndex < writeDescriptorSets.count()) {
					writeDescriptorSets[writeDescriptorSetIndex] = {
						.descriptorSet = vertexDescriptorSet,
						.bindingIndex = static_cast<uint32>(writeDescriptorSetIndex),
						.descriptorBuffers = descriptorBuffers[writeDescriptorSetIndex],
					};

					writeDescriptorSetIndex += 1;
				}

				getDevice()->updateDescriptorSets(writeDescriptorSets);
			}

			void postSetCameraController() override {}

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> vertexDescriptorSetLayout;
			sequence<ref<DescriptorSetLayout>> materialDescriptorSetLayouts;

			ref<DescriptorSet> vertexDescriptorSet;
			sequence<ref<DescriptorSet>> materialDescriptorSets;

			sequence<ref<PipelineLayout>> materialPipelineLayouts;
			ref<PipelineLayout> debugHullWireframePipelineLayout;
			ref<PipelineLayout> debugContactManifoldPipelineLayout;

			sequence<ref<GraphicsPipeline>> materialPipelines;
			ref<GraphicsPipeline> debugHullWireframePipeline;
			ref<GraphicsPipeline> debugContactManifoldPipeline;

			ref<Buffer<Vertex>> vertexBuffer;
			ref<Buffer<Instance>> instanceBuffer;
			ref<Buffer<uint8>> dataBuffer;
			ref<Buffer<Camera>> cameraBuffer;
			ref<Buffer<uint16>> debugHullWireframeBuffer;
			ref<Buffer<svec3<float32>>> debugContactManifoldBuffer;

			ref<Image> colorImage;
			ref<Image> depthStencilImage;
			sequence<sequence<ref<Image>>> materialImages;

			ref<ImageView> colorImageView;
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
			bool debugContactManifolds = true;

		public:
			// TODO(debug)
			Entity playerEntity;
	};
}