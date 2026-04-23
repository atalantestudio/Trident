// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class SampleMipLevelsRenderer : public Renderer {
		private:
			static constexpr view<char8> IMAGE_ASSET_NAME = "Baboon";

		public:
			void build() override {
				setupRenderPass();

				loadImage();

				{
					ImageViewDescriptor imageViewDescriptor{};
					imageViewDescriptor.mipLevelOffset = 0;
					imageViewDescriptor.mipLevelCount = 1;
					imageViewDescriptor.layerOffset = 0;
					imageViewDescriptor.layerCount = 1;

					#if defined(DEBUG_OBJECT_NAME)
						imageViewDescriptor.name = "Input";
					#endif

					ref<ImageView> inputImageView = image->createView(imageViewDescriptor);

					imageViewDescriptor.layerOffset = 1;
					imageViewDescriptor.layerCount = image->getLayerCount() - 1;

					#if defined(DEBUG_OBJECT_NAME)
						imageViewDescriptor.name = "Output";
					#endif

					ref<ImageView> outputImageView = image->createView(imageViewDescriptor);

					generateMipLevels(inputImageView, outputImageView);
				}

				setupImageView();
				setupSampler();

				recreateFramebuffers();

				createImageDescriptorSetLayout();
				allocateImageDescriptorSet();
				updateImageDescriptorSet();

				setupPipelineLayout();
				setupGraphicsPipeline();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getSwapchainImageIndex()];
				beginRenderPassDescriptor.clearValues = {{1.0f, 1.0f, 1.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->bindGraphicsPipeline(graphicsPipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, imageDescriptorSet);
				commandEncoder->draw(3, 1, 0, 0);
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

			void loadImage() {
				const ref<ImageAsset> imageAsset = getLayer()->getApplication()->getAssetLibrary()->getAsset<ImageAsset>(IMAGE_ASSET_NAME);
				const ref<Buffer<uint8>> imageStagingBuffer = createStagingBuffer<uint8>(imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * getChannelCount(imageAsset->getFormat()));

				DEBUG_ASSERT(imageAsset->getFormat() == ImageFormat::R8G8B8A8_UNORM);

				uint8* imageStagingBufferMappedRange = imageStagingBuffer->map(0, imageStagingBuffer->getSize());

				std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), imageStagingBufferMappedRange);

				imageStagingBuffer->unmap();

				ImageDescriptor imageDescriptor{};
				imageDescriptor.type = ImageType::TEXTURE_2D;
				imageDescriptor.format = imageAsset->getFormat();
				imageDescriptor.extent = imageAsset->getExtent();
				imageDescriptor.mipLevelCount = 1;
				imageDescriptor.layerCount = 2;
				imageDescriptor.sampleCount = 1;
				imageDescriptor.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED | ImageUsage::STORAGE;
				imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

				#if defined(DEBUG_OBJECT_NAME)
					imageDescriptor.name = "Input";
				#endif

				image = createImage(imageDescriptor);

				BufferImageCopyRegion bufferImageCopyRegion{};
				bufferImageCopyRegion.bufferOffset = 0;
				bufferImageCopyRegion.imageOffsetX = 0;
				bufferImageCopyRegion.imageOffsetY = 0;
				bufferImageCopyRegion.imageOffsetZ = 0;
				bufferImageCopyRegion.imageExtent = image->getExtent();
				bufferImageCopyRegion.imageMipLevel = 0;
				bufferImageCopyRegion.imageLayerOffset = 0;
				bufferImageCopyRegion.imageLayerCount = 1;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				#if defined(DEBUG_OBJECT_NAME)
					commandBufferDescriptor.name = "Image copy command buffer";
				#endif

				const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

				ImageMemoryBarrier imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
					.newAccess = Access::TRANSFER_WRITE,
				};

				commandEncoder->begin();
				commandEncoder->insertMemoryBarriers({}, {imageMemoryBarrier});
				commandEncoder->copyBufferToImage(imageStagingBuffer, image, {bufferImageCopyRegion});

				imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({}, {imageMemoryBarrier});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				graphicsQueue->submit(commandBuffer);
				graphicsQueue->waitIdle();
			}

			void setupImageView() {
				ImageViewDescriptor imageViewDescriptor{};
				imageViewDescriptor.mipLevelOffset = 0;
				imageViewDescriptor.mipLevelCount = 1;
				imageViewDescriptor.layerOffset = 1;
				imageViewDescriptor.layerCount = 1;

				#if defined(DEBUG_OBJECT_NAME)
					imageViewDescriptor.name = "Render";
				#endif

				imageView = image->createView(imageViewDescriptor);
			}

			void setupSampler() {
				SamplerDescriptor samplerDescriptor{};
				samplerDescriptor.magFilter = SamplerFilter::NEAREST;
				samplerDescriptor.minFilter = SamplerFilter::NEAREST;

				sampler = createSampler(samplerDescriptor);
			}

			void recreateFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr};
				framebufferDescriptor.extent = swapchain->getExtent();

				#if defined(DEBUG_OBJECT_NAME)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

				while (framebufferIndex < framebuffers.count()) {
					#if defined(DEBUG_OBJECT_NAME)
						name = Logger::format("In flight framebuffer []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			void createImageDescriptorSetLayout() {
				DescriptorSetLayoutBinding imageBinding{};
				imageBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
				imageBinding.descriptorCount = 1;
				imageBinding.shaderStages = ShaderStage::FRAGMENT;

				imageDescriptorSetLayout = createDescriptorSetLayout({imageBinding});
			}

			void allocateImageDescriptorSet() {
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(imageDescriptorSetLayout);

				imageDescriptorSet = descriptorSets[0];
			}

			void updateImageDescriptorSet() {
				DescriptorImage descriptorImage{};
				descriptorImage.imageView = imageView;
				descriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				descriptorImage.sampler = sampler;

				WriteDescriptorSet imageWriteDescriptorSet{};
				imageWriteDescriptorSet.descriptorSet = imageDescriptorSet;
				imageWriteDescriptorSet.bindingIndex = 0;
				imageWriteDescriptorSet.descriptorImages = {descriptorImage};

				getDevice()->updateDescriptorSets({imageWriteDescriptorSet});
			}

			void setupPipelineLayout() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};
				pipelineLayoutDescriptor.descriptorSetLayouts = {imageDescriptorSetLayout};

				#if defined(DEBUG_OBJECT_NAME)
					pipelineLayoutDescriptor.name = "Render pipeline layout";
				#endif

				pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
			}

			void setupGraphicsPipeline() {
				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				#if defined(DEBUG_OBJECT_NAME)
					vertexShaderModuleDescriptor.name = "Vertex shader module";
				#endif

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				#if defined(DEBUG_OBJECT_NAME)
					fragmentShaderModuleDescriptor.name = "Fragment shader module";
				#endif

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/Image/Image.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/Image/Image.frag");

				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = false,
					},
				};

				GraphicsPipelineDescriptor graphicsPipelineDescriptor{};
				graphicsPipelineDescriptor.pipelineLayout = pipelineLayout;
				graphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				graphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				graphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				graphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
				graphicsPipelineDescriptor.cullMode = CullMode::BACK;
				graphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				graphicsPipelineDescriptor.lineWidth = 1.0f;
				graphicsPipelineDescriptor.minSampleShading = 0.0f;
				graphicsPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
				graphicsPipelineDescriptor.depthStencilState.depthTest = false;
				graphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				graphicsPipelineDescriptor.renderPass = renderPass;
				graphicsPipelineDescriptor.subpassIndex = 0;

				#if defined(DEBUG_OBJECT_NAME)
					graphicsPipelineDescriptor.name = "Render pipeline";
				#endif

				graphicsPipeline = createGraphicsPipeline(graphicsPipelineDescriptor);
			}

			void generateMipLevels(ref<ImageView> inputImageView, ref<ImageView> outputImageView) const {
				DescriptorSetLayoutBinding inputImageBinding{};
				inputImageBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE;
				inputImageBinding.descriptorCount = 1;
				inputImageBinding.shaderStages = ShaderStage::COMPUTE;

				DescriptorSetLayoutBinding outputImageBinding{};
				outputImageBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE;
				outputImageBinding.descriptorCount = 1;
				outputImageBinding.shaderStages = ShaderStage::COMPUTE;

				ref<DescriptorSetLayout> descriptorSetLayout = createDescriptorSetLayout({inputImageBinding, outputImageBinding});

				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayout);

				ref<DescriptorSet> descriptorSet = descriptorSets[0];

				DescriptorImage inputDescriptorImage{};
				inputDescriptorImage.imageView = inputImageView;
				inputDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				inputDescriptorImage.sampler = nullptr;

				DescriptorImage outputDescriptorImage{};
				outputDescriptorImage.imageView = outputImageView;
				outputDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				outputDescriptorImage.sampler = nullptr;

				const sequence<WriteDescriptorSet> writeDescriptorSets = {
					{
						.descriptorSet = descriptorSet,
						.bindingIndex = 0,
						.descriptorImages = {inputDescriptorImage},
					},
					{
						.descriptorSet = descriptorSet,
						.bindingIndex = 1,
						.descriptorImages = {outputDescriptorImage},
					},
				};

				getDevice()->updateDescriptorSets(writeDescriptorSets);

				PipelineLayoutDescriptor pipelineLayoutDescriptor{};
				pipelineLayoutDescriptor.descriptorSetLayouts = {descriptorSetLayout};

				#if defined(DEBUG_OBJECT_NAME)
					pipelineLayoutDescriptor.name = "Compute pipeline";
				#endif

				ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				ShaderModuleDescriptor shaderModuleDescriptor{};
				shaderModuleDescriptor.stage = ShaderStage::COMPUTE;
				shaderModuleDescriptor.entryPoint = "main";

				#if defined(DEBUG_OBJECT_NAME)
					shaderModuleDescriptor.name = "Compute shader module";
				#endif

				ref<ShaderModule> shaderModule = loadShaderModule(shaderModuleDescriptor, "Sample/MipLevels/GenerateMipLevels.comp");

				ComputePipelineDescriptor computePipelineDescriptor{};
				computePipelineDescriptor.pipelineLayout = pipelineLayout;
				computePipelineDescriptor.computeShaderModule = shaderModule;

				#if defined(DEBUG_OBJECT_NAME)
					computePipelineDescriptor.name = "Compute pipeline";
				#endif

				ref<ComputePipeline> computePipeline = createComputePipeline(computePipelineDescriptor);

				ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::COMPUTE);

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				#if defined(DEBUG_OBJECT_NAME)
					commandBufferDescriptor.name = "Compute command buffer";
				#endif

				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);
				const Extent3D imageExtent = inputImageView->getImage()->getExtent();

				commandEncoder->begin();
				commandEncoder->bindComputePipeline(computePipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, descriptorSet);
				commandEncoder->dispatch((imageExtent.width + 0b111111) >> 6, (imageExtent.height + 0b111111) >> 6, 1);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

		private:
			ref<RenderPass> renderPass;
			ref<Image> image;
			ref<ImageView> imageView;
			ref<Sampler> sampler;
			sequence<ref<Framebuffer>> framebuffers;
			ref<DescriptorSetLayout> imageDescriptorSetLayout;
			ref<DescriptorSet> imageDescriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> graphicsPipeline;
	};

	DEFINE_SAMPLE_LAYER(SampleMipLevels);
}