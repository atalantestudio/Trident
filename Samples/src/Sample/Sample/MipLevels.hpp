// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	class SampleRenderer : public Renderer {
		private:
			static constexpr view<char8> IMAGE_PATH = "Asset/Image/Baboon.png";

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

					SET_DEBUG_NAME(imageViewDescriptor, "Input");

					ref<ImageView> inputImageView = image->createView(imageViewDescriptor);

					imageViewDescriptor.layerOffset = 1;
					imageViewDescriptor.layerCount = image->getLayerCount() - 1;

					SET_DEBUG_NAME(imageViewDescriptor, "Output");

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
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, {imageDescriptorSet});
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
				const sequence<uint8> buffer = Filesystem::openByteBuffer(IMAGE_PATH);
				const File<FileFormat::PNG> file = Filesystem::decode<FileFormat::PNG>(buffer);

				const ref<Buffer> imageStagingBuffer = createStagingBuffer(file.data.count());

				uint8* imageStagingBufferMappedRange = imageStagingBuffer->map<uint8>(0, imageStagingBuffer->getSize());

				std::copy(file.data.begin(), file.data.end(), imageStagingBufferMappedRange);

				imageStagingBuffer->unmap();

				// NOTE: On Vulkan, the data of an RGB image must first be copied to an RGBA image which supports TRANSFER_DST usage.
				ImageDescriptor imageDescriptor{};
				imageDescriptor.type = ImageType::TEXTURE_2D;
				imageDescriptor.format = file.format;
				imageDescriptor.extent = file.extent;
				imageDescriptor.mipLevelCount = 1;
				imageDescriptor.layerCount = 2;
				imageDescriptor.sampleCount = 1;
				imageDescriptor.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED | ImageUsage::STORAGE;
				imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

				SET_DEBUG_NAME(imageDescriptor, "Input");

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

				SET_DEBUG_NAME(commandBufferDescriptor, "Image copy command buffer");

				const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
				commandEncoder->copyBufferToImage(imageStagingBuffer, image, {bufferImageCopyRegion});
				commandEncoder->setImageMemoryBarrier(image, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				graphicsQueue->submit({commandBuffer});
				graphicsQueue->waitIdle();
			}

			void setupImageView() {
				ImageViewDescriptor imageViewDescriptor{};
				imageViewDescriptor.mipLevelOffset = 0;
				imageViewDescriptor.mipLevelCount = 1;
				imageViewDescriptor.layerOffset = 1;
				imageViewDescriptor.layerCount = 1;

				SET_DEBUG_NAME(imageViewDescriptor, "Render");

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

			void createImageDescriptorSetLayout() {
				DescriptorSetLayoutBinding imageBinding{};
				imageBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
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

				SET_DEBUG_NAME(pipelineLayoutDescriptor, "Render pipeline layout");

				pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
			}

			void setupGraphicsPipeline() {
				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Vertex shader module");

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Fragment shader module");

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/Image/Image.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/Image/Image.frag");

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

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
				graphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				graphicsPipelineDescriptor.depthStencilState.depthTest = false;
				graphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				graphicsPipelineDescriptor.renderPass = renderPass;
				graphicsPipelineDescriptor.subpassIndex = 0;

				SET_DEBUG_NAME(graphicsPipelineDescriptor, "Render pipeline");

				graphicsPipeline = createGraphicsPipeline(graphicsPipelineDescriptor);
			}

			void generateMipLevels(ref<ImageView> inputImageView, ref<ImageView> outputImageView) const {
				DescriptorSetLayoutBinding descriptorSetLayoutBinding{};
				descriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
				descriptorSetLayoutBinding.descriptorCount = 2; // 0 = input, 1 = output
				descriptorSetLayoutBinding.shaderStages = ShaderStage::COMPUTE;

				ref<DescriptorSetLayout> descriptorSetLayout = createDescriptorSetLayout({descriptorSetLayoutBinding});

				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({descriptorSetLayout});

				ref<DescriptorSet> descriptorSet = descriptorSets[0];

				DescriptorImage inputDescriptorImage{};
				inputDescriptorImage.imageView = inputImageView;
				inputDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				inputDescriptorImage.sampler = nullptr;

				DescriptorImage outputDescriptorImage{};
				outputDescriptorImage.imageView = outputImageView;
				outputDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				outputDescriptorImage.sampler = nullptr;

				WriteDescriptorSet writeDescriptorSet{};
				writeDescriptorSet.descriptorSet = descriptorSet;
				writeDescriptorSet.bindingIndex = 0;
				writeDescriptorSet.descriptorImages = {inputDescriptorImage, outputDescriptorImage};

				getDevice()->updateDescriptorSets({writeDescriptorSet});

				PipelineLayoutDescriptor pipelineLayoutDescriptor{};
				pipelineLayoutDescriptor.descriptorSetLayouts = {descriptorSetLayout};

				SET_DEBUG_NAME(pipelineLayoutDescriptor, "Compute pipeline");

				ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				ShaderModuleDescriptor shaderModuleDescriptor{};
				shaderModuleDescriptor.stage = ShaderStage::COMPUTE;
				shaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(shaderModuleDescriptor, "Compute shader module");

				ref<ShaderModule> shaderModule = loadShaderModule(shaderModuleDescriptor, "Sample/MipLevels/GenerateMipLevels.comp");

				ComputePipelineDescriptor computePipelineDescriptor{};
				computePipelineDescriptor.pipelineLayout = pipelineLayout;
				computePipelineDescriptor.computeShaderModule = shaderModule;

				SET_DEBUG_NAME(computePipelineDescriptor, "Compute pipeline");

				ref<ComputePipeline> computePipeline = createComputePipeline(computePipelineDescriptor);

				ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::COMPUTE);

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Compute command buffer");

				ref<CommandEncoder> commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);

				const Extent3D imageExtent = inputImageView->getImage()->getExtent();

				commandEncoder->begin();
				commandEncoder->bindComputePipeline(computePipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, {descriptorSet});
				commandEncoder->dispatch((imageExtent.width + 0b111111) >> 6, (imageExtent.height + 0b111111) >> 6, 1);

				ref<CommandBuffer> commandBuffer = commandEncoder->end();

				// TODO: Create one-time submit overload (without semaphores/fence).
				queue->submit({commandBuffer});

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
}