// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../SampleLayer.hpp"

namespace Trident {
	class SampleArrayTextureRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> position;
			};

			struct Instance {
				svec2<float32> positionOffset;
				uint32 textureIndex;
			};

		private:
			static constexpr uint32 ASSET_COUNT = 4;
			static constexpr uint32 VERTEX_BUFFER_SIZE = 6 * sizeof(Vertex);
			static constexpr uint32 INSTANCE_BUFFER_SIZE = 4 * sizeof(Instance);
			static constexpr view<char8> IMAGE_ASSET_PATHS[] = {
				"Baboon",
				"Cameraman",
				"Lenna",
				"Pepper",
			};

		public:
			void build() override {
				setupRenderPass();
				loadAssets();
				setupSampler();

				recreateFramebuffers();

				createArrayTextureDescriptorSetLayout();
				allocateArrayTextureDescriptorSet();
				updateArrayTextureDescriptorSet();

				setupPipelineLayout();
				setupGraphicsPipeline();

				createBuffers();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getSwapchainImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindGraphicsPipeline(graphicsPipeline);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, {arrayTextureDescriptorSet});
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->bindVertexBuffer(1, instanceBuffer, 0, sizeof(Instance));
				commandEncoder->draw(6, 4, 0, 0);
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

			void loadAssets() {
				images = sequence<ref<Image>>(ASSET_COUNT);
				imageViews = sequence<ref<ImageView>>(ASSET_COUNT);

				sequence<ref<ImageAsset>> imageAssets(ASSET_COUNT);

				uint64 stagingBufferSize = 0;

				ImageDescriptor imageDescriptor{};
				imageDescriptor.type = ImageType::TEXTURE_2D;
				imageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
				imageDescriptor.mipLevelCount = 1;
				imageDescriptor.layerCount = 1;
				imageDescriptor.sampleCount = 1;
				imageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
				imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

				SET_DEBUG_NAME(imageDescriptor, "Array texture");

				ImageViewDescriptor imageViewDescriptor{};
				imageViewDescriptor.mipLevelOffset = 0;
				imageViewDescriptor.mipLevelCount = 1;
				imageViewDescriptor.layerOffset = 0;
				imageViewDescriptor.layerCount = 1;

				SET_DEBUG_NAME(imageViewDescriptor, "Array texture");

				AssetLibrary* assetLibrary = getLayer()->getApplication()->getAssetLibrary();
				uint32 imageAssetIndex = 0;

				while (imageAssetIndex < imageAssets.count()) {
					const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(IMAGE_ASSET_PATHS[imageAssetIndex]);

					stagingBufferSize += imageAsset->getData().count();
					imageDescriptor.extent = imageAsset->getExtent();

					images[imageAssetIndex] = createImage(imageDescriptor);
					imageViews[imageAssetIndex] = images[imageAssetIndex]->createView(imageViewDescriptor);
					imageAssets[imageAssetIndex] = imageAsset;

					imageAssetIndex += 1;
				}

				const ref<Buffer<uint8>> stagingBuffer = createStagingBuffer<uint8>(stagingBufferSize);

				uint8* stagingBufferMappedRange = stagingBuffer->map(0, stagingBuffer->getSize());

				for (const ref<ImageAsset> imageAsset : imageAssets) {
					std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), stagingBufferMappedRange);

					stagingBufferMappedRange += imageAsset->getData().count();
				}

				stagingBuffer->unmap();

				BufferImageCopyRegion bufferImageCopyRegion{};
				bufferImageCopyRegion.bufferOffset = 0;
				bufferImageCopyRegion.imageOffsetX = 0;
				bufferImageCopyRegion.imageOffsetY = 0;
				bufferImageCopyRegion.imageOffsetZ = 0;
				bufferImageCopyRegion.imageMipLevel = 0;
				bufferImageCopyRegion.imageLayerOffset = 0;
				bufferImageCopyRegion.imageLayerCount = 1;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Image copy command buffer");

				const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();

				imageAssetIndex = 0;

				while (imageAssetIndex < ASSET_COUNT) {
					const ref<Image> image = images[imageAssetIndex];

					bufferImageCopyRegion.imageExtent = image->getExtent();

					commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
					commandEncoder->copyBufferToImage(stagingBuffer, image, {bufferImageCopyRegion});
					commandEncoder->setImageMemoryBarrier(image, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					bufferImageCopyRegion.bufferOffset += imageAssets[imageAssetIndex]->getData().count();

					imageAssetIndex += 1;
				}

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				graphicsQueue->submit({commandBuffer});
				graphicsQueue->waitIdle();
			}

			void setupSampler() {
				SamplerDescriptor samplerDescriptor{};
				samplerDescriptor.magFilter = SamplerFilter::LINEAR;
				samplerDescriptor.minFilter = SamplerFilter::NEAREST;

				sampler = createSampler(samplerDescriptor);
			}

			void createArrayTextureDescriptorSetLayout() {
				DescriptorSetLayoutBinding arrayTextureBinding{};
				arrayTextureBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
				arrayTextureBinding.descriptorCount = ASSET_COUNT;
				arrayTextureBinding.shaderStages = ShaderStage::FRAGMENT;

				arrayTextureDescriptorSetLayout = createDescriptorSetLayout({arrayTextureBinding});
			}

			void allocateArrayTextureDescriptorSet() {
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({arrayTextureDescriptorSetLayout});

				arrayTextureDescriptorSet = descriptorSets[0];
			}

			void updateArrayTextureDescriptorSet() {
				sequence<DescriptorImage> descriptorImages(ASSET_COUNT);

				for (uint32 descriptorImageIndex = 0; descriptorImageIndex < descriptorImages.count(); descriptorImageIndex += 1) {
					DescriptorImage& descriptorImage = descriptorImages[descriptorImageIndex];
					descriptorImage.imageView = imageViews[descriptorImageIndex];
					descriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
					descriptorImage.sampler = sampler;
				}

				WriteDescriptorSet arrayTextureWriteDescriptorSet{};
				arrayTextureWriteDescriptorSet.descriptorSet = arrayTextureDescriptorSet;
				arrayTextureWriteDescriptorSet.bindingIndex = 0;
				arrayTextureWriteDescriptorSet.descriptorImages = descriptorImages;

				getDevice()->updateDescriptorSets({arrayTextureWriteDescriptorSet});
			}

			void recreateFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr};
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

			void setupPipelineLayout() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};
				pipelineLayoutDescriptor.descriptorSetLayouts = {arrayTextureDescriptorSetLayout};

				SET_DEBUG_NAME(pipelineLayoutDescriptor, "Pipeline layout");

				pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
			}

			void setupGraphicsPipeline() {
				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<svec2<float32>>();

				VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
				instanceInputBinding.registerAttribute<svec2<float32>>();
				instanceInputBinding.registerAttribute<uint32>();

				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Vertex shader module");

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Fragment shader module");

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/ArrayTexture/ArrayTexture.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/ArrayTexture/ArrayTexture.frag");

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				GraphicsPipelineDescriptor graphicsPipelineDescriptor{};
				graphicsPipelineDescriptor.pipelineLayout = pipelineLayout;
				graphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
				graphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				graphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				graphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				graphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
				graphicsPipelineDescriptor.cullMode = CullMode::BACK;
				graphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				graphicsPipelineDescriptor.lineWidth = 1.0f;
				graphicsPipelineDescriptor.minSampleShading = 0.0f;
				graphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				graphicsPipelineDescriptor.depthStencilState.depthTest = false;
				graphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				graphicsPipelineDescriptor.renderPass = renderPass;
				graphicsPipelineDescriptor.subpassIndex = 0;

				SET_DEBUG_NAME(graphicsPipelineDescriptor, "Render pipeline");

				graphicsPipeline = createGraphicsPipeline(graphicsPipelineDescriptor);
			}

			void createBuffers() {
				const ref<Buffer<uint8>> stagingBuffer = createStagingBuffer<uint8>(VERTEX_BUFFER_SIZE + INSTANCE_BUFFER_SIZE);

				uint8* stagingBufferMappedRange = stagingBuffer->map(0, stagingBuffer->getSize());

				Vertex* vertexStagingBufferMappedRange = pointer_cast<Vertex*>(stagingBufferMappedRange);
				vertexStagingBufferMappedRange[0].position = {-0.5f,  0.5f};
				vertexStagingBufferMappedRange[1].position = { 0.5f,  0.5f};
				vertexStagingBufferMappedRange[2].position = { 0.5f, -0.5f};
				vertexStagingBufferMappedRange[3].position = {-0.5f,  0.5f};
				vertexStagingBufferMappedRange[4].position = { 0.5f, -0.5f};
				vertexStagingBufferMappedRange[5].position = {-0.5f, -0.5f};

				Instance* instanceStagingBufferMappedRange = pointer_cast<Instance*>(stagingBufferMappedRange + VERTEX_BUFFER_SIZE);
				instanceStagingBufferMappedRange[0].positionOffset = {-0.5f, 0.5f};
				instanceStagingBufferMappedRange[0].textureIndex = 0;
				instanceStagingBufferMappedRange[1].positionOffset = {0.5f, 0.5f};
				instanceStagingBufferMappedRange[1].textureIndex = 1;
				instanceStagingBufferMappedRange[2].positionOffset = {-0.5f, -0.5f};
				instanceStagingBufferMappedRange[2].textureIndex = 2;
				instanceStagingBufferMappedRange[3].positionOffset = {0.5f, -0.5f};
				instanceStagingBufferMappedRange[3].textureIndex = 3;

				stagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
				vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				SET_DEBUG_NAME(vertexBufferDescriptor, "Vertex buffer");

				BufferDescriptor instanceBufferDescriptor{};
				instanceBufferDescriptor.size = INSTANCE_BUFFER_SIZE;
				instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				SET_DEBUG_NAME(instanceBufferDescriptor, "Instance buffer");

				vertexBuffer = createBuffer<Vertex>(vertexBufferDescriptor);
				instanceBuffer = createBuffer<Instance>(instanceBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->getSize();

				BufferCopyRegion instanceBufferCopyRegion{};
				instanceBufferCopyRegion.sourceOffset = vertexBuffer->getSize();
				instanceBufferCopyRegion.destinationOffset = 0;
				instanceBufferCopyRegion.size = instanceBuffer->getSize();

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Buffer copy command buffer");

				const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, {instanceBufferCopyRegion});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit({commandBuffer});
				transferQueue->waitIdle();
			}

		private:
			ref<RenderPass> renderPass;
			sequence<ref<Image>> images;
			sequence<ref<ImageView>> imageViews;
			ref<Sampler> sampler;
			sequence<ref<Framebuffer>> framebuffers;
			ref<DescriptorSetLayout> arrayTextureDescriptorSetLayout;
			ref<DescriptorSet> arrayTextureDescriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> graphicsPipeline;
			ref<Buffer<Vertex>> vertexBuffer;
			ref<Buffer<Instance>> instanceBuffer;
	};

	DEFINE_SAMPLE_LAYER(SampleArrayTexture);
}