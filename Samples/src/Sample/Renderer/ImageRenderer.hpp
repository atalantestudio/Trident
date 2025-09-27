#pragma once

#include "Engine/Renderer/Renderer.hpp"

// PNG test images:
// - ../ProjectA/src/Asset/Image/icon-black.png		OK
// - ../ProjectA/src/Asset/Image/icon-white.png		OK
// - ../ProjectA/src/Asset/Image/Quiver.png			OK
// - Asset/Image/qoi_test_images/dice.png			OK
// - Asset/Image/qoi_test_images/edgecase.png		OK
// - Asset/Image/qoi_test_images/kodim10.png		OK
// - Asset/Image/qoi_test_images/kodim23.png		OK
// - Asset/Image/qoi_test_images/qoi_logo.png		OK
// - Asset/Image/qoi_test_images/testcard.png		OK
// - Asset/Image/qoi_test_images/testcard_rgba.png	OK
// - Asset/Image/qoi_test_images/wikipedia_008.png	OK
// - Asset/Image/Baboon.png							OK
// - Asset/Image/Cameraman.png						OK
// - Asset/Image/checkerboard_cm0.png				OK
// - Asset/Image/checkerboard_cm9.png				OK
// - Asset/Image/checkerboard1.png					OK
// - Asset/Image/checkerboard2.png					OK
// - Asset/Image/graffiti.png						OK
// - Asset/Image/Horse_rgb.png						OK
// - Asset/Image/Horse_rgba.png						OK
// - Asset/Image/Lenna.png							OK
// - Asset/Image/MetallicGold.png					OK
// - Asset/Image/Pepper.png							OK
// - Asset/Image/RPGpack_sheet_2X.png				OK
// - Asset/Image/test_inflate_0.png					OK
// - Asset/Image/test_inflate_1.png					OK
// - Asset/Image/test_inflate_1b.png				OK
// - Asset/Image/test_inflate_2.png					OK
// - Asset/Image/test_inflate_9.png					OK
// - Asset/Image/test_inflate2_0.png				OK
// - Asset/Image/test_inflate2_9.png				OK
// - Asset/Image/Zelda.png							OK

