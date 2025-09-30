#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class ColorBlendRenderer : public Renderer {
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

					AttachmentReference swapchainAttachmentReference{};
					swapchainAttachmentReference.index = 0;
					swapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

					SubpassDescriptor subpassDescriptor{};
					subpassDescriptor.colorAttachmentReferences = {swapchainAttachmentReference};

					const ref<Subpass> subpass = createSubpass(subpassDescriptor);

					RenderPassDescriptor renderPassDescriptor{};
					renderPassDescriptor.attachments = {swapchainAttachment};
					renderPassDescriptor.subpasses = {subpass};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 2. Create framebuffers.
				recreateFramebuffers();

				// 3. Create pipeline layout.
				{
					const PipelineLayoutDescriptor pipelineLayoutDescriptor;

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 4. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>();

					VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
					instanceInputBinding.registerAttribute<vec4<float32>>();

					ShaderModuleDescriptor vertexShaderModuleDescriptor{};
					vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
					vertexShaderModuleDescriptor.entryPoint = "main";

					ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
					fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
					fragmentShaderModuleDescriptor.entryPoint = "main";

					ColorBlendAttachment colorBlendAttachment{};
					colorBlendAttachment.blend = true;
					colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
					colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

					RenderPipelineDescriptor renderPipelineDescriptor{};
					renderPipelineDescriptor.pipelineLayout = pipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Base/Vertex2DRGBA.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Base/ColorRGBA.frag", fragmentShaderModuleDescriptor);
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

				// 5. Create vertex buffers.
				{
					const ref<Device> device = getDevice();

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(VERTEX_BUFFER_SIZE);

					Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, VERTEX_BUFFER_SIZE);

					vertexMappedRange[0].vertex = {-0.75f,  0.75f};
					vertexMappedRange[1].vertex = { 0.25f,  0.75f};
					vertexMappedRange[2].vertex = {-0.75f, -0.75f};
					vertexMappedRange[3].vertex = {-0.5f,   0.5f};
					vertexMappedRange[4].vertex = { 0.5f,   0.5f};
					vertexMappedRange[5].vertex = {-0.5f,  -0.5f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					vertexBuffer = createBuffer(vertexBufferDescriptor);

					const ref<Buffer> instanceStagingBuffer = createStagingBuffer(INSTANCE_BUFFER_SIZE);

					Instance* instances = instanceStagingBuffer->map<Instance>(0, INSTANCE_BUFFER_SIZE);

					instances[0].color = {1.0f, 1.0f, 1.0f, 1.0f};
					instances[1].color = {1.0f, 0.2f, 0.0f, 0.5f};

					instanceStagingBuffer->unmap();

					BufferDescriptor instanceBufferDescriptor;
					instanceBufferDescriptor.size = INSTANCE_BUFFER_SIZE;
					instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					instanceBuffer = createBuffer(instanceBufferDescriptor);

					sequence<BufferCopyRegion> vertexBufferCopyRegions(1);

					BufferCopyRegion& vertexBufferCopyRegion = vertexBufferCopyRegions[0];
					vertexBufferCopyRegion.sourceOffset = 0;
					vertexBufferCopyRegion.destinationOffset = 0;
					vertexBufferCopyRegion.size = VERTEX_BUFFER_SIZE;

					sequence<BufferCopyRegion> instanceBufferCopyRegions(1);

					BufferCopyRegion& instanceBufferCopyRegion = instanceBufferCopyRegions[0];
					instanceBufferCopyRegion.sourceOffset = 0;
					instanceBufferCopyRegion.destinationOffset = 0;
					instanceBufferCopyRegion.size = INSTANCE_BUFFER_SIZE;

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
					const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, vertexBufferCopyRegions);
					commandEncoder->copyBufferToBuffer(instanceStagingBuffer, instanceBuffer, instanceBufferCopyRegions);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
					device->waitIdle();
				}
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				for (uint32 instanceIndex = 0; instanceIndex < INSTANCE_COUNT; instanceIndex += 1) {
					const uint32 vertexOffset = 3 * static_cast<uint32>(instanceIndex);

					commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance), sizeof(Instance));
					commandEncoder->draw(3, 1, vertexOffset, 0);
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
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
				framebufferDescriptor.imageViews = {nullptr};
				framebufferDescriptor.extent = swapchain->getExtent();
				framebufferDescriptor.layerCount = 1;

				for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
				}
			}

		private:
			struct Vertex {
				svec2<float32> vertex;
			};

			struct Instance {
				vec4<float32> color;
			};

		private:
			static constexpr uint64 INSTANCE_COUNT = 2;
			static constexpr uint64 VERTEX_BUFFER_SIZE = 3 * INSTANCE_COUNT * sizeof(Vertex);
			static constexpr uint64 INSTANCE_BUFFER_SIZE = INSTANCE_COUNT * sizeof(Instance);

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> instanceBuffer;
	};
}