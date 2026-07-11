// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class SampleImageAssetRenderer : public Renderer {
		public:
			void build() override {
				renderPass = createRenderPass();

				loadImageAsset();
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
				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffers[getSwapchainImageIndex()],
					.clearValues = {
						{0.0f, 0.0f, 0.0f, 1.0f},
					},
				});
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

			void loadImageAsset() {
				const view<char8> imageAssetName = ((Layer*) getLayer())->getApplication()->getPropertyFile()->getProperty<view<char8>>("AssetName");
				const ref<ImageAsset> imageAsset = getLayer()->getApplication()->getAssetLibrary()->getAsset<ImageAsset>(imageAssetName);

				ImageFormat imageFormat = imageAsset->getFormat();

				switch (imageAsset->getFormat()) {
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

				const ref<Buffer> imageStagingBuffer = createDynamicBuffer({
					.size = imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * getChannelCount(imageFormat),
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image staging",
					#endif
				});
				const ref<BufferView<uint8>> imageStagingView = imageStagingBuffer->createView<uint8>(0, imageStagingBuffer->getSize());

				uint8* imageStagingBufferMappedRange = imageStagingView->map();

				if (imageAsset->getFormat() == ImageFormat::R8G8B8_UNORM) {
					convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), imageStagingBufferMappedRange, imageStagingBuffer->getSize());
				}
				else {
					std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), imageStagingBufferMappedRange);
				}

				imageStagingBuffer->unmap();

				image = createDynamicImage({
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

				const BufferImageCopyRegion bufferImageCopyRegion = {
					.bufferOffset = 0,
					.imageOffsetX = 0,
					.imageOffsetY = 0,
					.imageOffsetZ = 0,
					.imageExtent = image->getExtent(),
					.imageMipLevel = 0,
					.imageLayerOffset = 0,
					.imageLayerCount = 1,
				};

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(BUILD_MODE_DEBUG)
						.name = "Copy image",
					#endif
				});

				ImageMemoryBarrier imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
					.newAccess = Access::TRANSFER_WRITE,
				};

				commandEncoder->begin();
				commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
				commandEncoder->copyBufferToImage(imageStagingBuffer, image, bufferImageCopyRegion);

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

			void setupImageView() {
				imageView = image->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image asset",
					#endif
				});
			}

			void setupSampler() {
				sampler = createSampler({
					.magFilter = Filter::NEAREST,
					.minFilter = Filter::NEAREST,
					.addressModeU = AddressMode::REPEAT,
					.addressModeV = AddressMode::REPEAT,
					.addressModeW = AddressMode::REPEAT,
				});
			}

			void recreateFramebuffers() {
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
						name = Logger::format("In flight framebuffer []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			void createImageDescriptorSetLayout() {
				const DescriptorSetLayoutBinding imageBinding = {
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				};

				imageDescriptorSetLayout = createDescriptorSetLayout({imageBinding});
			}

			void allocateImageDescriptorSet() {
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(imageDescriptorSetLayout);

				imageDescriptorSet = descriptorSets[0];
			}

			void updateImageDescriptorSet() {
				const DescriptorImage descriptorImage = {
					.imageView = imageView,
					.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.sampler = sampler,
				};

				const WriteDescriptorSet imageWriteDescriptorSet = {
					.descriptorSet = imageDescriptorSet,
					.bindingIndex = 0,
					.descriptorImages = {descriptorImage},
				};

				getDevice()->updateDescriptorSets(imageWriteDescriptorSet);
			}

			void setupPipelineLayout() {
				pipelineLayout = createPipelineLayout({
					.descriptorSetLayouts = {imageDescriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image asset",
					#endif
				});
			}

			void setupGraphicsPipeline() {
				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = true,
						.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
						.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
					},
				};

				graphicsPipeline = createGraphicsPipeline({
					.pipelineLayout = pipelineLayout,
					.vertexInputBindings = {},
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Image asset vertex",
							#endif
						},
						"Sample/Image/Image.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Image asset fragment",
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
						.name = "Image asset",
					#endif
				});
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

	DEFINE_SAMPLE_LAYER(SampleImageAsset);
}