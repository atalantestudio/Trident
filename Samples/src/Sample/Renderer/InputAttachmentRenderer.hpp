#pragma once

#include "ProjectA.hpp"

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class InputAttachmentRenderer : public Renderer {
		public:
			void build() override {
				// 1. Create render pass and subpasses.
				{
					const ref<Surface> surface = getSurface();

					Attachment swapchainAttachment{};
					swapchainAttachment.format = surface->getFormat();
					swapchainAttachment.sampleCount = 1;
					swapchainAttachment.loadOperation = LoadOperation::CLEAR;
					swapchainAttachment.storeOperation = StoreOperation::STORE;
					swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

					Attachment inputAttachment{};
					inputAttachment.format = surface->getFormat();
					inputAttachment.sampleCount = 1;
					inputAttachment.loadOperation = LoadOperation::CLEAR;
					inputAttachment.storeOperation = StoreOperation::UNDEFINED;
					inputAttachment.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

					AttachmentReference writeInputAttachmentReference{};
					writeInputAttachmentReference.index = 1;
					writeInputAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

					AttachmentReference writeSwapchainAttachmentReference{};
					writeSwapchainAttachmentReference.index = 0;
					writeSwapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

					AttachmentReference readInputAttachmentReference{};
					readInputAttachmentReference.index = 1;
					readInputAttachmentReference.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

					SubpassDescriptor writeInputSubpassDescriptor{};
					writeInputSubpassDescriptor.colorAttachmentReferences = {writeInputAttachmentReference};

					SubpassDescriptor readInputSubpassDescriptor{};
					readInputSubpassDescriptor.colorAttachmentReferences = {writeSwapchainAttachmentReference};
					readInputSubpassDescriptor.inputAttachmentReferences = {readInputAttachmentReference};

					const ref<Subpass> writeInputSubpass = createSubpass(writeInputSubpassDescriptor);
					const ref<Subpass> readInputSubpass = createSubpass(readInputSubpassDescriptor);

					RenderPassDescriptor renderPassDescriptor{};
					renderPassDescriptor.attachments = {swapchainAttachment, inputAttachment};
					renderPassDescriptor.subpasses = {writeInputSubpass, readInputSubpass};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 2. Create framebuffers.
				recreateFramebuffers();

				// 3. Create write input render pipeline layout.
				{
					const PipelineLayoutDescriptor writeInputRenderPipelineLayoutDescriptor;

					writeInputRenderPipelineLayout = createPipelineLayout(writeInputRenderPipelineLayoutDescriptor);
				}

				// 4. Create write input pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>();
					vertexInputBinding.registerAttribute<vec3<float32>>();

					ShaderModuleDescriptor vertexShaderModuleDescriptor{};
					vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
					vertexShaderModuleDescriptor.entryPoint = "main";

					ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
					fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
					fragmentShaderModuleDescriptor.entryPoint = "main";

					ColorBlendAttachment colorBlendAttachment{};
					colorBlendAttachment.blend = false;

					RenderPipelineDescriptor writeInputRenderPipelineDescriptor{};
					writeInputRenderPipelineDescriptor.pipelineLayout = writeInputRenderPipelineLayout;
					writeInputRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
					writeInputRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Base/Vertex2DRGB.vert", vertexShaderModuleDescriptor);
					writeInputRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Base/ColorRGB.frag", fragmentShaderModuleDescriptor);
					writeInputRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					writeInputRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					writeInputRenderPipelineDescriptor.cullMode = CullMode::BACK;
					writeInputRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
					writeInputRenderPipelineDescriptor.lineWidth = 1.0f;
					writeInputRenderPipelineDescriptor.minSampleShading = 0.0f;
					writeInputRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
					writeInputRenderPipelineDescriptor.depthStencilState.depthTest = false;
					writeInputRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
					writeInputRenderPipelineDescriptor.renderPass = renderPass;
					writeInputRenderPipelineDescriptor.subpassIndex = 0;

					writeInputRenderPipeline = createRenderPipeline(writeInputRenderPipelineDescriptor);
				}

				// 5. Create read input render pipeline layout.
				createReadInputRenderPipelineLayout();

				// 6. Create read input render pipeline.
				createReadInputRenderPipeline();

				// 7. Create vertex buffer (triangle vertices).
				{
					static constexpr uint64 vertexBufferSize = 3 * sizeof(Vertex);

					const ref<Device> device = getDevice();

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

					vertexMappedRange[0].vertex = {-0.5f, -0.5f};
					vertexMappedRange[0].color = {1.0f, 0.0f, 0.0f};
					vertexMappedRange[1].vertex = {0.0f, 0.5f};
					vertexMappedRange[1].color = {0.0f, 1.0f, 0.0f};
					vertexMappedRange[2].vertex = {0.5f, -0.5f};
					vertexMappedRange[2].color = {0.0f, 0.0f, 1.0f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = vertexBufferSize;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
					vertexBufferDescriptor.persistent = false;

					vertexBuffer = createBuffer(vertexBufferDescriptor);

					sequence<BufferCopyRegion> bufferCopyRegions(1);

					BufferCopyRegion& vertexBufferCopyRegion = bufferCopyRegions[0];
					vertexBufferCopyRegion.sourceOffset = 0;
					vertexBufferCopyRegion.destinationOffset = 0;
					vertexBufferCopyRegion.size = vertexBufferSize;

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
					const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, bufferCopyRegions);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				commandEncoder->bindRenderPipeline(writeInputRenderPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->draw(3, 1, 0, 0);

				commandEncoder->nextSubpass();
				commandEncoder->bindRenderPipeline(readInputRenderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->draw(3, 1, 0, 0);

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
			}

		private:
			void createReadInputRenderPipelineLayout() {
				DescriptorSetLayoutBinding inputAttachmentDescriptorSetLayoutBinding{};
				inputAttachmentDescriptorSetLayoutBinding.descriptorType = DescriptorType::INPUT_ATTACHMENT;
				inputAttachmentDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
				inputAttachmentDescriptorSetLayoutBinding.imageViews = {inputImageView};

				DescriptorSetLayout inputAttachmentDescriptorSetLayout{};
				inputAttachmentDescriptorSetLayout.bindings = {inputAttachmentDescriptorSetLayoutBinding};

				PipelineLayoutDescriptor readInputRenderPipelineLayoutDescriptor{};
				readInputRenderPipelineLayoutDescriptor.layouts = {inputAttachmentDescriptorSetLayout};

				readInputRenderPipelineLayout = createPipelineLayout(readInputRenderPipelineLayoutDescriptor);
			}

			void createReadInputRenderPipeline() {
				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				ref<ShaderModule> vertexShaderModule;
				ref<ShaderModule> fragmentShaderModule;

				switch (getGraphicsApi()) {
					case GraphicsAPI::OPENGL:
						vertexShaderModule = loadShaderModule("Asset/Shader/Base/ScreenUV.vert", vertexShaderModuleDescriptor);
						fragmentShaderModule = loadShaderModule("Asset/Shader/Base/TextureRGBA.frag", fragmentShaderModuleDescriptor);

						break;
					case GraphicsAPI::VULKAN:
						vertexShaderModule = loadShaderModule("Asset/Shader/Base/Screen.vert", vertexShaderModuleDescriptor);
						fragmentShaderModule = loadShaderModule("Asset/Shader/Base/ScreenInputAttachment.frag", fragmentShaderModuleDescriptor);

						break;
					default:
						ENGINE_ERROR_UNSUPPORTED();

						break;
				}

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				RenderPipelineDescriptor readInputRenderPipelineDescriptor{};
				readInputRenderPipelineDescriptor.pipelineLayout = readInputRenderPipelineLayout;
				readInputRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				readInputRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				readInputRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				readInputRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
				readInputRenderPipelineDescriptor.cullMode = CullMode::BACK;
				readInputRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				readInputRenderPipelineDescriptor.lineWidth = 1.0f;
				readInputRenderPipelineDescriptor.minSampleShading = 0.0f;
				readInputRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				readInputRenderPipelineDescriptor.depthStencilState.depthTest = false;
				readInputRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				readInputRenderPipelineDescriptor.renderPass = renderPass;
				readInputRenderPipelineDescriptor.subpassIndex = 1;

				readInputRenderPipeline = createRenderPipeline(readInputRenderPipelineDescriptor);
			}

			void recreateFramebuffers() {
				const ref<Swapchain>& swapchain = getSwapchain();
				const Extent2D swapchainExtent = swapchain->getExtent();

				// 1. Create input image.
				{
					AttachmentImageDescriptor inputAttachmentImageDescriptor{};
					inputAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
					inputAttachmentImageDescriptor.extent.width = swapchainExtent.width;
					inputAttachmentImageDescriptor.extent.height = swapchainExtent.height;
					inputAttachmentImageDescriptor.extent.depth = 1;
					inputAttachmentImageDescriptor.mipLevelCount = 1;
					inputAttachmentImageDescriptor.layerCount = 1;

					inputImage = renderPass->createInputAttachmentImage(inputAttachmentImageDescriptor, 1);
				}

				// 2. Create input image view.
				{
					ImageViewDescriptor inputImageViewDescriptor{};
					inputImageViewDescriptor.mipLevelOffset = 0;
					inputImageViewDescriptor.mipLevelCount = inputImage->getMipLevelCount();
					inputImageViewDescriptor.layerOffset = 0;
					inputImageViewDescriptor.layerCount = inputImage->getLayerCount();

					inputImageView = inputImage->createView(inputImageViewDescriptor);
				}

				// 3. Create framebuffers.
				{
					for (ref<Framebuffer>& framebuffer : framebuffers) {
						framebuffer.~shared_ptr();
					}

					framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

					FramebufferDescriptor framebufferDescriptor{};
					framebufferDescriptor.renderPass = renderPass;
					framebufferDescriptor.imageViews = {nullptr, inputImageView};
					framebufferDescriptor.extent = swapchainExtent;
					framebufferDescriptor.layerCount = 1;

					for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
						framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

						framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
					}
				}

				// 4. Create read input render pipeline layout.
				createReadInputRenderPipelineLayout();

				// 5. Create read input render pipeline.
				createReadInputRenderPipeline();
			}

		private:
			struct Vertex {
				svec2<float32> vertex;
				vec3<float32> color;
			};

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<Image> inputImage;
			ref<ImageView> inputImageView;

			ref<PipelineLayout> writeInputRenderPipelineLayout;
			ref<RenderPipeline> writeInputRenderPipeline;

			ref<PipelineLayout> readInputRenderPipelineLayout;
			ref<RenderPipeline> readInputRenderPipeline;

			ref<Buffer> vertexBuffer;
	};
}