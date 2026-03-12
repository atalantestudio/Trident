// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	class SampleRenderer : public Renderer {
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
				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getSwapchainImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
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
					commandEncoder->bindDescriptorSets(readImagePipelineLayout, 0, {imageDescriptorSet});
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
				Attachment swapchainAttachment{};
				swapchainAttachment.format = getSurface()->getFormat();
				swapchainAttachment.sampleCount = 1;
				swapchainAttachment.loadOperation = LoadOperation::CLEAR;
				swapchainAttachment.storeOperation = StoreOperation::STORE;
				swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

				Attachment imageAttachment{};
				imageAttachment.format = getSurface()->getFormat();
				imageAttachment.sampleCount = 1;
				imageAttachment.loadOperation = LoadOperation::CLEAR;
				imageAttachment.storeOperation = StoreOperation::UNDEFINED;
				imageAttachment.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

				AttachmentReference writeImageAttachmentReference{};
				writeImageAttachmentReference.index = 1;
				writeImageAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

				AttachmentReference writeSwapchainAttachmentReference{};
				writeSwapchainAttachmentReference.index = 0;
				writeSwapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

				AttachmentReference readImageAttachmentReference{};
				readImageAttachmentReference.index = 1;
				readImageAttachmentReference.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

				SubpassDescriptor writeImageSubpassDescriptor{};
				writeImageSubpassDescriptor.colorAttachmentReferences = {writeImageAttachmentReference};

				SubpassDescriptor readImageSubpassDescriptor{};
				readImageSubpassDescriptor.colorAttachmentReferences = {writeSwapchainAttachmentReference};
				readImageSubpassDescriptor.inputAttachmentReferences = {readImageAttachmentReference};

				const ref<Subpass> writeImageSubpass = createSubpass(writeImageSubpassDescriptor);
				const ref<Subpass> readImageSubpass = createSubpass(readImageSubpassDescriptor);

				RenderPassDescriptor renderPassDescriptor{};
				renderPassDescriptor.attachments = {swapchainAttachment, imageAttachment};
				renderPassDescriptor.subpasses = {writeImageSubpass, readImageSubpass};

				renderPass = createRenderPass(renderPassDescriptor);
			}

			void recreateImage() {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();

				AttachmentImageDescriptor attachmentImageDescriptor{};
				attachmentImageDescriptor.type = ImageType::TEXTURE_2D;
				attachmentImageDescriptor.extent.width = swapchainExtent.width;
				attachmentImageDescriptor.extent.height = swapchainExtent.height;
				attachmentImageDescriptor.extent.depth = 1;
				attachmentImageDescriptor.mipLevelCount = 1;
				attachmentImageDescriptor.layerCount = 1;

				SET_DEBUG_NAME(attachmentImageDescriptor, "Image");

				image = renderPass->createInputAttachmentImage(attachmentImageDescriptor, 1);
			}

			void recreateImageView() {
				ImageViewDescriptor imageViewDescriptor{};
				imageViewDescriptor.mipLevelOffset = 0;
				imageViewDescriptor.mipLevelCount = 1;
				imageViewDescriptor.layerOffset = 0;
				imageViewDescriptor.layerCount = 1;

				SET_DEBUG_NAME(imageViewDescriptor, "Image view");

				imageView = image->createView(imageViewDescriptor);
			}

			void recreateFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr, imageView};
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

			void createImageDescriptorSetLayout() {
				DescriptorSetLayoutBinding imageBinding{};
				imageBinding.descriptorType = DescriptorType::INPUT_ATTACHMENT;
				imageBinding.descriptorCount = 1;
				imageBinding.shaderStages = ShaderStage::FRAGMENT;

				imageDescriptorSetLayout = createDescriptorSetLayout({imageBinding});
			}

			void allocateImageDescriptorSet() {
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({imageDescriptorSetLayout});

				imageDescriptorSet = descriptorSets[0];
			}

			void updateImageDescriptorSet() {
				DescriptorImage descriptorImage{};
				descriptorImage.imageView = imageView;
				descriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				descriptorImage.sampler = nullptr;

				WriteDescriptorSet imageWriteDescriptorSet{};
				imageWriteDescriptorSet.descriptorSet = imageDescriptorSet;
				imageWriteDescriptorSet.bindingIndex = 0;
				imageWriteDescriptorSet.descriptorImages = {descriptorImage};

				getDevice()->updateDescriptorSets({imageWriteDescriptorSet});
			}

			void createWriteImagePipelineLayout() {
				PipelineLayoutDescriptor writeImagePipelineLayoutDescriptor{};

				SET_DEBUG_NAME(writeImagePipelineLayoutDescriptor, "Image write");

				writeImagePipelineLayout = createPipelineLayout(writeImagePipelineLayoutDescriptor);
			}

			void createWriteImageGraphicsPipeline() {
				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<svec2<float32>>();
				vertexInputBinding.registerAttribute<vec3<float32>>();

				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Image write vertex");

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Image write fragment");

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/Triangle/Triangle.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/Triangle/Triangle.frag");

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				GraphicsPipelineDescriptor writeImageGraphicsPipelineDescriptor{};
				writeImageGraphicsPipelineDescriptor.pipelineLayout = writeImagePipelineLayout;
				writeImageGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
				writeImageGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				writeImageGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				writeImageGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				writeImageGraphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
				writeImageGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
				writeImageGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				writeImageGraphicsPipelineDescriptor.lineWidth = 1.0f;
				writeImageGraphicsPipelineDescriptor.minSampleShading = 0.0f;
				writeImageGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				writeImageGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
				writeImageGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				writeImageGraphicsPipelineDescriptor.renderPass = renderPass;
				writeImageGraphicsPipelineDescriptor.subpassIndex = WRITE_IMAGE_SUBPASS_INDEX;

				SET_DEBUG_NAME(writeImageGraphicsPipelineDescriptor, "Image write");

				writeImageGraphicsPipeline = createGraphicsPipeline(writeImageGraphicsPipelineDescriptor);
			}

			void createReadImagePipelineLayout() {
				PipelineLayoutDescriptor readImagePipelineLayoutDescriptor{};
				readImagePipelineLayoutDescriptor.descriptorSetLayouts = {imageDescriptorSetLayout};

				SET_DEBUG_NAME(readImagePipelineLayoutDescriptor, "Image read");

				readImagePipelineLayout = createPipelineLayout(readImagePipelineLayoutDescriptor);
			}

			void createReadImageGraphicsPipeline() {
				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Image read vertex");

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Image read fragment");

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/InputAttachment/InputAttachment.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/InputAttachment/InputAttachment.frag");

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				GraphicsPipelineDescriptor readImageGraphicsPipelineDescriptor{};
				readImageGraphicsPipelineDescriptor.pipelineLayout = readImagePipelineLayout;
				readImageGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				readImageGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				readImageGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				readImageGraphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
				readImageGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
				readImageGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				readImageGraphicsPipelineDescriptor.lineWidth = 1.0f;
				readImageGraphicsPipelineDescriptor.minSampleShading = 0.0f;
				readImageGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				readImageGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
				readImageGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				readImageGraphicsPipelineDescriptor.renderPass = renderPass;
				readImageGraphicsPipelineDescriptor.subpassIndex = READ_IMAGE_SUBPASS_INDEX;

				SET_DEBUG_NAME(readImageGraphicsPipelineDescriptor, "Image read");

				readImageGraphicsPipeline = createGraphicsPipeline(readImageGraphicsPipelineDescriptor);
			}

			void createVertexBuffer() {
				const ref<Buffer<Vertex>> vertexStagingBuffer = createStagingBuffer<Vertex>(3 * sizeof(Vertex));

				Vertex* vertexStagingBufferMappedRange = vertexStagingBuffer->map(0, vertexStagingBuffer->getSize());
				vertexStagingBufferMappedRange[0].vertex = {-0.5f, -0.5f};
				vertexStagingBufferMappedRange[0].color = {1.0f, 0.0f, 0.0f};
				vertexStagingBufferMappedRange[1].vertex = {0.0f, 0.5f};
				vertexStagingBufferMappedRange[1].color = {0.0f, 1.0f, 0.0f};
				vertexStagingBufferMappedRange[2].vertex = {0.5f, -0.5f};
				vertexStagingBufferMappedRange[2].color = {0.0f, 0.0f, 1.0f};

				vertexStagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = vertexStagingBuffer->getSize();
				vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexBufferDescriptor, "Vertex buffer");

				vertexBuffer = createBuffer<Vertex>(vertexBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->getSize();

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Buffer copy command buffer");

				const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, {vertexBufferCopyRegion});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit({commandBuffer});
				transferQueue->waitIdle();
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
}