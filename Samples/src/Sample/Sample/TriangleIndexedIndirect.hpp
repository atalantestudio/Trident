#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class SampleRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> position;
				vec3<float32> color;
			};

		private:
			static constexpr uint32 VERTEX_BUFFER_SIZE = 3 * sizeof(Vertex);
			static constexpr uint32 INDEX_BUFFER_SIZE = 3 * sizeof(uint16);
			static constexpr uint32 INDIRECT_BUFFER_SIZE = sizeof(DrawIndexedIndirectCommand);

		public:
			void build() override {
				setupRenderPass();
				recreateFramebuffers();
				setupRenderPipeline();
				createBuffers();
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
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
				commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
				commandEncoder->drawIndexedIndirect(indirectBuffer, 0, 1, 0);
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

			void setupRenderPipeline() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};

				const ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<svec2<float32>>();
				vertexInputBinding.registerAttribute<vec3<float32>>();

				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				const ref<ShaderModule> vertexShaderModule = loadShaderModule("Base/Vertex2DRGB.vert", vertexShaderModuleDescriptor);
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Base/ColorRGB.frag", fragmentShaderModuleDescriptor);

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				RenderPipelineDescriptor renderPipelineDescriptor{};
				renderPipelineDescriptor.pipelineLayout = pipelineLayout;
				renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
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

				renderPipeline = createRenderPipeline(renderPipelineDescriptor);
			}

			void createBuffers() {
				const ref<Buffer> stagingBuffer = createStagingBuffer(VERTEX_BUFFER_SIZE + INDEX_BUFFER_SIZE + INDIRECT_BUFFER_SIZE);

				uint8* stagingBufferMappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->size);

				Vertex* vertexStagingBufferMappedRange = pointer_cast<Vertex*>(stagingBufferMappedRange);
				vertexStagingBufferMappedRange[0].position = {-0.5f, -0.5f};
				vertexStagingBufferMappedRange[0].color = {1.0f, 0.0f, 0.0f};
				vertexStagingBufferMappedRange[1].position = {0.0f, 0.5f};
				vertexStagingBufferMappedRange[1].color = {0.0f, 1.0f, 0.0f};
				vertexStagingBufferMappedRange[2].position = {0.5f, -0.5f};
				vertexStagingBufferMappedRange[2].color = {0.0f, 0.0f, 1.0f};

				uint16* indexStagingBufferMappedRange = pointer_cast<uint16*>(stagingBufferMappedRange + VERTEX_BUFFER_SIZE);
				indexStagingBufferMappedRange[0] = 0;
				indexStagingBufferMappedRange[1] = 1;
				indexStagingBufferMappedRange[2] = 2;

				DrawIndexedIndirectCommand* indirectStagingBufferMappedRange = pointer_cast<DrawIndexedIndirectCommand*>(stagingBufferMappedRange + VERTEX_BUFFER_SIZE + INDEX_BUFFER_SIZE);
				indirectStagingBufferMappedRange[0].indexCount = 3;
				indirectStagingBufferMappedRange[0].instanceCount = 1;
				indirectStagingBufferMappedRange[0].indexOffset = 0;
				indirectStagingBufferMappedRange[0].vertexBias = 0;
				indirectStagingBufferMappedRange[0].instanceOffset = 0;

				stagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor;
				vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
				vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexBufferDescriptor.persistent = false;

				BufferDescriptor indexBufferDescriptor{};
				indexBufferDescriptor.size = INDEX_BUFFER_SIZE;
				indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
				indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				indexBufferDescriptor.persistent = false;

				BufferDescriptor indirectBufferDescriptor{};
				indirectBufferDescriptor.size = INDIRECT_BUFFER_SIZE;
				indirectBufferDescriptor.usage = BufferUsage::INDIRECT | BufferUsage::TRANSFER_DST;
				indirectBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				indirectBufferDescriptor.persistent = false;

				vertexBuffer = createBuffer(vertexBufferDescriptor);
				indexBuffer = createBuffer(indexBufferDescriptor);
				indirectBuffer = createBuffer(indirectBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->size;

				BufferCopyRegion indexBufferCopyRegion{};
				indexBufferCopyRegion.sourceOffset = vertexBuffer->size;
				indexBufferCopyRegion.destinationOffset = 0;
				indexBufferCopyRegion.size = indexBuffer->size;

				BufferCopyRegion indirectBufferCopyRegion{};
				indirectBufferCopyRegion.sourceOffset = vertexBuffer->size + indexBuffer->size;
				indirectBufferCopyRegion.destinationOffset = 0;
				indirectBufferCopyRegion.size = indirectBuffer->size;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, {indexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, indirectBuffer, {indirectBufferCopyRegion});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
				transferQueue->waitIdle();
			}

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

				for (uint32 framebufferIndex = 0; framebufferIndex < framebuffers.count(); framebufferIndex += 1) {
					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
				}
			}

		private:
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> indirectBuffer;
	};
}