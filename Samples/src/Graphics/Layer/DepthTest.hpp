// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class SampleDepthTestRenderer : public Renderer {
		private:
			struct Vertex {
				vec3<float32> position;
			};

			struct Instance {
				vec3<float32> color;
			};

		private:
			static constexpr uint64 INSTANCE_COUNT = 2;
			static constexpr uint64 VERTEX_BUFFER_SIZE = 3 * INSTANCE_COUNT * sizeof(Vertex);
			static constexpr uint64 INSTANCE_BUFFER_SIZE = INSTANCE_COUNT * sizeof(Instance);

		public:
			void build() override {
				renderPass = createRenderPass();

				recreateDepthStencilImage();
				recreateDepthStencilImageView();
				recreateFramebuffers();

				setupGraphicsPipeline();

				vertexBuffer = createVertexBuffer();
				instanceBuffer = createInstanceBuffer();
			}

			void encodeOneTimeCommands() const override {
				const ref<Buffer> stagingBuffer = createDynamicBuffer({
					.size = vertexBuffer->getSize() + instanceBuffer->getSize(),
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
				const BufferView<Vertex> vertexView = stagingBuffer->createView<Vertex>(0, vertexBuffer->getSize());
				const BufferView<Instance> instanceView = stagingBuffer->createView<Instance>(vertexBuffer->getSize(), instanceBuffer->getSize());

				Vertex* vertexMappedRange = vertexView.map();
				vertexMappedRange[0] = {{-0.75f,  0.75f,  1.0f}};
				vertexMappedRange[1] = {{ 0.25f,  0.75f,  1.0f}};
				vertexMappedRange[2] = {{-0.75f, -0.75f,  1.0f}};
				vertexMappedRange[3] = {{-0.5f,   0.5f,   0.0f}};
				vertexMappedRange[4] = {{ 0.5f,   0.5f,   0.0f}};
				vertexMappedRange[5] = {{-0.5f,  -0.5f,   0.0f}};

				Instance* instanceMappedRange = instanceView.map();
				instanceMappedRange[0] = {{1.0f, 1.0f, 1.0f}};
				instanceMappedRange[1] = {{1.0f, 0.2f, 0.0f}};

				stagingBuffer->unmap();

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->getSize();

				BufferCopyRegion instanceBufferCopyRegion{};
				instanceBufferCopyRegion.sourceOffset = vertexBuffer->getSize();
				instanceBufferCopyRegion.destinationOffset = 0;
				instanceBufferCopyRegion.size = instanceBuffer->getSize();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Copy buffers",
					#endif
				});

				commandEncoder->begin();

				BufferCopyRegion bufferCopyRegion = {
					.sourceOffset = vertexView.getOffset(),
					.destinationOffset = 0,
					.size = vertexView.getSize(),
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegion);

				bufferCopyRegion = {
					.sourceOffset = instanceView.getOffset(),
					.destinationOffset = 0,
					.size = instanceView.getSize(),
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, bufferCopyRegion);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getSwapchainImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindGraphicsPipeline(graphicsPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				for (uint32 instanceIndex = 0; instanceIndex < INSTANCE_COUNT; instanceIndex += 1) {
					commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance));
					commandEncoder->draw(3, 1, instanceIndex * 3, 0);
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateDepthStencilImage();
				recreateDepthStencilImageView();
				recreateFramebuffers();
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

			void recreateDepthStencilImage() {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();

				AttachmentImageDescriptor depthStencilAttachmentImageDescriptor{};
				depthStencilAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
				depthStencilAttachmentImageDescriptor.extent.width = swapchainExtent.width;
				depthStencilAttachmentImageDescriptor.extent.height = swapchainExtent.height;
				depthStencilAttachmentImageDescriptor.extent.depth = 1;
				depthStencilAttachmentImageDescriptor.mipLevelCount = 1;
				depthStencilAttachmentImageDescriptor.layerCount = 1;

				#if defined(DEBUG_OBJECT_NAME)
					depthStencilAttachmentImageDescriptor.name = "Depth stencil image";
				#endif

				depthStencilImage = renderPass->createDepthStencilAttachmentImage(depthStencilAttachmentImageDescriptor, 0);
			}

			void recreateDepthStencilImageView() {
				ImageViewDescriptor depthStencilImageViewDescriptor{};
				depthStencilImageViewDescriptor.mipLevelOffset = 0;
				depthStencilImageViewDescriptor.mipLevelCount = 1;
				depthStencilImageViewDescriptor.layerOffset = 0;
				depthStencilImageViewDescriptor.layerCount = 1;

				#if defined(DEBUG_OBJECT_NAME)
					depthStencilImageViewDescriptor.name = "Depth stencil image view";
				#endif

				depthStencilImageView = depthStencilImage->createView(depthStencilImageViewDescriptor);
			}

			void recreateFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr, depthStencilImageView};
				framebufferDescriptor.extent = swapchain->getExtent();

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

			void setupGraphicsPipeline() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};

				#if defined(DEBUG_OBJECT_NAME)
					pipelineLayoutDescriptor.name = "Pipeline layout";
				#endif

				const ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				VertexInputBinding vertexInputBindings[2] = {
					VertexInputBinding(InputRate::VERTEX),
					VertexInputBinding(InputRate::INSTANCE),
				};

				vertexInputBindings[0].registerAttribute<vec3<float32>>();

				vertexInputBindings[1].registerAttribute<vec3<float32>>();

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

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/DepthTest/DepthTest.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/DepthTest/DepthTest.frag");

				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = false,
					},
				};

				GraphicsPipelineDescriptor graphicsPipelineDescriptor{};
				graphicsPipelineDescriptor.pipelineLayout = pipelineLayout;
				graphicsPipelineDescriptor.vertexInputBindings = view<VertexInputBinding>(vertexInputBindings, 2);
				graphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				graphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				graphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				graphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
				graphicsPipelineDescriptor.cullMode = CullMode::BACK;
				graphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				graphicsPipelineDescriptor.lineWidth = 1.0f;
				graphicsPipelineDescriptor.minSampleShading = 0.0f;
				graphicsPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
				graphicsPipelineDescriptor.depthStencilState.depthTest = true;
				graphicsPipelineDescriptor.depthStencilState.depthWrite = true;
				graphicsPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
				graphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				graphicsPipelineDescriptor.renderPass = renderPass;
				graphicsPipelineDescriptor.subpassIndex = 0;

				#if defined(DEBUG_OBJECT_NAME)
					graphicsPipelineDescriptor.name = "Render pipeline";
				#endif

				graphicsPipeline = createGraphicsPipeline(graphicsPipelineDescriptor);
			}

			ref<Buffer> createVertexBuffer() const {
				return createDynamicBuffer({
					.size = VERTEX_BUFFER_SIZE,
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

			ref<Buffer> createInstanceBuffer() const {
				return createDynamicBuffer({
					.size = INSTANCE_BUFFER_SIZE,
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Instance",
					#endif
				});
			}

		private:
			ref<RenderPass> renderPass;
			ref<Image> depthStencilImage;
			ref<ImageView> depthStencilImageView;
			sequence<ref<Framebuffer>> framebuffers;
			ref<GraphicsPipeline> graphicsPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> instanceBuffer;
	};

	DEFINE_SAMPLE_LAYER(SampleDepthTest);
}