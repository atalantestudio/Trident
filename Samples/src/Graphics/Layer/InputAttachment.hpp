// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class SampleInputAttachmentRenderer : public Renderer {
		private:
			struct Vertex {
				vec2<float32> vertex;
				vec3<float32> color;
			};

		public:
			void build() override {
				renderPass = createRenderPass();

				inputDescriptorSetLayout = createInputDescriptorSetLayout();

				// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(inputDescriptorSetLayout);

				inputDescriptorSet = descriptorSets[0];

				postResize();

				trianglePipelineLayout = createTrianglePipelineLayout();
				presentPipelineLayout = createPresentPipelineLayout();

				trianglePipeline = createTrianglePipeline();
				presentPipeline = createPresentPipeline();

				vertexBuffer = createVertexBuffer();
			}

			void encodeOneTimeCommands() const override {
				const ref<Buffer> stagingBuffer = createDynamicBuffer({
					.size = vertexBuffer->getSize(),
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
				const BufferView<Vertex> vertexView = stagingBuffer->createView<Vertex>(0, vertexBuffer->getSize());

				Vertex* vertexMappedRange = vertexView.map();
				vertexMappedRange[0] = {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}};
				vertexMappedRange[1] = {{ 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}};
				vertexMappedRange[2] = {{ 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}};

				stagingBuffer->unmap();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Copy buffers",
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
				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.x);
				const float32 height = static_cast<float32>(swapchainExtent.y);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffers[getSwapchainImageIndex()],
					.clearValues = {
						{0.0f, 0.0f, 0.0f, 1.0f},
						{0.0f, 0.0f, 0.0f, 1.0f},
					},
				});
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				{
					commandEncoder->bindGraphicsPipeline(trianglePipeline);
					commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
					commandEncoder->draw(3, 1, 0, 0);
				}

				commandEncoder->nextSubpass();

				// TODO(graphics/gl)
				// This pipeline won't write anything to the swapchain image because `gl_VertexIndex` is not incremented properly.
				// Tested on HP Victus, RTX 3050, OpenGL.
				{
					commandEncoder->bindGraphicsPipeline(presentPipeline);
					commandEncoder->bindDescriptorSets(presentPipelineLayout, 0, inputDescriptorSet);
					commandEncoder->draw(3, 1, 0, 0);
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				postResize();
			}

			void postResize() {
				inputImage = createInputImage();
				inputImageView = createInputImageView();

				createFramebuffers();

				updateImageDescriptorSet();
			}

		private:
			ref<RenderPass> createRenderPass() const {
				return Renderer::createRenderPass({
					.attachments = {
						// Swapchain
						{
							.format = getSurface()->getFormat(),
							.sampleCount = ImageSampleCount::x1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::STORE,
							.finalLayout = ImageLayout::PRESENT_SRC,
						},

						// Input
						{
							.format = getSurface()->getFormat(),
							.sampleCount = ImageSampleCount::x1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::UNDEFINED,
							.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
					.subpasses = {
						// Triangle
						{
							.colorAttachmentReferences = {
								{
									.index = 1,
									.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
								},
							},
						},

						// Present
						{
							.colorAttachmentReferences = {
								{
									.index = 0,
									.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
								},
							},
							.inputAttachmentReferences = {
								{
									.index = 1,
									.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
								},
							},
						},
					},
				});
			}

			ref<Image> createInputImage() const {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();

				return renderPass->createInputAttachmentImage(
					{
						.type = ImageType::TYPE_2D,
						.extent = {swapchainExtent.x, swapchainExtent.y, 1},
						.mipLevelCount = 1,
						.layerCount = 1,
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Input",
						#endif
					},
					1
				);
			}

			ref<ImageView> createInputImageView() {
				return inputImage->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Input view",
					#endif
				});
			}

			void createFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor = {
					.renderPass = renderPass,
					.imageViews = {nullptr, inputImageView},
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

			ref<DescriptorSetLayout> createInputDescriptorSetLayout() const {
				const DescriptorSetLayoutBinding bindings[] = {
					// Input
					{
						.descriptorType = DescriptorType::INPUT_ATTACHMENT,
						.descriptorCount = 1,
						.stageFlags = ShaderStage::FRAGMENT,
					},
				};

				return createDescriptorSetLayout({
					.bindings = bindings,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Input",
					#endif
				});
			}

			void updateImageDescriptorSet() {
				const DescriptorImage inputDescriptorImage = {
					.imageView = inputImageView,
					.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.sampler = nullptr,
				};

				const WriteDescriptorSet inputWriteDescriptorSet = {
					.descriptorSet = inputDescriptorSet,
					.bindingIndex = 0,
					.descriptorImages = {inputDescriptorImage},
				};

				getDevice()->updateDescriptorSets(inputWriteDescriptorSet);
			}

			ref<PipelineLayout> createTrianglePipelineLayout() const {
				return createPipelineLayout({
					.descriptorSetLayouts = {},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Triangle",
					#endif
				});
			}

			ref<PipelineLayout> createPresentPipelineLayout() const {
				return createPipelineLayout({
					.descriptorSetLayouts = {inputDescriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Present",
					#endif
				});
			}

			ref<GraphicsPipeline> createTrianglePipeline() const {
				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<decltype(Vertex::vertex)>();
				vertexInputBinding.registerAttribute<decltype(Vertex::color)>();

				const ColorBlendAttachment colorBlendAttachments[] = {
					{
						.blend = false,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = trianglePipelineLayout,
					.vertexInputBindings = vertexInputBinding,
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Triangle vertex",
							#endif
						},
						"Sample/Triangle/Triangle.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Triangle fragment",
							#endif
						},
						"Sample/Triangle/Triangle.frag"
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
						.depthTest = false,
					},
					.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
					.renderPass = renderPass,
					.subpassIndex = 0,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Triangle",
					#endif
				});
			}

			ref<GraphicsPipeline> createPresentPipeline() const {
				const ColorBlendAttachment colorBlendAttachments[] = {
					{
						.blend = false,
					},
				};

				view<char8> vertexShaderModulePath;
				view<char8> fragmentShaderModulePath;

				switch (getGraphicsApi()) {
					case GraphicsAPI::GL: {
						// NOTE: subpassInput and subpassLoad are not supported in plain GLSL.
						vertexShaderModulePath = "Sample/InputAttachment/InputAttachmentGL.vert";
						fragmentShaderModulePath = "Sample/InputAttachment/InputAttachmentGL.frag";

						break;
					}
					default: {
						vertexShaderModulePath = "Sample/InputAttachment/InputAttachment.vert";
						fragmentShaderModulePath = "Sample/InputAttachment/InputAttachment.frag";

						break;
					}
				}

				return createGraphicsPipeline({
					.pipelineLayout = presentPipelineLayout,
					.vertexInputBindings = {},
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Present vertex",
							#endif
						},
						vertexShaderModulePath
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Present fragment",
							#endif
						},
						fragmentShaderModulePath
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
						.depthTest = false,
					},
					.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
					.renderPass = renderPass,
					.subpassIndex = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Present",
					#endif
				});
			}

			ref<Buffer> createVertexBuffer() const {
				return createDynamicBuffer({
					.size = 3 * sizeof(Vertex),
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

		private:
			ref<RenderPass> renderPass;

			ref<Image> inputImage;
			ref<ImageView> inputImageView;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> inputDescriptorSetLayout;

			ref<DescriptorSet> inputDescriptorSet;

			ref<PipelineLayout> trianglePipelineLayout;
			ref<PipelineLayout> presentPipelineLayout;

			ref<GraphicsPipeline> trianglePipeline;
			ref<GraphicsPipeline> presentPipeline;

			ref<Buffer> vertexBuffer;
	};

	DEFINE_SAMPLE_LAYER(SampleInputAttachment);
}