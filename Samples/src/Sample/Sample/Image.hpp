// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	class SampleRenderer : public Renderer {
		private:
			static constexpr view<char8> IMAGE_PATH = "Asset/Image/Zelda.png";

		public:
			void build() override {
				setupRenderPass();

				loadImage();
				setupImageView();
				setupSampler();

				recreateFramebuffers();

				createImageDescriptorSetLayout();
				allocateImageDescriptorSet();
				updateImageDescriptorSet();

				setupPipelineLayout();
				setupRenderPipeline();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
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

				ImageFormat imageFormat = file.format;

				switch (file.format) {
					case ImageFormat::R8_UNORM:
					case ImageFormat::R8G8B8A8_UNORM: {
						break;
					}
					case ImageFormat::R8G8B8_UNORM: {
						imageFormat = ImageFormat::R8G8B8A8_UNORM;

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				const ref<Buffer> imageStagingBuffer = createStagingBuffer(file.extent.width * file.extent.height * getChannelCount(imageFormat));

				uint8* imageStagingBufferMappedRange = imageStagingBuffer->map<uint8>(0, imageStagingBuffer->getSize());

				if (file.format == ImageFormat::R8G8B8_UNORM) {
					convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(file.data, imageStagingBufferMappedRange, imageStagingBuffer->getSize());
				}
				else {
					std::copy(file.data.begin(), file.data.end(), imageStagingBufferMappedRange);
				}

				imageStagingBuffer->unmap();

				ImageDescriptor imageDescriptor{};
				imageDescriptor.type = ImageType::TEXTURE_2D;
				imageDescriptor.format = imageFormat;
				imageDescriptor.extent = file.extent;
				imageDescriptor.mipLevelCount = 1;
				imageDescriptor.layerCount = 1;
				imageDescriptor.sampleCount = 1;
				imageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
				imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

				SET_DEBUG_NAME(imageDescriptor, "Image");

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

				graphicsQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
				graphicsQueue->waitIdle();
			}

			void setupImageView() {
				ImageViewDescriptor imageViewDescriptor{};
				imageViewDescriptor.mipLevelOffset = 0;
				imageViewDescriptor.mipLevelCount = 1;
				imageViewDescriptor.layerOffset = 0;
				imageViewDescriptor.layerCount = 1;

				SET_DEBUG_NAME(imageViewDescriptor, "Image view");

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

				SET_DEBUG_NAME(pipelineLayoutDescriptor, "Pipeline layout");

				pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
			}

			void setupRenderPipeline() {
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
				colorBlendAttachment.blend = true;
				colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
				colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

				RenderPipelineDescriptor renderPipelineDescriptor{};
				renderPipelineDescriptor.pipelineLayout = pipelineLayout;
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

				SET_DEBUG_NAME(renderPipelineDescriptor, "Render pipeline");

				renderPipeline = createRenderPipeline(renderPipelineDescriptor);
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
			ref<RenderPipeline> renderPipeline;
	};
}