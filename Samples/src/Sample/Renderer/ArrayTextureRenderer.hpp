#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class ArrayTextureRenderer : public Renderer {
		public:
			void build() override {
				const ref<Device> device = getDevice();

				// 1. Load assets as images.
				{
					baboonImage = loadImage(BABOON_IMAGE_PATH);
					cameramanImage = loadImage(CAMERAMAN_IMAGE_PATH);
					lennaImage = loadImage(LENNA_IMAGE_PATH);
					pepperImage = loadImage(PEPPER_IMAGE_PATH);
				}

				// 2. Create image views.
				{
					ImageViewDescriptor imageViewDescriptor{};
					imageViewDescriptor.mipLevelOffset = 0;
					imageViewDescriptor.mipLevelCount = 1;
					imageViewDescriptor.layerOffset = 0;
					imageViewDescriptor.layerCount = 1;

					baboonImageView = baboonImage->createView(imageViewDescriptor);
					cameramanImageView = cameramanImage->createView(imageViewDescriptor);
					lennaImageView = lennaImage->createView(imageViewDescriptor);
					pepperImageView = pepperImage->createView(imageViewDescriptor);
				}

				// 3. Create sampler.
				{
					SamplerDescriptor samplerDescriptor{};
					samplerDescriptor.magFilter = SamplerFilter::LINEAR;
					samplerDescriptor.minFilter = SamplerFilter::NEAREST;

					sampler = createSampler(samplerDescriptor);
				}

				// 4. Create render pass and subpass.
				{
					const ref<Surface> surface = getSurface();

					Attachment swapchainAttachment{};
					swapchainAttachment.format = surface->getFormat();
					swapchainAttachment.sampleCount = 1;
					swapchainAttachment.loadOperation = LoadOperation::CLEAR;
					swapchainAttachment.storeOperation = StoreOperation::STORE;
					swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

					AttachmentReference swapchainAttachmentReference{};
					swapchainAttachmentReference.index = 0;
					swapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

					SubpassDescriptor subpassDescriptor{};
					subpassDescriptor.colorAttachmentReferences = {swapchainAttachmentReference};

					const ref<Subpass> subpass = createSubpass(subpassDescriptor);

					RenderPassDescriptor renderPassDescriptor{};
					renderPassDescriptor.attachments = {swapchainAttachment};
					renderPassDescriptor.subpasses = {subpass};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 5. Create framebuffers.
				recreateFramebuffers();

				// 6. Create pipeline layout.
				{
					DescriptorSetLayoutBinding textureArrayDescriptorSetLayoutBinding;
					textureArrayDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
					textureArrayDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
					textureArrayDescriptorSetLayoutBinding.imageViews = {baboonImageView, cameramanImageView, lennaImageView, pepperImageView};

					// TODO: Don't provide a copy of the sampler for each image view.
					textureArrayDescriptorSetLayoutBinding.samplers = {sampler, sampler, sampler, sampler};

					DescriptorSetLayout descriptorSetLayout;
					descriptorSetLayout.bindings = {textureArrayDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor;
					pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 7. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>();

					VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
					instanceInputBinding.registerAttribute<svec2<float32>>();
					instanceInputBinding.registerAttribute<uint32>();

					ShaderModuleDescriptor vertexShaderModuleDescriptor{};
					vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
					vertexShaderModuleDescriptor.entryPoint = "main";

					ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
					fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
					fragmentShaderModuleDescriptor.entryPoint = "main";

					ColorBlendAttachment colorBlendAttachment{};
					colorBlendAttachment.blend = false;

					RenderPipelineDescriptor renderPipelineDescriptor{};
					renderPipelineDescriptor.pipelineLayout = pipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/ArrayTextureOrTextureArray.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/ArrayTexture.frag", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					renderPipelineDescriptor.cullMode = CullMode::BACK;
					renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
					renderPipelineDescriptor.lineWidth = 1.0f;
					renderPipelineDescriptor.minSampleShading = 0.0f;
					renderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
					renderPipelineDescriptor.depthStencilState.depthTest = false;
					renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
					renderPipelineDescriptor.renderPass = renderPass;
					renderPipelineDescriptor.subpassIndex = 0;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}

				// 8. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 6 * sizeof(Vertex);

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

					vertexMappedRange[0].vertex = {-0.5f,  0.5f};
					vertexMappedRange[1].vertex = { 0.5f,  0.5f};
					vertexMappedRange[2].vertex = { 0.5f, -0.5f};
					vertexMappedRange[3].vertex = {-0.5f,  0.5f};
					vertexMappedRange[4].vertex = { 0.5f, -0.5f};
					vertexMappedRange[5].vertex = {-0.5f, -0.5f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = vertexBufferSize;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

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

				// 9. Create instance buffer.
				{
					constexpr uint64 instanceVertexBufferSize = 4 * sizeof(Instance);

					const ref<Buffer> instanceVertexStagingBuffer = createStagingBuffer(instanceVertexBufferSize);

					Instance* instanceMappedRange = instanceVertexStagingBuffer->map<Instance>(0, instanceVertexBufferSize);

					instanceMappedRange[0].vertexOffset = {-0.5f, 0.5f};
					instanceMappedRange[0].textureIndex = 0;
					instanceMappedRange[1].vertexOffset = {0.5f, 0.5f};
					instanceMappedRange[1].textureIndex = 1;
					instanceMappedRange[2].vertexOffset = {-0.5f, -0.5f};
					instanceMappedRange[2].textureIndex = 2;
					instanceMappedRange[3].vertexOffset = {0.5f, -0.5f};
					instanceMappedRange[3].textureIndex = 3;

					instanceVertexStagingBuffer->unmap();

					BufferDescriptor instanceVertexBufferDescriptor;
					instanceVertexBufferDescriptor.size = instanceVertexBufferSize;
					instanceVertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					instanceVertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					instanceVertexBuffer = createBuffer(instanceVertexBufferDescriptor);

					sequence<BufferCopyRegion> bufferCopyRegions(1);

					BufferCopyRegion& instanceBufferCopyRegion = bufferCopyRegions[0];
					instanceBufferCopyRegion.sourceOffset = 0;
					instanceBufferCopyRegion.destinationOffset = 0;
					instanceBufferCopyRegion.size = instanceVertexBufferSize;

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
					const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(instanceVertexStagingBuffer, instanceVertexBuffer, bufferCopyRegions);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->bindVertexBuffer(1, instanceVertexBuffer, 0, sizeof(Instance));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->draw(24, 4, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
			}

		private:
			ref<Image> loadImage(std::string_view path) {
				const ref<Device> device = getDevice();

				const sequence<uint8> data = Filesystem::openBuffer(path);
				const File<FileFormat::PNG> file = Filesystem::decode<FileFormat::PNG>(data);
				const uint64 imageBufferSize = file.data.count * sizeof(uint8);

				const ref<Buffer> imageStagingBuffer = createStagingBuffer(imageBufferSize);

				uint8* imageData = imageStagingBuffer->map<uint8>(0, imageBufferSize);

				std::copy(file.data.begin(), file.data.end(), imageData);

				imageStagingBuffer->unmap();

				ImageDescriptor imageDescriptor{};
				imageDescriptor.type = ImageType::TEXTURE_2D;
				imageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
				imageDescriptor.extent = file.extent;
				imageDescriptor.mipLevelCount = 1;
				imageDescriptor.layerCount = 1;
				imageDescriptor.sampleCount = 1;
				imageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
				imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

				const ref<Image> image = createImage(imageDescriptor);

				sequence<BufferImageCopyRegion> copyRegions(1);

				BufferImageCopyRegion& bufferImageCopyRegion = copyRegions[0];
				bufferImageCopyRegion.bufferOffset = 0;
				bufferImageCopyRegion.imageOffsetX = 0;
				bufferImageCopyRegion.imageOffsetY = 0;
				bufferImageCopyRegion.imageOffsetZ = 0;
				bufferImageCopyRegion.imageExtent = file.extent;
				bufferImageCopyRegion.imageMipLevel = 0;
				bufferImageCopyRegion.imageLayerOffset = 0;
				bufferImageCopyRegion.imageLayerCount = 1;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
				commandEncoder->copyBufferToImage(imageStagingBuffer, image, copyRegions);
				commandEncoder->setImageMemoryBarrier(image, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				graphicsQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

				device->waitIdle();

				return image;
			}

			void recreateFramebuffers() {
				const ref<Swapchain>& swapchain = getSwapchain();

				for (ref<Framebuffer>& framebuffer : framebuffers) {
					framebuffer.~shared_ptr();
				}

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr};
				framebufferDescriptor.extent = swapchain->getExtent();
				framebufferDescriptor.layerCount = 1;

				for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
				}
			}

		private:
			struct Vertex {
				svec2<float32> vertex;
			};

			struct Instance {
				svec2<float32> vertexOffset;
				uint32 textureIndex;
			};

		private:
			static constexpr std::string_view BABOON_IMAGE_PATH = "Asset/Image/Baboon.png";
			static constexpr std::string_view CAMERAMAN_IMAGE_PATH = "Asset/Image/Cameraman.png";
			static constexpr std::string_view LENNA_IMAGE_PATH = "Asset/Image/Lenna.png";
			static constexpr std::string_view PEPPER_IMAGE_PATH = "Asset/Image/Pepper.png";

		private:
			ref<Image> baboonImage;
			ref<ImageView> baboonImageView;

			ref<Image> cameramanImage;
			ref<ImageView> cameramanImageView;

			ref<Image> lennaImage;
			ref<ImageView> lennaImageView;

			ref<Image> pepperImage;
			ref<ImageView> pepperImageView;

			ref<Sampler> sampler;

			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;

			ref<Buffer> instanceVertexBuffer;
	};
}