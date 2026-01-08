#pragma once

#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace ProjectA {
	class ForwardRenderer3D : public SceneRenderer<Scene3D> {
		private:
			struct Transform {
				mat4x4<float32> transform;
			};

			struct Camera {
				mat4x4<float32> projection;
			};

		public:
			void build() override {
				// 1. Create render pass and subpass.
				{
					const ref<Surface> surface = getSurface();

					Attachment swapchainAttachment{};
					swapchainAttachment.format = surface->getFormat();
					swapchainAttachment.sampleCount = 1;
					swapchainAttachment.loadOperation = LoadOperation::CLEAR;
					swapchainAttachment.storeOperation = StoreOperation::STORE;
					swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

					AttachmentReference colorAttachmentReference{};
					colorAttachmentReference.index = 0;
					colorAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

					SubpassDescriptor subpassDescriptor{};
					subpassDescriptor.colorAttachmentReferences = {colorAttachmentReference};

					subpass = createSubpass(subpassDescriptor);

					RenderPassDescriptor renderPassDescriptor{};
					renderPassDescriptor.attachments = {swapchainAttachment};
					renderPassDescriptor.subpasses = {subpass};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 2. Create framebuffers.
				recreateFramebuffers();

				BufferDescriptor cameraBufferDescriptor;
				cameraBufferDescriptor.size = sizeof(Camera);
				cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
				cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				cameraBufferDescriptor.persistent = true;

				cameraBuffer = createBuffer(cameraBufferDescriptor);
				cameraMappedRange = cameraBuffer->map<Camera>(0, cameraBuffer->size);

				// 3. Create pipeline layout.
				{
					DescriptorSetLayoutBinding cameraBinding{};
					cameraBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
					cameraBinding.descriptorCount = 1;
					cameraBinding.shaderStages = ShaderStage::VERTEX;

					cameraDescriptorSetLayout = createDescriptorSetLayout({cameraBinding});

					const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({cameraDescriptorSetLayout});

					cameraDescriptorSet = descriptorSets[0];

					DescriptorBuffer cameraDescriptorBuffer{};
					cameraDescriptorBuffer.buffer = cameraBuffer;
					cameraDescriptorBuffer.offset = 0;
					cameraDescriptorBuffer.size = cameraBuffer->size;

					WriteDescriptorSet cameraWriteDescriptorSet{};
					cameraWriteDescriptorSet.descriptorSet = cameraDescriptorSet;
					cameraWriteDescriptorSet.bindingIndex = 0;
					cameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

					getDevice()->updateDescriptorSets({cameraWriteDescriptorSet});

					PipelineLayoutDescriptor pipelineLayoutDescriptor{};
					pipelineLayoutDescriptor.descriptorSetLayouts = {cameraDescriptorSetLayout};

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 4. Create render pipeline.
				{
					VertexInputBinding transformInputBinding(InputRate::INSTANCE);
					transformInputBinding.registerAttribute<mat4x4<float32>>();

					VertexInputBinding geometryInputBinding(InputRate::VERTEX);
					geometryInputBinding.registerAttribute<svec3<float32>>();

					ShaderModuleDescriptor vertexShaderModuleDescriptor{};
					vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
					vertexShaderModuleDescriptor.entryPoint = "main";

					ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
					fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
					fragmentShaderModuleDescriptor.entryPoint = "main";

					ColorBlendAttachment colorBlendAttachment{};
					colorBlendAttachment.blend = false;

					RenderPipelineDescriptor renderPipelineDescriptor;
					renderPipelineDescriptor.pipelineLayout = pipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {transformInputBinding, geometryInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Base/Forward3D.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Base/Forward3D.frag", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					renderPipelineDescriptor.cullMode = CullMode::BACK;
					renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
					renderPipelineDescriptor.lineWidth = 1.0f;
					renderPipelineDescriptor.minSampleShading = 0.0f;
					renderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
					renderPipelineDescriptor.depthStencilState.depthTest = false;
					renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
					renderPipelineDescriptor.renderPass = renderPass;
					renderPipelineDescriptor.subpassIndex = 0;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}
			}

			void setScene(const Scene3D* scene) override {
				SceneRenderer::setScene(scene);

				const ComponentPool<RigidBody3D>& rigidBodyPool = scene->getComponentPool<RigidBody3D>();
				const ComponentPool<ref<Geometry3D>>& geometryPool = scene->getComponentPool<ref<Geometry3D>>();

				const uint64 transformBufferSize = rigidBodyPool.componentCount * sizeof(mat4x4<float32>);

				BufferDescriptor transformBufferDescriptor{};
				transformBufferDescriptor.size = transformBufferSize;
				transformBufferDescriptor.usage = BufferUsage::VERTEX;
				transformBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				transformBufferDescriptor.persistent = true;

				transformBuffer = createBuffer(transformBufferDescriptor);

				transformMappedRange = transformBuffer->map<Transform>(0, transformBuffer->size);

				for (Entity entity = 0; entity <= scene->getLastEntity(); entity += 1) {
					updateTransform(entity);
				}

				uint64 vertexPositionBufferSize = 0;
				uint64 vertexPositionIndexBufferSize = 0;

				for (uint32 geometryIndex = 0; geometryIndex < geometryPool.componentCount; geometryIndex += 1) {
					const ref<Geometry3D>& geometry = geometryPool.components[geometryIndex];

					vertexPositionBufferSize += geometry->getVertexPositionComponents().count * sizeof(float32);
					vertexPositionIndexBufferSize += geometry->getVertexPositionIndices().count * sizeof(uint32);
				}

				const ref<Buffer> vertexPositionStagingBuffer = createStagingBuffer(vertexPositionBufferSize);

				float32* vertexPositionMappedRange = vertexPositionStagingBuffer->map<float32>(0, vertexPositionStagingBuffer->size);

				const ref<Buffer> vertexPositionIndexStagingBuffer = createStagingBuffer(vertexPositionIndexBufferSize);

				uint32* vertexPositionIndexMappedRange = vertexPositionIndexStagingBuffer->map<uint32>(0, vertexPositionIndexStagingBuffer->size);

				uint64 vertexPositionOffset = 0;
				uint64 vertexPositionIndexOffset = 0;

				for (uint32 geometryIndex = 0; geometryIndex < geometryPool.componentCount; geometryIndex += 1) {
					const ref<Geometry3D>& geometry = geometryPool.components[geometryIndex];

					std::copy(geometry->getVertexPositionComponents().begin(), geometry->getVertexPositionComponents().end(), vertexPositionMappedRange + vertexPositionOffset);
					std::copy(geometry->getVertexPositionIndices().begin(), geometry->getVertexPositionIndices().end(), vertexPositionIndexMappedRange + vertexPositionIndexOffset);

					vertexPositionOffset += geometry->getVertexPositionComponents().count;
					vertexPositionIndexOffset += geometry->getVertexPositionIndices().count;
				}

				vertexPositionStagingBuffer->unmap();

				BufferDescriptor vertexPositionBufferDescriptor{};
				vertexPositionBufferDescriptor.size = vertexPositionStagingBuffer->size;
				vertexPositionBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexPositionBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexPositionBufferDescriptor.persistent = false;

				vertexPositionBuffer = createBuffer(vertexPositionBufferDescriptor);

				vertexPositionIndexStagingBuffer->unmap();

				BufferDescriptor vertexPositionIndexBufferDescriptor{};
				vertexPositionIndexBufferDescriptor.size = vertexPositionIndexStagingBuffer->size;
				vertexPositionIndexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
				vertexPositionIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexPositionIndexBufferDescriptor.persistent = false;

				vertexPositionIndexBuffer = createBuffer(vertexPositionIndexBufferDescriptor);

				sequence<BufferCopyRegion> vertexPositionBufferCopyRegions(1);

				BufferCopyRegion& vertexPositionBufferCopyRegion = vertexPositionBufferCopyRegions[0];
				vertexPositionBufferCopyRegion.sourceOffset = 0;
				vertexPositionBufferCopyRegion.destinationOffset = 0;
				vertexPositionBufferCopyRegion.size = vertexPositionStagingBuffer->size;

				sequence<BufferCopyRegion> vertexPositionIndexBufferCopyRegions(1);

				BufferCopyRegion& vertexPositionIndexBufferCopyRegion = vertexPositionIndexBufferCopyRegions[0];
				vertexPositionIndexBufferCopyRegion.sourceOffset = 0;
				vertexPositionIndexBufferCopyRegion.destinationOffset = 0;
				vertexPositionIndexBufferCopyRegion.size = vertexPositionIndexStagingBuffer->size;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				const ref<Device> device = getDevice();
				const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(vertexPositionStagingBuffer, vertexPositionBuffer, vertexPositionBufferCopyRegions);
				commandEncoder->copyBufferToBuffer(vertexPositionIndexStagingBuffer, vertexPositionIndexBuffer, vertexPositionIndexBufferCopyRegions);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
				transferQueue->waitIdle();
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, {cameraDescriptorSet});
				commandEncoder->bindVertexBuffer(0, transformBuffer, 0, sizeof(mat4x4<float32>));
				commandEncoder->bindVertexBuffer(1, vertexPositionBuffer, 0, 3 * sizeof(float32));
				commandEncoder->bindIndexBuffer<uint32>(vertexPositionIndexBuffer, 0);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				const ComponentPool<RigidBody3D>& rigidBodyPool = scene->getComponentPool<RigidBody3D>();
				const ComponentPool<ref<Geometry3D>>& geometryPool = scene->getComponentPool<ref<Geometry3D>>();

				for (uint32 instanceOffset = 0; instanceOffset < rigidBodyPool.componentCount; instanceOffset += 1) {
					const RigidBody3D& rigidBody = rigidBodyPool.components[instanceOffset];
					const ref<Geometry3D>& geometry = geometryPool.components[rigidBody.geometryIndex];

					uint32 indexOffset = 0;

					for (uint32 geometryIndex = 0; geometryIndex < rigidBody.geometryIndex; geometryIndex += 1) {
						const ref<Geometry3D>& geometry = geometryPool.components[geometryIndex];

						indexOffset += static_cast<uint32>(geometry->getVertexPositionIndices().count);
					}

					commandEncoder->drawIndexed(static_cast<uint32>(geometry->getVertexPositionIndices().count), 1, indexOffset, instanceOffset, 0);
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
				updateCamera();
			}

			void updateTransform(Entity entity) {
				transformMappedRange[entity].transform = scene->getRigidBody(entity).transform;
			}

			void updateCamera() {
				cameraMappedRange->projection = camera->projection;
			}

		private:
			void recreateFramebuffers() {
				const ref<Swapchain>& swapchain = getSwapchain();

				for (ref<Framebuffer>& framebuffer : framebuffers) {
					framebuffer.~shared_ptr();
				}

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = sequence<ref<ImageView>>(1);
				framebufferDescriptor.extent = swapchain->getExtent();
				framebufferDescriptor.layerCount = 1;

				for (uint32 framebufferIndex = 0; framebufferIndex < framebuffers.count; framebufferIndex += 1) {
					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
				}
			}

		private:
			ref<RenderPass> renderPass;
			ref<Subpass> subpass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> cameraDescriptorSetLayout;
			ref<DescriptorSet> cameraDescriptorSet;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> transformBuffer;
			Transform* transformMappedRange = nullptr;

			ref<Buffer> vertexPositionBuffer;
			ref<Buffer> vertexPositionIndexBuffer;

			ref<Buffer> vertexNormalBuffer;
			ref<Buffer> vertexNormalIndexBuffer;

			ref<Buffer> vertexTextureCoordinateBuffer;
			ref<Buffer> vertexTextureCoordinateIndexBuffer;

			ref<Buffer> cameraBuffer;
			Camera* cameraMappedRange = nullptr;
	};
}