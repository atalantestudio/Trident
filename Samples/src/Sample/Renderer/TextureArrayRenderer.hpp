#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextureArrayRenderer : public Renderer {
		public:
			void build() override {
				const ref<Device> device = getDevice();

				std::array<File<FileFormat::PNG>, ASSET_PATHS.size()> assets;
				uint64 size = 0;

				// 1. Load assets.
				{
					for (uint32 assetIndex = 0; assetIndex < assets.size(); assetIndex += 1) {
						const std::string_view path = ASSET_PATHS[assetIndex];

						const Stream stream = Filesystem::openStream(path);

						assets[assetIndex] = Filesystem::decode<FileFormat::PNG>(stream);

						size += assets[assetIndex].data.count;
					}
				}

				ref<Buffer> textureArrayStagingBuffer;

				// 2. Create staging buffer to hold asset data.
				{
					textureArrayStagingBuffer = createStagingBuffer(size);
					uint8* textureArrayMappedRange = textureArrayStagingBuffer->map<uint8>(0, size);

					for (uint32 assetIndex = 0; assetIndex < assets.size(); assetIndex += 1) {
						const File<FileFormat::PNG>& asset = assets[assetIndex];

						std::copy(asset.data.begin(), asset.data.end(), textureArrayMappedRange);

						textureArrayMappedRange += asset.data.count;
					}

					textureArrayStagingBuffer->unmap();
				}

				// 3. Create texture array.
				{
					ImageDescriptor textureArrayDescriptor;
					textureArrayDescriptor.type = ImageType::TEXTURE_2D;
					textureArrayDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
					textureArrayDescriptor.extent.width = TEXTURE_ARRAY_WIDTH;
					textureArrayDescriptor.extent.height = TEXTURE_ARRAY_HEIGHT;
					textureArrayDescriptor.extent.depth = 1;
					textureArrayDescriptor.mipLevelCount = 1;
					textureArrayDescriptor.layerCount = 4;
					textureArrayDescriptor.sampleCount = 1;
					textureArrayDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
					textureArrayDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

					textureArray = createImage(textureArrayDescriptor);
				}

				// 4. Copy asset data to the texture array.
				{
					sequence<BufferImageCopyRegion> copyRegions(1);

					BufferImageCopyRegion& copyRegion = copyRegions[0];
					copyRegion.bufferOffset = 0;
					copyRegion.imageOffsetX = 0;
					copyRegion.imageOffsetY = 0;
					copyRegion.imageOffsetZ = 0;
					copyRegion.imageExtent = textureArray->getExtent();
					copyRegion.imageMipLevel = 0;
					copyRegion.imageLayerOffset = 0;
					copyRegion.imageLayerCount = static_cast<uint32>(assets.size());

					CommandBufferDescriptor commandBufferDescriptor;
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
					const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->setImageMemoryBarrier(textureArray, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
					commandEncoder->copyBufferToImage(textureArrayStagingBuffer, textureArray, copyRegions);
					commandEncoder->setImageMemoryBarrier(textureArray, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					graphicsQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}

				// 5. Create texture array view.
				{
					ImageViewDescriptor textureArrayViewDescriptor{};
					textureArrayViewDescriptor.mipLevelOffset = 0;
					textureArrayViewDescriptor.mipLevelCount = textureArray->getMipLevelCount();
					textureArrayViewDescriptor.layerOffset = 0;
					textureArrayViewDescriptor.layerCount = textureArray->getLayerCount();

					textureArrayView = textureArray->createView(textureArrayViewDescriptor);
				}

				// 6. Create sampler.
				{
					SamplerDescriptor samplerDescriptor{};
					samplerDescriptor.magFilter = SamplerFilter::LINEAR;
					samplerDescriptor.minFilter = SamplerFilter::NEAREST;

					sampler = createSampler(samplerDescriptor);
				}

				// 7. Create render pass and subpass.
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

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.subpasses = {subpass};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 8. Create framebuffers.
				recreateFramebuffers();

				// 9. Create pipeline layout.
				{
					DescriptorSetLayoutBinding textureArrayDescriptorSetLayoutBinding;
					textureArrayDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
					textureArrayDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
					textureArrayDescriptorSetLayoutBinding.imageViews = {textureArrayView};
					textureArrayDescriptorSetLayoutBinding.samplers = {sampler};

					DescriptorSetLayout descriptorSetLayout;
					descriptorSetLayout.bindings = {textureArrayDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor;
					pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 10. Create render pipeline.
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
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/TextureArray.frag", fragmentShaderModuleDescriptor);
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

				// 11. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 6 * sizeof(svec2<float32>);

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					svec2<float32>* vertexMappedRange = vertexStagingBuffer->map<svec2<float32>>(0, vertexBufferSize);

					vertexMappedRange[0] = {-0.5f,  0.5f};
					vertexMappedRange[1] = { 0.5f,  0.5f};
					vertexMappedRange[2] = { 0.5f, -0.5f};
					vertexMappedRange[3] = {-0.5f,  0.5f};
					vertexMappedRange[4] = { 0.5f, -0.5f};
					vertexMappedRange[5] = {-0.5f, -0.5f};

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

				// 12. Create instance buffer.
				{
					const ref<Device> device = getDevice();

					constexpr uint64 instanceVertexBufferSize = 4 * sizeof(Instance);

					const ref<Buffer> instanceVertexStagingBuffer = createStagingBuffer(instanceVertexBufferSize);

					Instance* instances = instanceVertexStagingBuffer->map<Instance>(0, instanceVertexBufferSize);

					instances[0].vertexOffset = {-0.5f, 0.5f};
					instances[0].textureArrayIndex = 0;
					instances[1].vertexOffset = {0.5f, 0.5f};
					instances[1].textureArrayIndex = 1;
					instances[2].vertexOffset = {-0.5f, -0.5f};
					instances[2].textureArrayIndex = 2;
					instances[3].vertexOffset = {0.5f, -0.5f};
					instances[3].textureArrayIndex = 3;

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
				const ref<Swapchain>& swapchain = getSwapchain();
				const float32 width = static_cast<float32>(swapchain->getExtent().width);
				const float32 height = static_cast<float32>(swapchain->getExtent().height);

				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(svec2<float32>));
				commandEncoder->bindVertexBuffer(1, instanceVertexBuffer, 0, sizeof(Instance));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtent());
				commandEncoder->draw(24, 4, 0);
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
			struct Instance {
				svec2<float32> vertexOffset;
				uint32 textureArrayIndex;
			};

		private:
			static constexpr std::array<std::string_view, 4> ASSET_PATHS = {
				"Asset/Image/Baboon.png",
				"Asset/Image/Cameraman.png",
				"Asset/Image/Lenna.png",
				"Asset/Image/Pepper.png",
			};

			static constexpr uint32 TEXTURE_ARRAY_WIDTH = 512;
			static constexpr uint32 TEXTURE_ARRAY_HEIGHT = 512;

		private:
			ref<Image> textureArray;
			ref<ImageView> textureArrayView;

			ref<Sampler> sampler;

			ref<Subpass> subpass;
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;

			ref<Buffer> instanceVertexBuffer;
	};
}