// QOI test images:
// - Asset/Image/qoi_test_images/dice.qoi			OK
// - Asset/Image/qoi_test_images/edgecase.qoi		OK
// - Asset/Image/qoi_test_images/kodim10.qoi		OK
// - Asset/Image/qoi_test_images/kodim23.qoi		OK
// - Asset/Image/qoi_test_images/qoi_logo.qoi		OK
// - Asset/Image/qoi_test_images/testcard.qoi		OK
// - Asset/Image/qoi_test_images/testcard_rgba.qoi	OK
// - Asset/Image/qoi_test_images/wikipedia_008.qoi	OK
// - Asset/Image/test1.qoi							OK
namespace ProjectA {
	class ImageRenderer : public Renderer {
		public:
			void build() override {
				const sequence<uint8> data = Filesystem::openBuffer("Asset/Image/Zelda.png");
				const File<FileFormat::PNG> file = Filesystem::decode<FileFormat::PNG>(data);
				// const File<FileFormat::QOI> file = Filesystem::decode<FileFormat::QOI>(data);

				const ref<Device> device = getDevice();

				ref<Buffer> imageStagingBuffer;

				// 1. Create image staging buffer and copy the image data to it.
				{
					imageStagingBuffer = createStagingBuffer(file.data.count);

					uint8* imageStagingBufferMappedRange = imageStagingBuffer->map<uint8>(0, imageStagingBuffer->size);

					std::copy(file.data.begin(), file.data.end(), imageStagingBufferMappedRange);

					imageStagingBuffer->unmap();
				}

				// 2. Create image and copy the image staging buffer to it.
				{
					// NOTE: On Vulkan, the data of an RGB image must first be copied to an RGBA image which supports TRANSFER_DST usage.
					ImageDescriptor imageDescriptor;
					imageDescriptor.type = ImageType::TEXTURE_2D;
					imageDescriptor.format = file.format;
					imageDescriptor.extent = file.extent;
					imageDescriptor.mipLevelCount = 1;
					imageDescriptor.layerCount = 1;
					imageDescriptor.sampleCount = 1;
					imageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
					imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

					image = createImage(imageDescriptor);

					sequence<BufferImageCopyRegion> bufferImageCopyRegions(1);

					BufferImageCopyRegion& bufferImageCopyRegion = bufferImageCopyRegions[0];
					bufferImageCopyRegion.bufferOffset = 0;
					bufferImageCopyRegion.imageOffsetX = 0;
					bufferImageCopyRegion.imageOffsetY = 0;
					bufferImageCopyRegion.imageOffsetZ = 0;
					bufferImageCopyRegion.imageExtent = image->getExtent();
					bufferImageCopyRegion.imageMipLevel = 0;
					bufferImageCopyRegion.imageLayerOffset = 0;
					bufferImageCopyRegion.imageLayerCount = 1;

					CommandBufferDescriptor commandBufferDescriptor;
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
					const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
					commandEncoder->copyBufferToImage(imageStagingBuffer, image, bufferImageCopyRegions);
					commandEncoder->setImageMemoryBarrier(image, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					graphicsQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}

				// 3. Create image view.
				{
					ImageViewDescriptor imageViewDescriptor{};
					imageViewDescriptor.mipLevelOffset = 0;
					imageViewDescriptor.mipLevelCount = image->getMipLevelCount();
					imageViewDescriptor.layerOffset = 0;
					imageViewDescriptor.layerCount = image->getLayerCount();

					imageView = image->createView(imageViewDescriptor);
				}

				// 4. Create image sampler.
				{
					SamplerDescriptor samplerDescriptor{};
					samplerDescriptor.magFilter = SamplerFilter::NEAREST;
					samplerDescriptor.minFilter = SamplerFilter::NEAREST;

					sampler = createSampler(samplerDescriptor);
				}

				// 5. Create render pass and subpass.
				{
					const ref<Surface> surface = getSurface();

					ColorAttachment colorAttachment{};
					colorAttachment.format = surface->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;
					colorAttachment.finalLayout = ImageLayout::PRESENT_SRC;

					SubpassDescriptor subpassDescriptor{};
					subpassDescriptor.colorAttachments = {colorAttachment};
					subpassDescriptor.depthStencil = false;

					subpass = createSubpass(subpassDescriptor);

					RenderPassDescriptor renderPassDescriptor{};
					renderPassDescriptor.subpasses = {subpass};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 6. Create framebuffers.
				recreateFramebuffers();

				// 7. Create pipeline layout.
				{
					DescriptorSetLayoutBinding textureDescriptorSetLayoutBinding;
					textureDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
					textureDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
					textureDescriptorSetLayoutBinding.imageViews = {imageView};
					textureDescriptorSetLayoutBinding.samplers = {sampler};

					DescriptorSetLayout descriptorSetLayout;
					descriptorSetLayout.bindings = {textureDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor;
					pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 8. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>();

					ShaderModuleDescriptor vertexShaderModuleDescriptor{};
					vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
					vertexShaderModuleDescriptor.entryPoint = "main";

					ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
					fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
					fragmentShaderModuleDescriptor.entryPoint = "main";

					ColorBlendAttachment colorBlendAttachment{};
					colorBlendAttachment.blend = true;
					colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
					colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

					RenderPipelineDescriptor renderPipelineDescriptor{};
					renderPipelineDescriptor.renderPass = renderPass;
					renderPipelineDescriptor.pipelineLayout = pipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Base/Vertex2DUV.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Base/TextureRGBA.frag", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					renderPipelineDescriptor.cullMode = CullMode::BACK;
					renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
					renderPipelineDescriptor.lineWidth = 1.0f;
					renderPipelineDescriptor.minSampleShading = 0.0f;
					renderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
					renderPipelineDescriptor.depthStencilState.depthTest = false;
					renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}

				// 9. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 3 * sizeof(svec2<float32>);

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					svec2<float32>* vertexMappedRange = vertexStagingBuffer->map<svec2<float32>>(0, vertexBufferSize);

					vertexMappedRange[0] = {-1.0f,  1.0f};
					vertexMappedRange[1] = { 3.0f,  1.0f};
					vertexMappedRange[2] = {-1.0f, -3.0f};

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
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				const ref<Swapchain>& swapchain = getSwapchain();
				const float32 width = static_cast<float32>(swapchain->getExtent().width);
				const float32 height = static_cast<float32>(swapchain->getExtent().height);

				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{1.0f, 1.0f, 1.0f, 1.0f}};

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(svec2<float32>));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtent());
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
			}

		private:
			void recreateFramebuffers() {
				const ref<Swapchain>& swapchain = getSwapchain();

				if (framebuffers.count > 0) {
					for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
						framebuffers[framebufferIndex].~shared_ptr();
					}
				}

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = sequence<ref<ImageView>>(1);
				framebufferDescriptor.extent = swapchain->getExtent();
				framebufferDescriptor.layerCount = 1;

				for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
				}
			}

		private:
			ref<Image> image;
			ref<ImageView> imageView;

			ref<Sampler> sampler;

			ref<Subpass> subpass;
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
	};
}