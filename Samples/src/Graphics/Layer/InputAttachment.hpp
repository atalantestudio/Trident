// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class SampleInputAttachmentRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> vertex;
				vec3<float32> color;
			};

		private:
			static constexpr uint32 VERTEX_COUNT = 3;
			static constexpr uint32 WRITE_IMAGE_SUBPASS_INDEX = 0;
			static constexpr uint32 READ_IMAGE_SUBPASS_INDEX = 1;

		public:
			void build() override {
				setupRenderPass();

				recreateImage();
				recreateImageView();
				recreateFramebuffers();

				createImageDescriptorSetLayout();
				allocateImageDescriptorSet();
				updateImageDescriptorSet();

				createWriteImagePipelineLayout();
				createWriteImageGraphicsPipeline();

				createReadImagePipelineLayout();
				createReadImageGraphicsPipeline();

				createVertexBuffer();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffers[getSwapchainImageIndex()],
					.clearValues = {
						{0.0f, 0.0f, 0.0f, 1.0f},
						{0.0f, 0.0f, 0.0f, 1.0f},
					},
				});
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				// Write image subpass.
				{
					commandEncoder->bindGraphicsPipeline(writeImageGraphicsPipeline);
					commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
					commandEncoder->draw(3, 1, 0, 0);
				}

				commandEncoder->nextSubpass();

				// Read image subpass.
				{
					commandEncoder->bindGraphicsPipeline(readImageGraphicsPipeline);
					commandEncoder->bindDescriptorSets(readImagePipelineLayout, 0, imageDescriptorSet);
					commandEncoder->draw(3, 1, 0, 0);
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateImage();
				recreateImageView();
				recreateFramebuffers();

				updateImageDescriptorSet();
			}

		private:
			void setupRenderPass() {
				renderPass = createRenderPass({
					.attachments = {
						// Swapchain
						{
							.format = getSurface()->getFormat(),
							.sampleCount = 1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::STORE,
							.finalLayout = ImageLayout::PRESENT_SRC,
						},

						// Input
						{
							.format = getSurface()->getFormat(),
							.sampleCount = 1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::UNDEFINED,
							.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
					.subpasses = {
						// Write input
						createSubpass({
							.colorAttachmentReferences = {
								// Write input
								{
									.index = 1,
									.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
								},
							},
						}),

						// Read input
						createSubpass({
							.colorAttachmentReferences = {
								// Write swapchain
								{
									.index = 0,
									.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
								},
							},
							.inputAttachmentReferences = {
								// Read input
								{
									.index = 1,
									.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
								},
							},
						}),
					},
				});
			}

			void recreateImage() {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();

				image = renderPass->createInputAttachmentImage(
					{
						.type = ImageType::TEXTURE_2D,
						.extent = {swapchainExtent.width, swapchainExtent.height, 1},
						.mipLevelCount = 1,
						.layerCount = 1,
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Image",
						#endif
					},
					1
				);
			}

			void recreateImageView() {
				imageView = image->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image view",
					#endif
				});
			}

			void recreateFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor = {
					.renderPass = renderPass,
					.imageViews = {nullptr, imageView},
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

			void createImageDescriptorSetLayout() {
				const DescriptorSetLayoutBinding imageBinding = {
					.descriptorType = DescriptorType::INPUT_ATTACHMENT,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				};

				imageDescriptorSetLayout = createDescriptorSetLayout({imageBinding});
			}

			void allocateImageDescriptorSet() {
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(imageDescriptorSetLayout);

				imageDescriptorSet = descriptorSets[0];
			}

			void updateImageDescriptorSet() {
				const DescriptorImage descriptorImage = {
					.imageView = imageView,
					.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.sampler = nullptr,
				};

				const WriteDescriptorSet imageWriteDescriptorSet = {
					.descriptorSet = imageDescriptorSet,
					.bindingIndex = 0,
					.descriptorImages = {descriptorImage},
				};

				getDevice()->updateDescriptorSets(imageWriteDescriptorSet);
			}

			void createWriteImagePipelineLayout() {
				writeImagePipelineLayout = createPipelineLayout({
					.descriptorSetLayouts = {},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image write",
					#endif
				});
			}

			void createWriteImageGraphicsPipeline() {
				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<svec2<float32>>();
				vertexInputBinding.registerAttribute<vec3<float32>>();

				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = false,
					},
				};

				writeImageGraphicsPipeline = createGraphicsPipeline({
					.pipelineLayout = writeImagePipelineLayout,
					.vertexInputBindings = {vertexInputBinding},
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Image write vertex",
							#endif
						},
						"Sample/Triangle/Triangle.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Image write fragment",
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
					.subpassIndex = WRITE_IMAGE_SUBPASS_INDEX,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image write",
					#endif
				});
			}

			void createReadImagePipelineLayout() {
				readImagePipelineLayout = createPipelineLayout({
					.descriptorSetLayouts = {imageDescriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image read",
					#endif
				});
			}

			void createReadImageGraphicsPipeline() {
				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = false,
					},
				};

				readImageGraphicsPipeline = createGraphicsPipeline({
					.pipelineLayout = readImagePipelineLayout,
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Image read vertex",
							#endif
						},
						"Sample/InputAttachment/InputAttachment.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Image read fragment",
							#endif
						},
						"Sample/InputAttachment/InputAttachment.frag"
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
					.subpassIndex = READ_IMAGE_SUBPASS_INDEX,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image read",
					#endif
				});
			}

			void createVertexBuffer() {
				const ref<Buffer<Vertex>> vertexStagingBuffer = createStagingBuffer<Vertex>(3 * sizeof(Vertex));

				Vertex* vertexStagingBufferMappedRange = vertexStagingBuffer->map(0, vertexStagingBuffer->getSize());
				vertexStagingBufferMappedRange[0] = {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}};
				vertexStagingBufferMappedRange[1] = {{0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}};
				vertexStagingBufferMappedRange[2] = {{0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}};

				vertexStagingBuffer->unmap();

				vertexBuffer = createBuffer<Vertex>({
					.size = vertexStagingBuffer->getSize(),
					.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});

				const BufferCopyRegion bufferCopyRegion = {
					.sourceOffset = 0,
					.destinationOffset = 0,
					.size = vertexBuffer->getSize(),
				};

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Copy buffer",
					#endif
				});

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, bufferCopyRegion);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

		private:
			ref<RenderPass> renderPass;
			ref<Image> image;
			ref<ImageView> imageView;
			sequence<ref<Framebuffer>> framebuffers;
			ref<DescriptorSetLayout> imageDescriptorSetLayout;
			ref<DescriptorSet> imageDescriptorSet;
			ref<PipelineLayout> writeImagePipelineLayout;
			ref<PipelineLayout> readImagePipelineLayout;
			ref<GraphicsPipeline> writeImageGraphicsPipeline;
			ref<GraphicsPipeline> readImageGraphicsPipeline;
			ref<Buffer<Vertex>> vertexBuffer;
	};

	DEFINE_SAMPLE_LAYER(SampleInputAttachment);
}