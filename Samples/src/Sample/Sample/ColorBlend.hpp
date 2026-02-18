// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	class SampleRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> position;
			};

			struct Instance {
				vec4<float32> color;
			};

		private:
			static constexpr uint32 INSTANCE_COUNT = 2;
			static constexpr uint64 VERTEX_BUFFER_SIZE = 3 * INSTANCE_COUNT * sizeof(Vertex);
			static constexpr uint64 INSTANCE_BUFFER_SIZE = INSTANCE_COUNT * sizeof(Instance);

		public:
			void build() override {
				setupRenderPass();
				recreateFramebuffers();
				setupRenderPipeline();
				createBuffers();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));

				for (uint32 instanceIndex = 0; instanceIndex < INSTANCE_COUNT; instanceIndex += 1) {
					commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance), sizeof(Instance));
					commandEncoder->draw(3, 1, instanceIndex * 3, 0);
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
			}

		private:
			void setupRenderPass() {
				Attachment swapchainAttachment{};
				swapchainAttachment.format = getSurface()->getFormat();
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

			void recreateFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr};
				framebufferDescriptor.extent = swapchain->getExtent();

				#if defined(BUILD_MODE_DEBUG)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

				while (framebufferIndex < framebuffers.count()) {
					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("In flight framebuffer []", framebufferIndex);

						SET_DEBUG_NAME(framebufferDescriptor, name);
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			void setupRenderPipeline() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};

				SET_DEBUG_NAME(pipelineLayoutDescriptor, "Pipeline layout");

				const ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<svec2<float32>>();

				VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
				instanceInputBinding.registerAttribute<vec4<float32>>();

				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Vertex shader module");

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Fragment shader module");

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/ColorBlend/ColorBlend.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/ColorBlend/ColorBlend.frag");

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = true;
				colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
				colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

				RenderPipelineDescriptor renderPipelineDescriptor{};
				renderPipelineDescriptor.pipelineLayout = pipelineLayout;
				renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
				renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
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

				SET_DEBUG_NAME(renderPipelineDescriptor, "Render pipeline");

				renderPipeline = createRenderPipeline(renderPipelineDescriptor);
			}

			void createBuffers() {
				const ref<Buffer> stagingBuffer = createStagingBuffer(VERTEX_BUFFER_SIZE + INSTANCE_BUFFER_SIZE);

				uint8* stagingBufferMappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->getSize());

				Vertex* vertexStagingBufferMappedRange = pointer_cast<Vertex*>(stagingBufferMappedRange);
				vertexStagingBufferMappedRange[0].position = {-0.75f,  0.75f};
				vertexStagingBufferMappedRange[1].position = { 0.25f,  0.75f};
				vertexStagingBufferMappedRange[2].position = {-0.75f, -0.75f};
				vertexStagingBufferMappedRange[3].position = {-0.5f,   0.5f};
				vertexStagingBufferMappedRange[4].position = { 0.5f,   0.5f};
				vertexStagingBufferMappedRange[5].position = {-0.5f,  -0.5f};

				Instance* instanceStagingBufferMappedRange = pointer_cast<Instance*>(stagingBufferMappedRange + VERTEX_BUFFER_SIZE);
				instanceStagingBufferMappedRange[0].color = {1.0f, 1.0f, 1.0f, 1.0f};
				instanceStagingBufferMappedRange[1].color = {1.0f, 0.2f, 0.0f, 0.5f};

				stagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
				vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				SET_DEBUG_NAME(vertexBufferDescriptor, "Vertex buffer");

				BufferDescriptor instanceBufferDescriptor{};
				instanceBufferDescriptor.size = INSTANCE_BUFFER_SIZE;
				instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				SET_DEBUG_NAME(instanceBufferDescriptor, "Instance buffer");

				vertexBuffer = createBuffer(vertexBufferDescriptor);
				instanceBuffer = createBuffer(instanceBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->getSize();

				BufferCopyRegion instanceBufferCopyRegion{};
				instanceBufferCopyRegion.sourceOffset = vertexBuffer->getSize();
				instanceBufferCopyRegion.destinationOffset = 0;
				instanceBufferCopyRegion.size = instanceBuffer->getSize();

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Buffer copy command buffer");

				const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, {instanceBufferCopyRegion});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit({commandBuffer});
				transferQueue->waitIdle();
			}

		private:
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> instanceBuffer;
	};
}