#pragma once

#include "Engine/Renderer/3D/Renderer3D.hpp"

namespace ProjectA {
	class ForwardRenderer3D : public Renderer3D {
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

					ColorAttachment colorAttachment{};
					colorAttachment.format = surface->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;
					colorAttachment.finalImageLayout = ImageLayout::PRESENT_SRC;

					SubpassDescriptor subpassDescriptor{};
					subpassDescriptor.colorAttachments = {colorAttachment};
					subpassDescriptor.depthStencil = false;

					subpass = createSubpass(subpassDescriptor);

					RenderPassDescriptor renderPassDescriptor;
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

				DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding{};
				cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
				cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
				cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

				// 3. Create pipeline layout.
				{
					DescriptorSetLayout cameraDescriptorSetLayout{};
					cameraDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor{};
					pipelineLayoutDescriptor.layouts = {cameraDescriptorSetLayout};

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 4. Create render pipeline.
				{
					VertexInputBinding transformInputBinding(InputRate::INSTANCE);
					transformInputBinding.registerAttribute<mat4x4<float32>>();

					VertexInputBinding geometryInputBinding(InputRate::VERTEX);
					geometryInputBinding.registerAttribute<vec3<float32>>();

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
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Base/Forward3D.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Base/Forward3D.frag", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;

					// TODO: Use CullMode::BACK.
					renderPipelineDescriptor.cullMode = CullMode::NONE;

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
				Renderer3D::setScene(scene);

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

				uint64 geometryVertexBufferSize = 0;
				uint64 geometryIndexBufferSize = 0;

				for (uint32 geometryIndex = 0; geometryIndex < geometryPool.componentCount; geometryIndex += 1) {
					const ref<Geometry3D>& geometry = geometryPool.components[geometryIndex];

					geometryVertexBufferSize += geometry->getVertexCount() * sizeof(vec3<float32>);
					geometryIndexBufferSize += geometry->getIndexCount() * sizeof(uint16);
				}

				const ref<Buffer> geometryVertexStagingBuffer = createStagingBuffer(geometryVertexBufferSize);

				vec3<float32>* geometryVertexMappedRange = geometryVertexStagingBuffer->map<vec3<float32>>(0, geometryVertexStagingBuffer->size);

				const ref<Buffer> geometryIndexStagingBuffer = createStagingBuffer(geometryIndexBufferSize);

				uint16* geometryIndexMappedRange = geometryIndexStagingBuffer->map<uint16>(0, geometryIndexStagingBuffer->size);

				uint64 vertexIndex = 0;
				uint64 indexIndex = 0;

				for (uint32 geometryIndex = 0; geometryIndex < geometryPool.componentCount; geometryIndex += 1) {
					const ref<Geometry3D>& geometry = geometryPool.components[geometryIndex];

					for (uint32 geometryVertexIndex = 0; geometryVertexIndex < geometry->getVertexCount(); geometryVertexIndex += 1) {
						geometryVertexMappedRange[vertexIndex] = geometry->getVertices()[geometryVertexIndex];
						vertexIndex += 1;
					}

					for (uint32 geometryIndexIndex = 0; geometryIndexIndex < geometry->getIndexCount(); geometryIndexIndex += 1) {
						geometryIndexMappedRange[indexIndex] = geometry->getIndices()[geometryIndexIndex];
						indexIndex += 1;
					}
				}

				geometryVertexStagingBuffer->unmap();

				BufferDescriptor geometryVertexBufferDescriptor{};
				geometryVertexBufferDescriptor.size = geometryVertexStagingBuffer->size;
				geometryVertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				geometryVertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				geometryVertexBufferDescriptor.persistent = false;

				geometryVertexBuffer = createBuffer(geometryVertexBufferDescriptor);

				geometryIndexStagingBuffer->unmap();

				BufferDescriptor geometryIndexBufferDescriptor{};
				geometryIndexBufferDescriptor.size = geometryIndexStagingBuffer->size;
				geometryIndexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
				geometryIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				geometryIndexBufferDescriptor.persistent = false;

				geometryIndexBuffer = createBuffer(geometryIndexBufferDescriptor);

				sequence<BufferCopyRegion> geometryVertexBufferCopyRegions(1);

				BufferCopyRegion& geometryVertexBufferCopyRegion = geometryVertexBufferCopyRegions[0];
				geometryVertexBufferCopyRegion.sourceOffset = 0;
				geometryVertexBufferCopyRegion.destinationOffset = 0;
				geometryVertexBufferCopyRegion.size = geometryVertexStagingBuffer->size;

				sequence<BufferCopyRegion> geometryIndexBufferCopyRegions(1);

				BufferCopyRegion& geometryIndexBufferCopyRegion = geometryIndexBufferCopyRegions[0];
				geometryIndexBufferCopyRegion.sourceOffset = 0;
				geometryIndexBufferCopyRegion.destinationOffset = 0;
				geometryIndexBufferCopyRegion.size = geometryIndexStagingBuffer->size;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				const ref<Device> device = getDevice();
				const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(geometryVertexStagingBuffer, geometryVertexBuffer, geometryVertexBufferCopyRegions);
				commandEncoder->copyBufferToBuffer(geometryIndexStagingBuffer, geometryIndexBuffer, geometryIndexBufferCopyRegions);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

				device->waitIdle();
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				const ref<Swapchain>& swapchain = getSwapchain();
				const float32 width = static_cast<float32>(swapchain->getExtent().width);
				const float32 height = static_cast<float32>(swapchain->getExtent().height);

				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindVertexBuffer(0, transformBuffer, 0, sizeof(mat4x4<float32>));
				commandEncoder->bindVertexBuffer(1, geometryVertexBuffer, 0, sizeof(vec3<float32>));
				commandEncoder->bindIndexBuffer<uint16>(geometryIndexBuffer, 0);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtent());

				const ComponentPool<RigidBody3D>& rigidBodyPool = scene->getComponentPool<RigidBody3D>();
				const ComponentPool<ref<Geometry3D>>& geometryPool = scene->getComponentPool<ref<Geometry3D>>();

				for (uint32 instanceOffset = 0; instanceOffset < rigidBodyPool.componentCount; instanceOffset += 1) {
					const RigidBody3D& rigidBody = rigidBodyPool.components[instanceOffset];
					const ref<Geometry3D>& geometry = geometryPool.components[rigidBody.geometryIndex];

					uint32 indexOffset = 0;

					for (uint32 geometryIndex = 0; geometryIndex < rigidBody.geometryIndex; geometryIndex += 1) {
						const ref<Geometry3D>& geometry = geometryPool.components[geometryIndex];

						indexOffset += geometry->getIndexCount();
					}

					commandEncoder->drawIndexed(geometry->getIndexCount(), 1, indexOffset, instanceOffset, 0);
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
			ref<Buffer> transformBuffer;
			Transform* transformMappedRange = nullptr;

			ref<Buffer> geometryVertexBuffer;
			ref<Buffer> geometryIndexBuffer;

			ref<Buffer> cameraBuffer;
			Camera* cameraMappedRange = nullptr;
	};
}