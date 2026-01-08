#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class SampleRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> position;
			};

			struct Instance {
				svec2<float32> vertexOffset;
				uint32 textureArrayIndex;
			};

		private:
			static constexpr uint32 ASSET_COUNT = 4;
			static constexpr uint32 TEXTURE_ARRAY_IMAGE_WIDTH = 512;
			static constexpr uint32 TEXTURE_ARRAY_IMAGE_HEIGHT = 512;
			static constexpr uint32 VERTEX_BUFFER_SIZE = 6 * sizeof(Vertex);
			static constexpr uint32 INSTANCE_BUFFER_SIZE = 4 * sizeof(Instance);

			static constexpr char8* ASSET_PATHS[] = {
				"Asset/Image/Baboon.png",
				"Asset/Image/Cameraman.png",
				"Asset/Image/Lenna.png",
				"Asset/Image/Pepper.png",
			};

		public:
			void build() override {
				setupRenderPass();

				createTextureArrayImage();
				createTextureArrayImageView();

				loadAssets();

				recreateFramebuffers();

				setupSampler();

				createTextureArrayImageDescriptorSetLayout();
				allocateTextureArrayImageDescriptorSet();
				updateTextureArrayImageDescriptorSet();

				setupPipelineLayout();
				setupRenderPipeline();

				createVertexAndInstanceBuffers();
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, {textureArrayDescriptorSet});
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(svec2<float32>));
				commandEncoder->bindVertexBuffer(1, instanceBuffer, 0, sizeof(Instance));
				commandEncoder->draw(24, 4, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
			}

		private:
			void setupRenderPass() {
				Attachment swapchainAttachment{};
				swapchainAttachment.format = getSurface()->getFormat();
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

			void createTextureArrayImage() {
				ImageDescriptor textureArrayImageDescriptor{};
				textureArrayImageDescriptor.type = ImageType::TEXTURE_2D;
				textureArrayImageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
				textureArrayImageDescriptor.extent.width = TEXTURE_ARRAY_IMAGE_WIDTH;
				textureArrayImageDescriptor.extent.height = TEXTURE_ARRAY_IMAGE_HEIGHT;
				textureArrayImageDescriptor.extent.depth = 1;
				textureArrayImageDescriptor.mipLevelCount = 1;
				textureArrayImageDescriptor.layerCount = 4;
				textureArrayImageDescriptor.sampleCount = 1;
				textureArrayImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
				textureArrayImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

				textureArrayImage = createImage(textureArrayImageDescriptor);
			}

			void createTextureArrayImageView() {
				ImageViewDescriptor textureArrayImageViewDescriptor{};
				textureArrayImageViewDescriptor.mipLevelOffset = 0;
				textureArrayImageViewDescriptor.mipLevelCount = textureArrayImage->getMipLevelCount();
				textureArrayImageViewDescriptor.layerOffset = 0;
				textureArrayImageViewDescriptor.layerCount = textureArrayImage->getLayerCount();

				textureArrayImageView = textureArrayImage->createView(textureArrayImageViewDescriptor);
			}

			void loadAssets() {
				sequence<File<FileFormat::PNG>> assets(ASSET_COUNT);
				uint64 textureArrayImageSize = 0;

				for (uint32 assetIndex = 0; assetIndex < assets.count(); assetIndex += 1) {
					const sequence<uint8> assetBuffer = Filesystem::openByteBuffer(ASSET_PATHS[assetIndex]);

					File<FileFormat::PNG>& asset = assets[assetIndex];
					asset = Filesystem::decode<FileFormat::PNG>(assetBuffer);

					textureArrayImageSize += asset.data.count();
				}

				const ref<Buffer> textureArrayImageStagingBuffer = createStagingBuffer(textureArrayImageSize);

				uint8* textureArrayImageStagingBufferMappedRange = textureArrayImageStagingBuffer->map<uint8>(0, textureArrayImageStagingBuffer->size);

				for (uint32 assetIndex = 0; assetIndex < assets.count(); assetIndex += 1) {
					const File<FileFormat::PNG>& asset = assets[assetIndex];

					std::copy(asset.data.begin(), asset.data.end(), textureArrayImageStagingBufferMappedRange);

					textureArrayImageStagingBufferMappedRange += asset.data.count();
				}

				textureArrayImageStagingBuffer->unmap();

				BufferImageCopyRegion bufferImageCopyRegion{};
				bufferImageCopyRegion.bufferOffset = 0;
				bufferImageCopyRegion.imageOffsetX = 0;
				bufferImageCopyRegion.imageOffsetY = 0;
				bufferImageCopyRegion.imageOffsetZ = 0;
				bufferImageCopyRegion.imageExtent = textureArrayImage->getExtent();
				bufferImageCopyRegion.imageMipLevel = 0;
				bufferImageCopyRegion.imageLayerOffset = 0;
				bufferImageCopyRegion.imageLayerCount = static_cast<uint32>(assets.count());

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->setImageMemoryBarrier(textureArrayImage, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
				commandEncoder->copyBufferToImage(textureArrayImageStagingBuffer, textureArrayImage, {bufferImageCopyRegion});
				commandEncoder->setImageMemoryBarrier(textureArrayImage, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				graphicsQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
				graphicsQueue->waitIdle();
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

				for (uint32 framebufferIndex = 0; framebufferIndex < framebuffers.count(); framebufferIndex += 1) {
					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
				}
			}

			void setupSampler() {
				SamplerDescriptor samplerDescriptor{};
				samplerDescriptor.magFilter = SamplerFilter::LINEAR;
				samplerDescriptor.minFilter = SamplerFilter::NEAREST;

				sampler = createSampler(samplerDescriptor);
			}

			void createTextureArrayImageDescriptorSetLayout() {
				DescriptorSetLayoutBinding textureArrayBinding{};
				textureArrayBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
				textureArrayBinding.descriptorCount = 1;
				textureArrayBinding.shaderStages = ShaderStage::FRAGMENT;

				textureArrayDescriptorSetLayout = createDescriptorSetLayout({textureArrayBinding});
			}

			void allocateTextureArrayImageDescriptorSet() {
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({textureArrayDescriptorSetLayout});

				textureArrayDescriptorSet = descriptorSets[0];
			}

			void updateTextureArrayImageDescriptorSet() {
				DescriptorImage textureArrayDescriptorImage{};
				textureArrayDescriptorImage.imageView = textureArrayImageView;
				textureArrayDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				textureArrayDescriptorImage.sampler = sampler;

				WriteDescriptorSet textureArrayWriteDescriptorSet{};
				textureArrayWriteDescriptorSet.descriptorSet = textureArrayDescriptorSet;
				textureArrayWriteDescriptorSet.bindingIndex = 0;
				textureArrayWriteDescriptorSet.descriptorImages = {textureArrayDescriptorImage};

				getDevice()->updateDescriptorSets({textureArrayWriteDescriptorSet});
			}

			void setupPipelineLayout() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};
				pipelineLayoutDescriptor.descriptorSetLayouts = {textureArrayDescriptorSetLayout};

				pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
			}

			void setupRenderPipeline() {
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

				const ref<ShaderModule> vertexShaderModule = loadShaderModule("VertexOffset2DUVTextureIndex.vert", vertexShaderModuleDescriptor);
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule("TextureArray.frag", fragmentShaderModuleDescriptor);

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				RenderPipelineDescriptor renderPipelineDescriptor{};
				renderPipelineDescriptor.pipelineLayout = pipelineLayout;
				renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
				renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
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

			void createVertexAndInstanceBuffers() {
				const ref<Buffer> stagingBuffer = createStagingBuffer(VERTEX_BUFFER_SIZE + INSTANCE_BUFFER_SIZE);

				uint8* stagingBufferMappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->size);

				Vertex* vertexStagingBufferMappedRange = pointer_cast<Vertex*>(stagingBufferMappedRange);
				vertexStagingBufferMappedRange[0].position = {-0.5f,  0.5f};
				vertexStagingBufferMappedRange[1].position = { 0.5f,  0.5f};
				vertexStagingBufferMappedRange[2].position = { 0.5f, -0.5f};
				vertexStagingBufferMappedRange[3].position = {-0.5f,  0.5f};
				vertexStagingBufferMappedRange[4].position = { 0.5f, -0.5f};
				vertexStagingBufferMappedRange[5].position = {-0.5f, -0.5f};

				Instance* instanceStagingBufferMappedRange = pointer_cast<Instance*>(stagingBufferMappedRange + VERTEX_BUFFER_SIZE);
				instanceStagingBufferMappedRange[0].vertexOffset = {-0.5f, 0.5f};
				instanceStagingBufferMappedRange[0].textureArrayIndex = 0;
				instanceStagingBufferMappedRange[1].vertexOffset = {0.5f, 0.5f};
				instanceStagingBufferMappedRange[1].textureArrayIndex = 1;
				instanceStagingBufferMappedRange[2].vertexOffset = {-0.5f, -0.5f};
				instanceStagingBufferMappedRange[2].textureArrayIndex = 2;
				instanceStagingBufferMappedRange[3].vertexOffset = {0.5f, -0.5f};
				instanceStagingBufferMappedRange[3].textureArrayIndex = 3;

				stagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
				vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				BufferDescriptor instanceBufferDescriptor{};
				instanceBufferDescriptor.size = INSTANCE_BUFFER_SIZE;
				instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				vertexBuffer = createBuffer(vertexBufferDescriptor);
				instanceBuffer = createBuffer(instanceBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->size;

				BufferCopyRegion instanceBufferCopyRegion{};
				instanceBufferCopyRegion.sourceOffset = vertexBuffer->size;
				instanceBufferCopyRegion.destinationOffset = 0;
				instanceBufferCopyRegion.size = instanceBuffer->size;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, {instanceBufferCopyRegion});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
				transferQueue->waitIdle();
			}

		private:
			ref<RenderPass> renderPass;
			ref<Image> textureArrayImage;
			ref<ImageView> textureArrayImageView;
			sequence<ref<Framebuffer>> framebuffers;
			ref<Sampler> sampler;
			ref<DescriptorSetLayout> textureArrayDescriptorSetLayout;
			ref<DescriptorSet> textureArrayDescriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> instanceBuffer;
	};
}