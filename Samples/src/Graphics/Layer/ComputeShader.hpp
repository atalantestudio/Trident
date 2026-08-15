// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class SampleComputeShaderRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> vertex;
			};

		private:
			static constexpr uint32 VERTEX_COUNT = 3;

		public:
			void build() override {
				setupImage();
				setupImageView();
				writeImage();

				renderPass = createRenderPass();

				setupSampler();

				createImageDescriptorSetLayout();
				allocateImageDescriptorSet();
				updateImageDescriptorSet();

				setupPipelineLayout();
				setupGraphicsPipeline();

				vertexBuffer = createVertexBuffer();

				createBuffers();

				recreateFramebuffers();
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
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, imageDescriptorSet);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
			}

		private:
			void setupImage() {
				ImageDescriptor imageDescriptor{};
				imageDescriptor.type = ImageType::TEXTURE_2D;
				imageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
				imageDescriptor.extent.width = 64;
				imageDescriptor.extent.height = 64;
				imageDescriptor.extent.depth = 1;
				imageDescriptor.mipLevelCount = 1;
				imageDescriptor.layerCount = 1;
				imageDescriptor.sampleCount = 1;
				imageDescriptor.usage = ImageUsage::STORAGE | ImageUsage::SAMPLED;
				imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				#if defined(DEBUG_OBJECT_NAME)
					imageDescriptor.name = "Output";
				#endif

				image = createDynamicImage(imageDescriptor);
			}

			void setupImageView() {
				ImageViewDescriptor imageViewDescriptor{};
				imageViewDescriptor.mipLevelOffset = 0;
				imageViewDescriptor.mipLevelCount = 1;
				imageViewDescriptor.layerOffset = 0;
				imageViewDescriptor.layerCount = 1;

				#if defined(DEBUG_OBJECT_NAME)
					imageViewDescriptor.name = "Output";
				#endif

				imageView = image->createView(imageViewDescriptor);
			}

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

			void createImageDescriptorSetLayout() {
				DescriptorSetLayoutBinding imageBinding{};
				imageBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
				imageBinding.descriptorCount = 1;
				imageBinding.shaderStages = ShaderStage::FRAGMENT;

				imageDescriptorSetLayout = createDescriptorSetLayout({imageBinding});
			}

			void allocateImageDescriptorSet() {
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(imageDescriptorSetLayout);

				imageDescriptorSet = descriptorSets[0];
			}

			void updateImageDescriptorSet() {
				DescriptorImage descriptorImage{};
				descriptorImage.imageView = imageView;
				descriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				descriptorImage.sampler = sampler;

				WriteDescriptorSet imageWriteDescriptorSet{};
				imageWriteDescriptorSet.descriptorSet = imageDescriptorSet;
				imageWriteDescriptorSet.bindingIndex = 0;
				imageWriteDescriptorSet.descriptorImages = {descriptorImage};

				getDevice()->updateDescriptorSets({imageWriteDescriptorSet});
			}

			void setupPipelineLayout() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};
				pipelineLayoutDescriptor.descriptorSetLayouts = {imageDescriptorSetLayout};

				#if defined(DEBUG_OBJECT_NAME)
					pipelineLayoutDescriptor.name = "Render pipeline layout";
				#endif

				pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
			}

			void setupGraphicsPipeline() {
				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<svec2<float32>>();

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

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/ComputeShader/ComputeShader.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/ComputeShader/ComputeShader.frag");

				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = false,
					},
				};

				GraphicsPipelineDescriptor graphicsPipelineDescriptor{};
				graphicsPipelineDescriptor.pipelineLayout = pipelineLayout;
				graphicsPipelineDescriptor.vertexInputBindings = vertexInputBinding;
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

			ref<Buffer> createVertexBuffer() const {
				return createDynamicBuffer({
					.size = VERTEX_COUNT * sizeof(Vertex),
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

			void createBuffers() {
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
				vertexMappedRange[0].vertex = {-1.0f,  1.0f};
				vertexMappedRange[1].vertex = { 3.0f,  1.0f};
				vertexMappedRange[2].vertex = {-1.0f, -3.0f};

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
					.sourceOffset = vertexView.getOffset(),
					.destinationOffset = 0,
					.size = vertexView.getSize(),
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegion);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

			void setupSampler() {
				sampler = createSampler({
					.magFilter = Filter::NEAREST,
					.minFilter = Filter::NEAREST,
					.addressModeU = AddressMode::REPEAT,
					.addressModeV = AddressMode::REPEAT,
					.addressModeW = AddressMode::REPEAT,
				});
			}

			void writeImage() const {
				DescriptorSetLayoutBinding imageBinding{};
				imageBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE;
				imageBinding.descriptorCount = 1;
				imageBinding.shaderStages = ShaderStage::COMPUTE;

				const ref<DescriptorSetLayout> imageDescriptorSetLayout = createDescriptorSetLayout({imageBinding});
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(imageDescriptorSetLayout);
				const ref<DescriptorSet> imageDescriptorSet = descriptorSets[0];

				DescriptorImage descriptorImage{};
				descriptorImage.imageView = imageView;
				descriptorImage.imageLayout = ImageLayout::GENERAL;
				descriptorImage.sampler = nullptr;

				WriteDescriptorSet imageWriteDescriptorSet{};
				imageWriteDescriptorSet.descriptorSet = imageDescriptorSet;
				imageWriteDescriptorSet.bindingIndex = 0;
				imageWriteDescriptorSet.descriptorImages = {descriptorImage};

				getDevice()->updateDescriptorSets({imageWriteDescriptorSet});

				PipelineLayoutDescriptor pipelineLayoutDescriptor{};
				pipelineLayoutDescriptor.descriptorSetLayouts = {imageDescriptorSetLayout};

				#if defined(DEBUG_OBJECT_NAME)
					pipelineLayoutDescriptor.name = "Compute pipeline layout";
				#endif

				const ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				ShaderModuleDescriptor computeShaderModuleDescriptor{};
				computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
				computeShaderModuleDescriptor.entryPoint = "main";

				#if defined(DEBUG_OBJECT_NAME)
					computeShaderModuleDescriptor.name = "Compute shader module";
				#endif

				const ref<ShaderModule> computeShaderModule = loadShaderModule(computeShaderModuleDescriptor, "Sample/ComputeShader/ComputeShader.comp");

				ComputePipelineDescriptor computePipelineDescriptor{};
				computePipelineDescriptor.pipelineLayout = pipelineLayout;
				computePipelineDescriptor.computeShaderModule = computeShaderModule;

				#if defined(DEBUG_OBJECT_NAME)
					computePipelineDescriptor.name = "Compute pipeline";
				#endif

				const ref<ComputePipeline> computePipeline = createComputePipeline(computePipelineDescriptor);

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				#if defined(DEBUG_OBJECT_NAME)
					commandBufferDescriptor.name = "Compute command buffer";
				#endif

				const ref<Queue> computeQueue = getDevice()->getQueue(QueueFamilyCapability::COMPUTE);
				const ref<CommandEncoder> commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();

				ImageMemoryBarrier imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::GENERAL,
					.newAccess = Access::SHADER_WRITE,
				};

				commandEncoder->insertMemoryBarriers({}, {imageMemoryBarrier});
				commandEncoder->bindComputePipeline(computePipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, imageDescriptorSet);
				commandEncoder->dispatch(2, 2, 1);

				imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({}, {imageMemoryBarrier});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				computeQueue->submit(commandBuffer);
				computeQueue->waitIdle();
			}

			void recreateFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr};
				framebufferDescriptor.extent = swapchain->getExtent();

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

		private:
			ref<Image> image;
			ref<ImageView> imageView;
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<Sampler> sampler;
			ref<DescriptorSetLayout> imageDescriptorSetLayout;
			ref<DescriptorSet> imageDescriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> graphicsPipeline;
			ref<Buffer> vertexBuffer;
	};

	DEFINE_SAMPLE_LAYER(SampleComputeShader);
}