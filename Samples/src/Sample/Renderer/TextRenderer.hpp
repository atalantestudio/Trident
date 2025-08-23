#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextRenderer : public Renderer {
		public:
			void build() override {
				const ref<FontAsset>& fontAsset = Application::getAssetLibrary().findFontAsset("test-regular");
				const ref<ImageAsset>& glyphMapImageAsset = fontAsset->getGlyphMapImage();
				const ref<Device> device = getDevice();

				ref<Buffer> glyphMapImageStagingBuffer;

				// 1. Create image staging buffer and copy the glyph map data to it.
				{
					glyphMapImageStagingBuffer = createStagingBuffer(glyphMapImageAsset->getData().count);

					uint8* glyphMapImageStagingBufferMappedRange = glyphMapImageStagingBuffer->map<uint8>(0, glyphMapImageStagingBuffer->size);

					std::copy(glyphMapImageAsset->getData().begin(), glyphMapImageAsset->getData().end(), glyphMapImageStagingBufferMappedRange);

					glyphMapImageStagingBuffer->unmap();
				}

				// 2. Create image and copy the image staging buffer to it.
				{
					ImageDescriptor glyphMapImageDescriptor;
					glyphMapImageDescriptor.type = ImageType::TEXTURE_2D;
					glyphMapImageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
					glyphMapImageDescriptor.extent.width = glyphMapImageAsset->getExtent().width;
					glyphMapImageDescriptor.extent.height = glyphMapImageAsset->getExtent().height;
					glyphMapImageDescriptor.extent.depth = 1;
					glyphMapImageDescriptor.mipLevelCount = 1;
					glyphMapImageDescriptor.layerCount = 1;
					glyphMapImageDescriptor.sampleCount = 1;
					glyphMapImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
					glyphMapImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

					glyphMapImage = createImage(glyphMapImageDescriptor);

					sequence<BufferImageCopyRegion> bufferImageCopyRegions(1);

					BufferImageCopyRegion& bufferImageCopyRegion = bufferImageCopyRegions[0];
					bufferImageCopyRegion.bufferOffset = 0;
					bufferImageCopyRegion.imageOffsetX = 0;
					bufferImageCopyRegion.imageOffsetY = 0;
					bufferImageCopyRegion.imageOffsetZ = 0;
					bufferImageCopyRegion.imageExtent.width = glyphMapImageAsset->getExtent().width;
					bufferImageCopyRegion.imageExtent.height = glyphMapImageAsset->getExtent().height;
					bufferImageCopyRegion.imageExtent.depth = 1;
					bufferImageCopyRegion.imageMipLevel = 0;
					bufferImageCopyRegion.imageLayerOffset = 0;
					bufferImageCopyRegion.imageLayerCount = 1;

					CommandBufferDescriptor commandBufferDescriptor;
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
					const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->setImageMemoryBarrier(glyphMapImage, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
					commandEncoder->copyBufferToImage(glyphMapImageStagingBuffer, glyphMapImage, bufferImageCopyRegions);
					commandEncoder->setImageMemoryBarrier(glyphMapImage, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					graphicsQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}

				// 3. Create glyph map image view.
				{
					ImageViewDescriptor imageViewDescriptor{};
					imageViewDescriptor.mipLevelOffset = 0;
					imageViewDescriptor.mipLevelCount = glyphMapImage->getMipLevelCount();
					imageViewDescriptor.layerOffset = 0;
					imageViewDescriptor.layerCount = glyphMapImage->getLayerCount();

					glyphMapImageView = glyphMapImage->createView(imageViewDescriptor);
				}

				// 4. Create glyph map image sampler.
				{
					SamplerDescriptor samplerDescriptor{};
					samplerDescriptor.magFilter = SamplerFilter::NEAREST;
					samplerDescriptor.minFilter = SamplerFilter::NEAREST;

					glyphMapImageSampler = createSampler(samplerDescriptor);
				}

				// 5. Create camera buffer.
				{
					constexpr uint64 cameraBufferSize = sizeof(Camera);

					BufferDescriptor cameraUniformBufferDescriptor;
					cameraUniformBufferDescriptor.size = cameraBufferSize;
					cameraUniformBufferDescriptor.usage = BufferUsage::UNIFORM;
					cameraUniformBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
					cameraUniformBufferDescriptor.persistent = true;

					cameraBuffer = createBuffer(cameraUniformBufferDescriptor);
					cameraMappedRange = cameraBuffer->map<Camera>(0, cameraBufferSize);
				}

				// 6. Create render pass and subpass.
				{
					const ref<Surface> surface = getSurface();

					ColorAttachment colorAttachment{};
					colorAttachment.format = surface->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;
					colorAttachment.finalImageLayout = ImageLayout::PRESENT_SRC;

					SubpassDescriptor subpassDescriptor{};
					subpassDescriptor.colorAttachments = {colorAttachment};
					subpassDescriptor.depthStencil = false;

					subpass = createSubpass(subpassDescriptor);

					RenderPassDescriptor renderPassDescriptor{};
					renderPassDescriptor.subpasses = {subpass};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 7. Create framebuffers.
				recreateFramebuffers();

				// 8. Create pipeline layout.
				{
					DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
					cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
					cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
					cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

					DescriptorSetLayoutBinding glyphMapImageDescriptorSetLayoutBinding;
					glyphMapImageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
					glyphMapImageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
					glyphMapImageDescriptorSetLayoutBinding.imageViews = {glyphMapImageView};
					glyphMapImageDescriptorSetLayoutBinding.samplers = {glyphMapImageSampler};

					DescriptorSetLayout descriptorSetLayout;
					descriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, glyphMapImageDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor;
					pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 9. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>();
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
					renderPipelineDescriptor.pipelineLayout = pipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Glyph.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Glyph.frag", fragmentShaderModuleDescriptor);
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

				// 10. Create index buffer.
				{
					constexpr uint64 indexBufferSize = 6 * MAX_CHARACTER_COUNT * sizeof(uint16);

					const ref<Buffer> indexStagingBuffer = createStagingBuffer(indexBufferSize);

					uint16* indexMappedRange = indexStagingBuffer->map<uint16>(0, indexBufferSize);

					for (uint32 characterIndex = 0; characterIndex < MAX_CHARACTER_COUNT; characterIndex += 1) {
						indexMappedRange[0] = 4 * characterIndex + 0;
						indexMappedRange[1] = 4 * characterIndex + 1;
						indexMappedRange[2] = 4 * characterIndex + 2;
						indexMappedRange[3] = 4 * characterIndex + 0;
						indexMappedRange[4] = 4 * characterIndex + 2;
						indexMappedRange[5] = 4 * characterIndex + 3;

						indexMappedRange += 6;
					}

					indexStagingBuffer->unmap();

					BufferDescriptor indexBufferDescriptor;
					indexBufferDescriptor.size = indexBufferSize;
					indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
					indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					indexBuffer = createBuffer(indexBufferDescriptor);

					sequence<BufferCopyRegion> bufferCopyRegions(1);

					BufferCopyRegion& indexBufferCopyRegion = bufferCopyRegions[0];
					indexBufferCopyRegion.sourceOffset = 0;
					indexBufferCopyRegion.destinationOffset = 0;
					indexBufferCopyRegion.size = indexBufferSize;

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
					const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(indexStagingBuffer, indexBuffer, bufferCopyRegions);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}

				// 11. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 4 * MAX_CHARACTER_COUNT * sizeof(Vertex);

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = vertexBufferSize;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX;
					vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
					vertexBufferDescriptor.persistent = true;

					vertexBuffer = createBuffer(vertexBufferDescriptor);
					vertexMappedRange = vertexBuffer->map<Vertex>(0, vertexBufferSize);
				}

				updateCamera();

				setText("PP Radio Grotesk", fontAsset, 64.0f);
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				const ref<Swapchain>& swapchain = getSwapchain();
				const float32 width = static_cast<float32>(swapchain->getExtent().width);
				const float32 height = static_cast<float32>(swapchain->getExtent().height);

				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtent());
				commandEncoder->drawIndexed(6 * characterCount, 1, 0, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
				updateCamera();
			}

			void setText(std::string_view text, const ref<FontAsset>& fontAsset, float32 fontSize) {
				ENGINE_ASSERT(text.size() <= MAX_CHARACTER_COUNT, "The text must not exceed [] characters.", MAX_CHARACTER_COUNT);

				characterCount = static_cast<uint32>(text.size());

				const float32 lineHeight = static_cast<float32>(fontAsset->getLineHeight());

				uint32 vertexIndex = 0;
				float32 positionX = 0;
				float32 positionY = 0;

				for (const char8 character : text) {
					const uint16 codePoint = static_cast<uint16>(character);
					const Glyph& glyph = (*fontAsset)[codePoint];

					const float32 advanceWidth = std::ceil(glyph.advanceWidth / 1210.0f * fontSize);

					if (!glyph.whitespace) {
						const float32 width = glyph.width / lineHeight * fontSize;
						const float32 leftSideBearing = glyph.leftSideBearing / lineHeight * fontSize;

						const float32 x = positionX + leftSideBearing;
						const float32 y = positionY;

						// Top-left
						vertexMappedRange[vertexIndex + 0].vertex = {x, y};
						vertexMappedRange[vertexIndex + 0].imageCoordinate = {glyph.minCoordinate.x, glyph.minCoordinate.y};

						// Top-right
						vertexMappedRange[vertexIndex + 1].vertex = {x + width, y};
						vertexMappedRange[vertexIndex + 1].imageCoordinate = {glyph.maxCoordinate.x, glyph.minCoordinate.y};

						// Bottom-right
						vertexMappedRange[vertexIndex + 2].vertex = {x + width, y + fontSize};
						vertexMappedRange[vertexIndex + 2].imageCoordinate = {glyph.maxCoordinate.x, glyph.maxCoordinate.y};

						// Bottom-left
						vertexMappedRange[vertexIndex + 3].vertex = {x, y + fontSize};
						vertexMappedRange[vertexIndex + 3].imageCoordinate = {glyph.minCoordinate.x, glyph.maxCoordinate.y};

						vertexIndex += 4;
					}

					positionX += advanceWidth;
				}
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

			void updateCamera() const {
				const ref<Swapchain>& swapchain = getSwapchain();
				const float32 width = static_cast<float32>(swapchain->getExtent().width);
				const float32 height = static_cast<float32>(swapchain->getExtent().height);

				cameraMappedRange->projection = {
					 2.0f / width,  0.0f,           0.0f,
					 0.0f,         -2.0f / height,  0.0f,
					-1.0f,          1.0f,           1.0f,
				};
			}

		private:
			struct Vertex {
				svec2<float32> vertex;
				svec2<float32> imageCoordinate;
			};

			struct Camera {
				mat3x3<float32> projection;
			};

		private:
			static constexpr uint64 MAX_CHARACTER_COUNT = 64;

		private:
			ref<Image> glyphMapImage;
			ref<ImageView> glyphMapImageView;

			ref<Sampler> glyphMapImageSampler;

			ref<Buffer> cameraBuffer;
			Camera* cameraMappedRange;

			ref<Subpass> subpass;
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> indexBuffer;

			ref<Buffer> vertexBuffer;
			Vertex* vertexMappedRange;

			uint32 characterCount = 0;
	};
}