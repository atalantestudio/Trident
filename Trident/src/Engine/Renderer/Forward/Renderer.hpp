// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/PerspectiveCamera.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/MaterialComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace Trident {
	class ForwardRenderer : public SceneRenderer<Scene3D> {
		private:
			struct Camera {
				mat4x4<float32> projectionView;
				vec3<float32> position;
			};

			struct Instance {
				mat4x4<float32> world;
				mat3x3<float32> normal;
			};

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

				cameraBuffer = createCameraBuffer();
				cameraMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());

				imageSampler = createImageSampler();
			}

			void postSetScene() override {
				createMaterialObjects();

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

				const view<uint8> dataBuffer = scene->getDataBuffer();
				const ref<Buffer<uint8>> dataStagingBuffer = createStagingBuffer<uint8>(dataBuffer.count());

				uint8* vertexMappedRange = dataStagingBuffer->map(0, dataStagingBuffer->getSize());

				std::copy(dataBuffer.begin(), dataBuffer.end(), vertexMappedRange);

				dataStagingBuffer->unmap();

				vertexBuffer = createBuffer<uint8>({
					.size = dataStagingBuffer->getSize(),
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(BUILD_MODE_DEBUG)
						.name = "Vertex",
					#endif
				});

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(BUILD_MODE_DEBUG)
						.name = "Copy buffers",
					#endif
				});

				BufferCopyRegion copyRegion = {
					.sourceOffset = 0,
					.destinationOffset = 0,
					.size = vertexBuffer->getSize(),
				};

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(dataStagingBuffer, vertexBuffer, copyRegion);

				copyRegion = {
					.sourceOffset = scene->getHullLineIndexByteOffset(),
					.destinationOffset = 0,
					.size = debugHullWireframeBuffer->getSize(),
				};

				commandEncoder->copyBufferToBuffer(dataStagingBuffer, debugHullWireframeBuffer, copyRegion);

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

				// TODO(GL): Allow the vertex buffer to be bound before setting a render pipeline.

				const sequence<Geometry3D>& geometries = scene->getGeometries();
				const ComponentPool<GeometryComponent>& geometryComponentPool = scene->ecs.getComponentPool<GeometryComponent>();
				const ComponentPool<MaterialComponent>& materialComponentPool = scene->ecs.getComponentPool<MaterialComponent>();
				const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

				sequence<ref<DescriptorSet>> descriptorSets = {vertexDescriptorSet, nullptr};
				uint32 rigidBodyComponentIndex = 0;

				while (rigidBodyComponentIndex < rigidBodyComponentPool.componentCount) {
					const Entity entity = rigidBodyComponentPool.componentIndexToEntity[rigidBodyComponentIndex];
					const GeometryComponent geometryComponent = geometryComponentPool.components[geometryComponentPool.entityToComponentIndex[entity]];
					const Geometry3D& geometry = geometries[geometryComponent.geometryIndex];
					const MaterialComponent materialComponent = materialComponentPool.components[materialComponentPool.entityToComponentIndex[entity]];

					// NOTE: Material is incremented by 1 for visibility rendering.
					const uint32 materialShaderIndex = scene->getMaterialShaderIndex(materialComponent.materialIndex - 1);

					const range<uint32, uint32> indexRange = geometry.getVertexPositionIndexRange();

					descriptorSets[1] = materialDescriptorSets[materialShaderIndex];

					commandEncoder->bindGraphicsPipeline(materialPipelines[materialShaderIndex]);
					commandEncoder->bindVertexBuffer(0, instanceBuffer, 0, sizeof(Instance));
					commandEncoder->bindDescriptorSets(materialPipelineLayouts[materialShaderIndex], 0, descriptorSets);
					commandEncoder->draw(indexRange.count, 1, indexRange.offset, rigidBodyComponentIndex);

					rigidBodyComponentIndex += 1;
				}

				if (debugHullWireframe) {
					rigidBodyComponentIndex = 0;

					commandEncoder->bindGraphicsPipeline(debugHullWireframePipeline);
					commandEncoder->bindDescriptorSets(debugHullWireframePipelineLayout, 0, vertexDescriptorSet);
					commandEncoder->bindVertexBuffer(0, instanceBuffer, 0, sizeof(Instance));
					commandEncoder->bindIndexBuffer<uint16>(debugHullWireframeBuffer, 0);

					while (rigidBodyComponentIndex < rigidBodyComponentPool.componentCount) {
						const Entity entity = rigidBodyComponentPool.componentIndexToEntity[rigidBodyComponentIndex];
						const GeometryComponent geometryComponent = geometryComponentPool.components[geometryComponentPool.entityToComponentIndex[entity]];
						const Geometry3D& geometry = geometries[geometryComponent.geometryIndex];

						if (geometry.hullIndex >= scene->getHulls().count()) {
							rigidBodyComponentIndex += 1;

							continue;
						}

						const Hull3D& hull = scene->getHulls()[geometry.hullIndex];

						commandEncoder->drawIndexed(hull.lineIndexRange.count, 1, hull.lineIndexRange.offset, geometry.getVertexPositionComponentRange().offset / 2, rigidBodyComponentIndex);

						rigidBodyComponentIndex += 1;
					}
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
				const PerspectiveCamera& camera = getCameraController<PerspectiveCamera>().getCamera();

				cameraMappedRange->projectionView = camera.getProjectionView();
				cameraMappedRange->position = camera.getPosition();
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

				#if defined(BUILD_MODE_DEBUG)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

				while (framebufferIndex < framebuffers.count()) {
					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("In flight framebuffer []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			ref<PipelineLayout> createDebugHullWireframePipelineLayout() const;

			ref<GraphicsPipeline> createDebugHullWireframePipeline() const;

			ref<Image> createColorImage() const {
				const Extent2D extent = getSwapchain()->getExtent();

				return renderPass->createColorAttachmentImage(
					{
						.type = ImageType::TEXTURE_2D,
						.extent = {extent.width, extent.height, 1},
						.mipLevelCount = 1,
						.layerCount = 1,
						#if defined(BUILD_MODE_DEBUG)
							.name = "Color (multisampled)",
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
						#if defined(BUILD_MODE_DEBUG)
							.name = "Depth stencil"
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
					#if defined(BUILD_MODE_DEBUG)
						.name = "Color (multisampled)",
					#endif
				});
			}

			ref<ImageView> createDepthStencilImageView() const {
				return depthStencilImage->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(BUILD_MODE_DEBUG)
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

					// Vertex position index
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},

					// Vertex normal index
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},

					// Vertex texture coordinate index
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},
				});
			}

			ref<Buffer<Instance>> createInstanceBuffer() const {
				const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

				return createBuffer<Instance>({
					.size = rigidBodyComponentPool.componentCount * sizeof(Instance),
					.usage = BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
					.persistent = true,
					#if defined(BUILD_MODE_DEBUG)
						.name = "Instance",
					#endif
				});
			}

			ref<Buffer<uint16>> createDebugHullWireframeBuffer() const;

			void createMaterialObjects();

			ref<Buffer<Camera>> createCameraBuffer() const {
				return createBuffer<Camera>({
					.size = sizeof(Camera),
					.usage = BufferUsage::UNIFORM,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
					.persistent = true,
					#if defined(BUILD_MODE_DEBUG)
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
						.buffer = vertexBuffer,
						.offset = scene->getVertexPositionComponentRange().offset,
						.size = scene->getVertexPositionComponentRange().count,
					},
					{
						.buffer = vertexBuffer,
						.offset = scene->getVertexNormalComponentRange().offset,
						.size = scene->getVertexNormalComponentRange().count,
					},
					{
						.buffer = vertexBuffer,
						.offset = scene->getVertexTextureCoordinateComponentRange().offset,
						.size = scene->getVertexTextureCoordinateComponentRange().count,
					},
					{
						.buffer = vertexBuffer,
						.offset = scene->getTriangleTangentComponentRange().offset,
						.size = scene->getTriangleTangentComponentRange().count,
					},
					{
						.buffer = vertexBuffer,
						.offset = scene->getVertexPositionIndexRange().offset,
						.size = scene->getVertexPositionIndexRange().count,
					},
					{
						.buffer = vertexBuffer,
						.offset = scene->getVertexNormalIndexRange().offset,
						.size = scene->getVertexNormalIndexRange().count,
					},
					{
						.buffer = vertexBuffer,
						.offset = scene->getVertexTextureCoordinateIndexRange().offset,
						.size = scene->getVertexTextureCoordinateIndexRange().count,
					},
				};

				sequence<WriteDescriptorSet> writeDescriptorSets(8);
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

			sequence<ref<GraphicsPipeline>> materialPipelines;
			ref<GraphicsPipeline> debugHullWireframePipeline;

			ref<Buffer<uint8>> vertexBuffer;
			ref<Buffer<Instance>> instanceBuffer;
			ref<Buffer<Camera>> cameraBuffer;
			ref<Buffer<uint16>> debugHullWireframeBuffer;

			ref<Image> colorImage;
			ref<Image> depthStencilImage;
			sequence<sequence<ref<Image>>> materialImages;

			ref<ImageView> colorImageView;
			ref<ImageView> depthStencilImageView;
			sequence<ref<ImageView>> materialReferenceBindingImageViews;

			ref<Sampler> imageSampler;

			Camera* cameraMappedRange = nullptr;
			Instance* instanceMappedRange = nullptr;

			bool debugHullWireframe = true;
	};
}