// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class ImageSampleRenderer : public Renderer {
		public:
			void build() override {
				renderPass = createRenderPass();

				const ref<Application> application = getLayer()->getApplication();
				const view<char8> assetName = application->getPropertyFile()->getProperty<view<char8>>("AssetName");

				imageAsset = application->getAssetLibrary()->getAsset<ImageAsset>(assetName);
				imageFormat = getImageFormat();
				image = createImage();
				descriptorSetLayout = createDescriptorSetLayout();

				// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayout);

				descriptorSet = descriptorSets[0];
				pipelineLayout = createPipelineLayout();
				pipeline = createPipeline();
				stagingBuffer = createStagingBuffer();
				sampler = createSampler();
			}

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override {
				return {
					.buffers = stagingBuffer,
					.images = image,
				};
			}

			void postBuild() override {
				imageView = createImageView();

				createFramebuffers();
				updateDescriptorSets();
			}

			void encodeOneTimeCommands() const override {
				const BufferView<uint8> imageView = stagingBuffer->createView<uint8>(0, stagingBuffer->getSize());
				const view<uint8> data = imageAsset->getData();

				uint8* imageMappedRange = imageView.map();

				if (imageAsset->getFormat() == ImageFormat::R8G8B8_UNORM) {
					convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(data, imageMappedRange, imageView.getSize());
				}
				else {
					std::copy(data.begin(), data.end(), imageMappedRange);
				}

				stagingBuffer->unmap();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(BUILD_MODE_DEBUG)
						.name = "One-time commands",
					#endif
				});

				commandEncoder->begin();

				ImageMemoryBarrier imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
					.newAccess = Access::TRANSFER_WRITE,
				};

				commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

				const BufferImageCopyRegion bufferImageCopyRegion = {
					.bufferOffset = imageView.getOffset(),
					.imageOffsetX = 0,
					.imageOffsetY = 0,
					.imageOffsetZ = 0,
					.imageExtent = image->getExtent(),
					.imageMipLevel = 0,
					.imageLayerOffset = 0,
					.imageLayerCount = 1,
				};

				commandEncoder->copyBufferToImage(stagingBuffer, image, bufferImageCopyRegion);

				imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const ref<Framebuffer> framebuffer = framebuffers[getSwapchainImageIndex()];
				const Extent2D extent = framebuffer->getExtent();
				const float32 width = static_cast<float32>(extent.width);
				const float32 height = static_cast<float32>(extent.height);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffer,
					.clearValues = {{1.0f, 1.0f, 1.0f, 1.0f}},
				});
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindGraphicsPipeline(pipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, descriptorSet);
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				createFramebuffers();
			}

		private:
			ref<RenderPass> createRenderPass() const {
				return Renderer::createRenderPass({
					.attachments = {
						{
							.format = getSurface()->getFormat(),
							.sampleCount = 1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::STORE,
							.finalLayout = ImageLayout::PRESENT_SRC,
						},
					},
					.subpasses = {
						{
							.colorAttachmentReferences = {
								{
									.index = 0,
									.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
								},
							},
						},
					},
				});
			}

			ImageFormat getImageFormat() const {
				ImageFormat format = imageAsset->getFormat();

				switch (imageAsset->getFormat()) {
					case ImageFormat::R8_UNORM:
					case ImageFormat::R8G8B8A8_UNORM: {
						break;
					}
					case ImageFormat::R8G8B8_UNORM: {
						format = ImageFormat::R8G8B8A8_UNORM;

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				return format;
			}

			ref<Image> createImage() const {
				return createStaticImage({
					.type = ImageType::TEXTURE_2D,
					.format = imageFormat,
					.extent = imageAsset->getExtent(),
					.mipLevelCount = imageAsset->getMipLevelCount(),
					.layerCount = imageAsset->getLayerCount(),
					.sampleCount = 1,
					.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image",
					#endif
				});
			}

			ref<ImageView> createImageView() const {
				return image->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image",
					#endif
				});
			}

			void createFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor = {
					.renderPass = renderPass,
					.imageViews = {nullptr},
					.extent = swapchain->getExtent(),
				};

				uint32 framebufferIndex = 0;

				#if defined(DEBUG_OBJECT_NAME)
					sequence<char8> name;
				#endif

				while (framebufferIndex < framebuffers.count()) {
					#if defined(DEBUG_OBJECT_NAME)
						name = Logger::format("In flight []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			ref<DescriptorSetLayout> createDescriptorSetLayout() const {
				const DescriptorSetLayoutBinding binding = {
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::FRAGMENT,
				};

				return Renderer::createDescriptorSetLayout(binding);
			}

			ref<PipelineLayout> createPipelineLayout() const {
				return Renderer::createPipelineLayout({
					.descriptorSetLayouts = {descriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Pipeline Layout",
					#endif
				});
			}

			ref<GraphicsPipeline> createPipeline() const {
				const ColorBlendAttachment colorBlendAttachments[] = {
					{
						.blend = true,
						.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
						.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = pipelineLayout,
					.vertexInputBindings = {},
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Vertex",
							#endif
						},
						"Sample/Image/Image.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Fragment",
							#endif
						},
						"Sample/Image/Image.frag"
					),
					.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
					.polygonMode = PolygonMode::FILL,
					.cullMode = CullMode::BACK,
					.frontFace = FrontFace::CLOCKWISE,
					.lineWidth = 1.0f,
					.minSampleShading = 0.0f,
					.colorBlendState = {
						.attachments = colorBlendAttachments,
					},
					.depthStencilState = {
						.depthTest = false,
					},
					.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
					.renderPass = renderPass,
					.subpassIndex = 0,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Pipeline",
					#endif
				});
			}

			ref<Buffer> createStagingBuffer() const {
				return createStaticBuffer({
					.size = imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * getByteSize(imageFormat),
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
			}

			ref<Sampler> createSampler() const {
				return Renderer::createSampler({
					.magFilter = Filter::NEAREST,
					.minFilter = Filter::NEAREST,
					.addressModeU = AddressMode::REPEAT,
					.addressModeV = AddressMode::REPEAT,
					.addressModeW = AddressMode::REPEAT,
				});
			}

			void updateDescriptorSets() {
				const DescriptorImage descriptorImage = {
					.imageView = imageView,
					.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.sampler = sampler,
				};

				const WriteDescriptorSet writeDescriptorSet = {
					.descriptorSet = descriptorSet,
					.bindingIndex = 0,
					.descriptorImages = descriptorImage,
				};

				getDevice()->updateDescriptorSets(writeDescriptorSet);
			}

		private:
			ref<RenderPass> renderPass;
			ref<ImageAsset> imageAsset;
			ImageFormat imageFormat;
			ref<Image> image;
			ref<ImageView> imageView;
			sequence<ref<Framebuffer>> framebuffers;
			ref<DescriptorSetLayout> descriptorSetLayout;
			ref<DescriptorSet> descriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> pipeline;
			ref<Buffer> stagingBuffer;
			ref<Sampler> sampler;
	};

	DEFINE_SAMPLE_LAYER(ImageSample);
}