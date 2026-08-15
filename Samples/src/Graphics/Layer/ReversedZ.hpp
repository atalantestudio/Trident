// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class ReversedZSampleRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> position;
			};

			struct Instance {
				vec4<float32> color;
				vec3<float32> translation;
			};

		private:
			static constexpr uint64 VERTEX_SIZE = sizeof(Vertex) * 3;
			static constexpr uint64 INSTANCE_SIZE = sizeof(Instance) * 3;

		public:
			void build() override {
				renderPass = createRenderPass();
				depthStencilImage = createDepthStencilImage();
				depthStencilImageView = createDepthStencilImageView();
				pipelineLayout = createPipelineLayout();
				pipeline = createPipeline();
				vertexBuffer = createVertexBuffer();
				stagingBuffer = createStagingBuffer();

				createFramebuffers();
			}

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override {
				return {
					.buffers = {vertexBuffer, stagingBuffer},
					.images = {},
				};
			}

			void encodeOneTimeCommands() const override {
				const BufferView<Vertex> vertexView = stagingBuffer->createView<Vertex>(0, VERTEX_SIZE);
				const BufferView<Instance> instanceView = stagingBuffer->createView<Instance>(vertexView.getOffset() + vertexView.getSize(), INSTANCE_SIZE);

				Vertex* vertexMappedRange = vertexView.map();
				vertexMappedRange[0] = {{-0.5f, -0.5f}};
				vertexMappedRange[1] = {{ 0.0f,  0.5f}};
				vertexMappedRange[2] = {{ 0.5f, -0.5f}};

				Instance* instanceMappedRange = instanceView.map();
				instanceMappedRange[0] = {Color(0x8e1a23ff).value, {-0.1f, -0.1f,  1.0f}};
				instanceMappedRange[1] = {Color(0x80b5e7ff).value, { 0.0f,  0.0f,  0.3f}};
				instanceMappedRange[2] = {Color(0xeda041ff).value, { 0.1f,  0.1f,  0.7f}};

				stagingBuffer->unmap();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "One-time commands",
					#endif
				});

				commandEncoder->begin();

				const BufferCopyRegion bufferCopyRegion = {
					.sourceOffset = 0,
					.destinationOffset = 0,
					.size = vertexBuffer->getSize(),
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegion);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const ref<Framebuffer> framebuffer = framebuffers[getSwapchainImageIndex()];
				const Extent2D extent = framebuffer->getExtent();
				const float32 width = static_cast<float32>(extent.width);
				const float32 height = static_cast<float32>(extent.height);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffer,
					.clearValues = {
						{0.0f, 0.0f, 0.0f, 1.0f},
						{
							.depth = 0.0f,
							.stencil = 0,
						},
					},
				});
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindGraphicsPipeline(pipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->bindVertexBuffer(1, vertexBuffer, VERTEX_SIZE);
				commandEncoder->draw(3, 3, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				depthStencilImage = createDepthStencilImage();
				depthStencilImageView = createDepthStencilImageView();

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

						// Depth stencil
						{
							.format = ImageFormat::D32_SFLOAT,
							.sampleCount = 1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::UNDEFINED,
							.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
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
							.depthStencilAttachmentReference = {
								.index = 1,
								.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
							},
						},
					},
				});
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

			void createFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor = {
					.renderPass = renderPass,
					.imageViews = {nullptr, depthStencilImageView},
					.extent = swapchain->getExtent(),
				};

				#if defined(DEBUG_OBJECT_NAME)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

				while (framebufferIndex < framebuffers.count()) {
					#if defined(DEBUG_OBJECT_NAME)
						name = Logger::format("In flight []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			ref<PipelineLayout> createPipelineLayout() const {
				return Renderer::createPipelineLayout({
					.descriptorSetLayouts = {},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Pipeline layout",
					#endif
				});
			}

			ref<GraphicsPipeline> createPipeline() const {
				VertexInputBinding vertexInputBindings[2] = {
					VertexInputBinding(InputRate::VERTEX),
					VertexInputBinding(InputRate::INSTANCE),
				};

				vertexInputBindings[0].registerAttribute<decltype(Vertex::position)>();
				vertexInputBindings[1].registerAttribute<decltype(Instance::color)>();
				vertexInputBindings[1].registerAttribute<decltype(Instance::translation)>();

				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = false,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = pipelineLayout,
					.vertexInputBindings = view<VertexInputBinding>(vertexInputBindings, 2),
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Vertex",
							#endif
						},
						"Sample/DepthTest/DepthTest.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Fragment",
							#endif
						},
						"Sample/DepthTest/DepthTest.frag"
					),
					.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
					.polygonMode = PolygonMode::FILL,
					.cullMode = CullMode::BACK,
					.frontFace = FrontFace::CLOCKWISE,
					.lineWidth = 1.0f,
					.minSampleShading = 0.0f,
					.colorBlendState = {
						.attachments = colorBlendAttachments,
					},
					.depthStencilState = {
						.depthTest = true,
						.depthWrite = true,
						.compareOperation = CompareOperation::GREATER,
					},
					.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
					.renderPass = renderPass,
					.subpassIndex = 0,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Pipeline",
					#endif
				});
			}

			ref<Buffer> createVertexBuffer() const {
				return createStaticBuffer({
					.size = VERTEX_SIZE + INSTANCE_SIZE,
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

			ref<Buffer> createStagingBuffer() const {
				return createStaticBuffer({
					.size = vertexBuffer->getSize(),
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
			}

		private:
			ref<Image> depthStencilImage;
			ref<ImageView> depthStencilImageView;
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> pipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> stagingBuffer;
	};

	DEFINE_SAMPLE_LAYER(ReversedZSample);
}