#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextureRenderer : public Renderer {
		public:
			TextureRenderer() {
				TrueTypeFont font(FONT_PATH);

				const ref<Device> device = getDevice();
				const SDFGlyphMap& glyphMap = font.getGlyphMap();

				ref<Buffer> imageStagingBuffer;

				// 1. Create image staging buffer and copy the glyph map data to it.
				{
					imageStagingBuffer = createStagingBuffer(glyphMap.data.count);

					uint8* imageStagingBufferMappedRange = imageStagingBuffer->map<uint8>(0, imageStagingBuffer->size);

					std::copy(glyphMap.data.begin(), glyphMap.data.end(), imageStagingBufferMappedRange);

					imageStagingBuffer->unmap();
				}

				// 1. Create image and copy the image staging buffer to it.
				{
					ImageDescriptor imageDescriptor;
					imageDescriptor.type = ImageType::TEXTURE_2D;
					imageDescriptor.format = ImageFormat::R8_UNORM;
					imageDescriptor.extent.width = glyphMap.extent.width;
					imageDescriptor.extent.height = glyphMap.extent.height;
					imageDescriptor.extent.depth = 1;
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
					bufferImageCopyRegion.imageExtent.width = glyphMap.extent.width;
					bufferImageCopyRegion.imageExtent.height = glyphMap.extent.height;
					bufferImageCopyRegion.imageExtent.depth = 1;
					bufferImageCopyRegion.imageMipLevel = 0;
					bufferImageCopyRegion.imageLayerOffset = 0;
					bufferImageCopyRegion.imageLayerCount = 1;

					CommandBufferDescriptor commandBufferDescriptor;
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->setImageMemoryBarrier(image, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
					commandEncoder->copyBufferToImage(imageStagingBuffer, image, bufferImageCopyRegions);
					commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				// 2. Create image view.
				{
					constexpr ImageViewDescriptor imageViewDescriptor;

					imageView = image->createView(imageViewDescriptor);
				}

				// 3. Create image sampler.
				{
					SamplerDescriptor samplerDescriptor{};
					samplerDescriptor.magFilter = SamplerFilter::NEAREST;
					samplerDescriptor.minFilter = SamplerFilter::NEAREST;

					sampler = createSampler(samplerDescriptor);
				}

				// 4. Create render pass.
				{
					const ref<Surface> surface = getSurface();

					ColorAttachment colorAttachment{};
					colorAttachment.format = surface->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.colorAttachments = {colorAttachment};
					renderPassDescriptor.resolve = false;
					renderPassDescriptor.depthStencil = false;
					renderPassDescriptor.presentAttachmentIndex = 0;

					renderPass = createRenderPass(renderPassDescriptor);

					const ref<Swapchain>& swapchain = getSwapchain();

					swapchain->getImages(renderPass);
				}

				// 5. Create pipeline layout.
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

				// 6. Create render pipeline.
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
					colorBlendAttachment.blend = false;

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

				// 7. Create vertex buffer.
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

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, bufferCopyRegions);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
					device->waitIdle();
				}
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				const ref<Swapchain>& swapchain = getSwapchain();
				const float32 width = static_cast<float32>(swapchain->getExtent().width);
				const float32 height = static_cast<float32>(swapchain->getExtent().height);

				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.renderPass = renderPass;
				beginRenderPassDescriptor.swapchain = swapchain;
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};
				beginRenderPassDescriptor.imageIndex = getImageIndex();

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(svec2<float32>));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtent().width, swapchain->getExtent().height);
				commandEncoder->draw(6, 1, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				const ref<Swapchain>& swapchain = getSwapchain();

				swapchain->getImages(renderPass);
			}

		private:
			static constexpr std::string_view FONT_PATH = "../ProjectA/src/Asset/Font/test.ttf";

		private:
			ref<Image> image;
			ref<ImageView> imageView;

			ref<Sampler> sampler;

			ref<RenderPass> renderPass;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
	};
}