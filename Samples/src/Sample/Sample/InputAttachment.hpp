#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
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
				createWriteImageRenderPipeline();

				createReadImagePipelineLayout();
				createReadImageRenderPipeline();

				createVertexBuffer();
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				// Write image subpass.
				{
					commandEncoder->bindRenderPipeline(writeImageRenderPipeline);
					commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
					commandEncoder->draw(3, 1, 0, 0);
				}

				commandEncoder->nextSubpass();

				// Read image subpass.
				{
					commandEncoder->bindRenderPipeline(readImageRenderPipeline);
					commandEncoder->bindDescriptorSets(readImagePipelineLayout, {imageDescriptorSet});
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

				image = renderPass->createInputAttachmentImage(attachmentImageDescriptor, 1);
			}

			void recreateImageView() {
				ImageViewDescriptor imageViewDescriptor{};
				imageViewDescriptor.mipLevelOffset = 0;
				imageViewDescriptor.mipLevelCount = image->getMipLevelCount();
				imageViewDescriptor.layerOffset = 0;
				imageViewDescriptor.layerCount = image->getLayerCount();

				imageView = image->createView(imageViewDescriptor);
			}

			void recreateFramebuffers() {
				const ref<Swapchain>& swapchain = getSwapchain();

				for (ref<Framebuffer>& framebuffer : framebuffers) {
					framebuffer.~shared_ptr();
				}

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr, imageView};
				framebufferDescriptor.extent = swapchain->getExtent();
				framebufferDescriptor.layerCount = 1;

				for (uint32 framebufferIndex = 0; framebufferIndex < framebuffers.count(); framebufferIndex += 1) {
					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
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

				writeImagePipelineLayout = createPipelineLayout(writeImagePipelineLayoutDescriptor);
			}

			void createWriteImageRenderPipeline() {
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

				RenderPipelineDescriptor writeImageRenderPipelineDescriptor{};
				writeImageRenderPipelineDescriptor.pipelineLayout = writeImagePipelineLayout;
				writeImageRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
				writeImageRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				writeImageRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				writeImageRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				writeImageRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
				writeImageRenderPipelineDescriptor.cullMode = CullMode::BACK;
				writeImageRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				writeImageRenderPipelineDescriptor.lineWidth = 1.0f;
				writeImageRenderPipelineDescriptor.minSampleShading = 0.0f;
				writeImageRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				writeImageRenderPipelineDescriptor.depthStencilState.depthTest = false;
				writeImageRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				writeImageRenderPipelineDescriptor.renderPass = renderPass;
				writeImageRenderPipelineDescriptor.subpassIndex = WRITE_IMAGE_SUBPASS_INDEX;

				writeImageRenderPipeline = createRenderPipeline(writeImageRenderPipelineDescriptor);
			}

			void createReadImagePipelineLayout() {
				PipelineLayoutDescriptor readImagePipelineLayoutDescriptor{};
				readImagePipelineLayoutDescriptor.descriptorSetLayouts = {imageDescriptorSetLayout};

				readImagePipelineLayout = createPipelineLayout(readImagePipelineLayoutDescriptor);
			}

			void createReadImageRenderPipeline() {
				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				ref<ShaderModule> vertexShaderModule;
				ref<ShaderModule> fragmentShaderModule;

				switch (getGraphicsApi()) {
					case GraphicsAPI::CPU: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
					case GraphicsAPI::DX12: {
						ENGINE_ERROR_UNIMPLEMENTED();

						break;
					}
					case GraphicsAPI::GL: {
						vertexShaderModule = loadShaderModule("Base/ScreenUV.vert", vertexShaderModuleDescriptor);
						fragmentShaderModule = loadShaderModule("Base/TextureRGBA.frag", fragmentShaderModuleDescriptor);

						break;
					}
					case GraphicsAPI::VK: {
						vertexShaderModule = loadShaderModule("Base/Screen.vert", vertexShaderModuleDescriptor);
						fragmentShaderModule = loadShaderModule("Base/ScreenInputAttachment.frag", fragmentShaderModuleDescriptor);

						break;
					}
				}

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				RenderPipelineDescriptor readImageRenderPipelineDescriptor{};
				readImageRenderPipelineDescriptor.pipelineLayout = readImagePipelineLayout;
				readImageRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				readImageRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				readImageRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				readImageRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
				readImageRenderPipelineDescriptor.cullMode = CullMode::BACK;
				readImageRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				readImageRenderPipelineDescriptor.lineWidth = 1.0f;
				readImageRenderPipelineDescriptor.minSampleShading = 0.0f;
				readImageRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				readImageRenderPipelineDescriptor.depthStencilState.depthTest = false;
				readImageRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				readImageRenderPipelineDescriptor.renderPass = renderPass;
				readImageRenderPipelineDescriptor.subpassIndex = READ_IMAGE_SUBPASS_INDEX;

				readImageRenderPipeline = createRenderPipeline(readImageRenderPipelineDescriptor);
			}

			void createVertexBuffer() {
				const ref<Buffer> vertexStagingBuffer = createStagingBuffer(3 * sizeof(Vertex));

				Vertex* vertexStagingBufferMappedRange = vertexStagingBuffer->map<Vertex>(0, vertexStagingBuffer->size);
				vertexStagingBufferMappedRange[0].vertex = {-0.5f, -0.5f};
				vertexStagingBufferMappedRange[0].color = {1.0f, 0.0f, 0.0f};
				vertexStagingBufferMappedRange[1].vertex = {0.0f, 0.5f};
				vertexStagingBufferMappedRange[1].color = {0.0f, 1.0f, 0.0f};
				vertexStagingBufferMappedRange[2].vertex = {0.5f, -0.5f};
				vertexStagingBufferMappedRange[2].color = {0.0f, 0.0f, 1.0f};

				vertexStagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = vertexStagingBuffer->size;
				vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexBufferDescriptor.persistent = false;

				vertexBuffer = createBuffer(vertexBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->size;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, {vertexBufferCopyRegion});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
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

			ref<RenderPipeline> writeImageRenderPipeline;
			ref<RenderPipeline> readImageRenderPipeline;

			ref<Buffer> vertexBuffer;
	};
}