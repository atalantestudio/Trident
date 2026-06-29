// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class SampleTriangleIndexedIndirectRenderer : public Renderer {
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
				renderPass = createRenderPass();

				createFramebuffers();
				setupGraphicsPipeline();
				createBuffers();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getSwapchainImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->bindGraphicsPipeline(graphicsPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
				commandEncoder->drawIndexedIndirect(indirectBuffer, 0, 1, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				createFramebuffers();
			}

		private:
			ref<RenderPass> createRenderPass() const {
				return Renderer::createRenderPass({
					.attachments = {
						// Swapchain
						{
							.format = getSurface()->getFormat(),
							.sampleCount = 1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::STORE,
							.finalLayout = ImageLayout::PRESENT_SRC,
						},
					},
					.subpasses = {
						{
							.colorAttachmentReferences = {
								{
									.index = 0,
									.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
								},
							},
						},
					},
				});
			}

			void setupGraphicsPipeline() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};

				#if defined(DEBUG_OBJECT_NAME)
					pipelineLayoutDescriptor.name = "Pipeline layout";
				#endif

				const ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<svec2<float32>>();
				vertexInputBinding.registerAttribute<vec3<float32>>();

				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				#if defined(DEBUG_OBJECT_NAME)
					vertexShaderModuleDescriptor.name = "Vertex shader module";
				#endif

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				#if defined(DEBUG_OBJECT_NAME)
					fragmentShaderModuleDescriptor.name = "Fragment shader module";
				#endif

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/Triangle/Triangle.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/Triangle/Triangle.frag");

				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = false,
					},
				};

				GraphicsPipelineDescriptor graphicsPipelineDescriptor{};
				graphicsPipelineDescriptor.pipelineLayout = pipelineLayout;
				graphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
				graphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				graphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				graphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				graphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
				graphicsPipelineDescriptor.cullMode = CullMode::BACK;
				graphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				graphicsPipelineDescriptor.lineWidth = 1.0f;
				graphicsPipelineDescriptor.minSampleShading = 0.0f;
				graphicsPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
				graphicsPipelineDescriptor.depthStencilState.depthTest = false;
				graphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				graphicsPipelineDescriptor.renderPass = renderPass;
				graphicsPipelineDescriptor.subpassIndex = 0;

				#if defined(DEBUG_OBJECT_NAME)
					graphicsPipelineDescriptor.name = "Render pipeline";
				#endif

				graphicsPipeline = createGraphicsPipeline(graphicsPipelineDescriptor);
			}

			void createBuffers() {
				const ref<Buffer<uint8>> stagingBuffer = createStagingBuffer<uint8>(VERTEX_BUFFER_SIZE + INDEX_BUFFER_SIZE + INDIRECT_BUFFER_SIZE);

				uint8* stagingBufferMappedRange = stagingBuffer->map(0, stagingBuffer->getSize());

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

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
				vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexBufferDescriptor.persistent = false;

				#if defined(DEBUG_OBJECT_NAME)
					vertexBufferDescriptor.name = "Vertex buffer";
				#endif

				BufferDescriptor indexBufferDescriptor{};
				indexBufferDescriptor.size = INDEX_BUFFER_SIZE;
				indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
				indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				indexBufferDescriptor.persistent = false;

				#if defined(DEBUG_OBJECT_NAME)
					indexBufferDescriptor.name = "Index buffer";
				#endif

				BufferDescriptor indirectBufferDescriptor{};
				indirectBufferDescriptor.size = INDIRECT_BUFFER_SIZE;
				indirectBufferDescriptor.usage = BufferUsage::INDIRECT | BufferUsage::TRANSFER_DST;
				indirectBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				indirectBufferDescriptor.persistent = false;

				#if defined(DEBUG_OBJECT_NAME)
					indirectBufferDescriptor.name = "Indirect buffer";
				#endif

				vertexBuffer = createBuffer<Vertex>(vertexBufferDescriptor);
				indexBuffer = createBuffer<uint16>(indexBufferDescriptor);
				indirectBuffer = createBuffer<DrawIndexedIndirectCommand>(indirectBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->getSize();

				BufferCopyRegion indexBufferCopyRegion{};
				indexBufferCopyRegion.sourceOffset = vertexBuffer->getSize();
				indexBufferCopyRegion.destinationOffset = 0;
				indexBufferCopyRegion.size = indexBuffer->getSize();

				BufferCopyRegion indirectBufferCopyRegion{};
				indirectBufferCopyRegion.sourceOffset = vertexBuffer->getSize() + indexBuffer->getSize();
				indirectBufferCopyRegion.destinationOffset = 0;
				indirectBufferCopyRegion.size = indirectBuffer->getSize();

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				#if defined(DEBUG_OBJECT_NAME)
					commandBufferDescriptor.name = "Buffer copy command buffer";
				#endif

				const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, {indexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, indirectBuffer, {indirectBufferCopyRegion});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit(commandBuffer);
				transferQueue->waitIdle();
			}

			void createFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor = {
					.renderPass = renderPass,
					.imageViews = {nullptr},
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

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

		private:
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<GraphicsPipeline> graphicsPipeline;
			ref<Buffer<Vertex>> vertexBuffer;
			ref<Buffer<uint16>> indexBuffer;
			ref<Buffer<DrawIndexedIndirectCommand>> indirectBuffer;
	};

	DEFINE_SAMPLE_LAYER(SampleTriangleIndexedIndirect);
}