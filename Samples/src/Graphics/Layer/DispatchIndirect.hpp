// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class DispatchIndirectSampleRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> position;
			};

		public:
			DispatchIndirectSampleRenderer() {}

			~DispatchIndirectSampleRenderer() {
				vertexBuffer.~ref();
				indirectBuffer.~ref();
				stagingBuffer.~ref();
			}

			void build() override {
				renderPass = createRenderPass();
				outputDescriptorSetLayout = createOutputDescriptorSetLayout();

				// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(outputDescriptorSetLayout);

				outputDescriptorSet = descriptorSets[0];
				pipelineLayout = createPipelineLayout();
				pipeline = createPipeline();
				vertexBuffer = createVertexBuffer();
				indirectBuffer = createIndirectBuffer();
				stagingBuffer = createStagingBuffer();
				outputImage = createOutputImage();
				sampler = createSampler();
			}

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override {
				return {
					.buffers = staticBuffers,
					.images = outputImage,
				};
			}

			void postBuild() override {
				outputImageView = createOutputImageView();

				createFramebuffers();
				updateDescriptorSets();
			}

			void encodeOneTimeCommands() const override {
				const BufferView<Vertex> vertexView = stagingBuffer->createView<Vertex>(0, vertexBuffer->getSize());
				const BufferView<DispatchIndirectCommand> indirectView = stagingBuffer->createView<DispatchIndirectCommand>(vertexView.getOffset() + vertexView.getSize(), sizeof(DispatchIndirectCommand));

				Vertex* vertexMappedRange = vertexView.map();
				vertexMappedRange[0] = {{-1.0f,  1.0f}};
				vertexMappedRange[1] = {{ 3.0f,  1.0f}};
				vertexMappedRange[2] = {{-1.0f, -3.0f}};

				DispatchIndirectCommand* indirectMappedRange = indirectView.map();
				indirectMappedRange->x = 2;
				indirectMappedRange->y = 2;
				indirectMappedRange->z = 1;

				stagingBuffer->unmap();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "One-time commands",
					#endif
				});

				commandEncoder->begin();

				const BufferCopyRegion bufferCopyRegions[2] = {
					{
						.sourceOffset = vertexView.getOffset(),
						.destinationOffset = 0,
						.size = vertexView.getSize(),
					},
					{
						.sourceOffset = indirectView.getOffset(),
						.destinationOffset = 0,
						.size = indirectView.getSize(),
					},
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegions[0]);
				commandEncoder->copyBufferToBuffer(stagingBuffer, indirectBuffer, bufferCopyRegions[1]);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();

				computeOutputImage();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const ref<Framebuffer> framebuffer = framebuffers[getSwapchainImageIndex()];
				const Extent2D extent = framebuffer->getExtent();
				const float32 width = static_cast<float32>(extent.width);
				const float32 height = static_cast<float32>(extent.height);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffer,
					.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}},
				});
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindGraphicsPipeline(pipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, outputDescriptorSet);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				createFramebuffers();
			}

		private:
			ref<RenderPass> createRenderPass() const {
				return Renderer::createRenderPass({
					.attachments = {
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
						name = Logger::format("In flight []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			ref<DescriptorSetLayout> createOutputDescriptorSetLayout() const {
				const DescriptorSetLayoutBinding bindings[] = {
					{
						.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
						.descriptorCount = 1,
						.stageFlags = ShaderStage::FRAGMENT,
					},
				};

				return createDescriptorSetLayout(bindings);
			}

			ref<PipelineLayout> createPipelineLayout() const {
				return Renderer::createPipelineLayout({
					.descriptorSetLayouts = {outputDescriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Pipeline layout",
					#endif
				});
			}

			ref<GraphicsPipeline> createPipeline() const {
				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<decltype(Vertex::position)>();

				const ColorBlendAttachment colorBlendAttachments[] = {
					{
						.blend = false,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = pipelineLayout,
					.vertexInputBindings = vertexInputBinding,
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Vertex",
							#endif
						},
						"Sample/ComputeShader/ComputeShader.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Fragment",
							#endif
						},
						"Sample/ComputeShader/ComputeShader.frag"
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
						.name = "Pipeline",
					#endif
				});
			}

			ref<Buffer> createVertexBuffer() const {
				return createStaticBuffer({
					.size = sizeof(Vertex) * 3,
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

			ref<Buffer> createIndirectBuffer() const {
				return createStaticBuffer({
					.size = sizeof(DispatchIndirectCommand),
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDIRECT,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Indirect",
					#endif
				});
			}

			ref<Buffer> createStagingBuffer() const {
				return createStaticBuffer({
					.size = vertexBuffer->getSize() + indirectBuffer->getSize(),
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
			}

			ref<Image> createOutputImage() const {
				return createStaticImage({
					.type = ImageType::TEXTURE_2D,
					.format = ImageFormat::R8G8B8A8_UNORM,
					.extent = {64, 64, 1},
					.mipLevelCount = 1,
					.layerCount = 1,
					.sampleCount = 1,
					.usage = ImageUsage::STORAGE | ImageUsage::SAMPLED,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Output",
					#endif
				});
			}

			ref<ImageView> createOutputImageView() const {
				return outputImage->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Output",
					#endif
				});
			}

			ref<Sampler> createSampler() const {
				return Renderer::createSampler({
					.magFilter = Filter::NEAREST,
					.minFilter = Filter::NEAREST,
					.addressModeU = AddressMode::REPEAT,
					.addressModeV = AddressMode::REPEAT,
					.addressModeW = AddressMode::REPEAT,
				});
			}

			void updateDescriptorSets() const {
				const DescriptorImage descriptorImage = {
					.imageView = outputImageView,
					.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.sampler = sampler,
				};

				const WriteDescriptorSet writeDescriptorSet = {
					.descriptorSet = outputDescriptorSet,
					.bindingIndex = 0,
					.descriptorImages = descriptorImage,
				};

				getDevice()->updateDescriptorSets(writeDescriptorSet);
			}

			void computeOutputImage() const {
				const ref<Device> device = getDevice();

				const DescriptorSetLayoutBinding binding = {
					.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::COMPUTE,
				};

				const ref<DescriptorSetLayout> descriptorSetLayout = createDescriptorSetLayout(binding);

				// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = device->allocateDescriptorSets(descriptorSetLayout);

				const ref<DescriptorSet> descriptorSet = descriptorSets[0];

				const DescriptorImage descriptorImage = {
					.imageView = outputImageView,
					.imageLayout = ImageLayout::GENERAL,
					.sampler = nullptr,
				};

				const WriteDescriptorSet writeDescriptorSet = {
					.descriptorSet = descriptorSet,
					.bindingIndex = 0,
					.descriptorImages = descriptorImage,
				};

				device->updateDescriptorSets(writeDescriptorSet);

				const ref<PipelineLayout> pipelineLayout = Renderer::createPipelineLayout({
					.descriptorSetLayouts = {descriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Compute",
					#endif
				});

				const ref<ComputePipeline> computePipeline = createComputePipeline({
					.pipelineLayout = pipelineLayout,
					.computeShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::COMPUTE,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Compute",
							#endif
						},
						"Sample/ComputeShader/ComputeShader.comp"
					),
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Compute",
					#endif
				});

				const ref<Queue> queue = device->getQueue(QueueFamilyCapability::COMPUTE);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Compute",
					#endif
				});

				commandEncoder->begin();

				ImageMemoryBarrier imageMemoryBarrier = {
					.image = outputImage,
					.newLayout = ImageLayout::GENERAL,
					.newAccess = Access::SHADER_WRITE,
				};

				commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
				commandEncoder->bindComputePipeline(computePipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, descriptorSet);
				commandEncoder->dispatchIndirect(indirectBuffer, 0);

				imageMemoryBarrier = {
					.image = outputImage,
					.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

		private:
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<DescriptorSetLayout> outputDescriptorSetLayout;
			ref<DescriptorSet> outputDescriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> pipeline;

			union {
				struct {
					ref<Buffer> vertexBuffer;
					ref<Buffer> indirectBuffer;
					ref<Buffer> stagingBuffer;
				};

				ref<Buffer> staticBuffers[3];
			};

			ref<Image> outputImage;
			ref<ImageView> outputImageView;
			ref<Sampler> sampler;
	};

	DEFINE_SAMPLE_LAYER(DispatchIndirectSample);
}