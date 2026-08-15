// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

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
			static constexpr uint32 IMAGE_COUNT = 4;
			static constexpr view<char8> IMAGE_ASSET_PATHS[IMAGE_COUNT] = {
				"Baboon",
				"Cameraman",
				"Lenna",
				"Pepper",
			};

		public:
			void build() override {
				renderPass = createRenderPass();

				createFramebuffers();

				arrayTextureDescriptorSetLayout = createArrayTextureDescriptorSetLayout();

				// TODO(graphics/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(arrayTextureDescriptorSetLayout);

				arrayTextureDescriptorSet = descriptorSets[0];
				arrayTexturePipelineLayout = createArrayTexturePipelineLayout();
				arrayTexturePipeline = createArrayTexturePipeline();
				vertexBuffer = createVertexBuffer();

				AssetLibrary* assetLibrary = getLayer()->getApplication()->getAssetLibrary();
				uint32 imageIndex = 0;

				while (imageIndex < IMAGE_COUNT) {
					const view<char8> imageAssetPath = IMAGE_ASSET_PATHS[imageIndex];
					const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(imageAssetPath);

					images[imageIndex] = createImage(imageAsset, imageIndex);

					imageIndex += 1;
				}

				stagingBuffer = createStagingBuffer();
				sampler = createSampler();
			}

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override {
				sequence<ref<Image>> staticImages(IMAGE_COUNT);

				std::copy_n(&images[0], IMAGE_COUNT, &staticImages[0]);

				// TODO(graphics/ref): Remove.
				for (ref<Image>& image : staticImages) {
					image.owning = false;
				}

				return {
					.buffers = {vertexBuffer, stagingBuffer},
					.images = staticImages,
				};
			};

			void postBuild() override {
				uint32 imageIndex = 0;

				while (imageIndex < IMAGE_COUNT) {
					imageViews[imageIndex] = createImageView(images[imageIndex], imageIndex);

					imageIndex += 1;
				}

				updateArrayTextureDescriptorSet();
			}

			void encodeOneTimeCommands() const override {
				const BufferView<Vertex> vertexStagingBufferView = stagingBuffer->createView<Vertex>(0, 6 * sizeof(Vertex));
				const BufferView<Instance> instanceStagingBufferView = stagingBuffer->createView<Instance>(vertexStagingBufferView.getSize(), 4 * sizeof(Instance));
				const uint64 imageStagingBufferViewOffset = vertexStagingBufferView.getSize() + instanceStagingBufferView.getSize();
				const uint64 imageStagingBufferViewSize = stagingBuffer->getSize() - imageStagingBufferViewOffset;
				const BufferView<uint8> imageStagingBufferView = stagingBuffer->createView<uint8>(imageStagingBufferViewOffset, imageStagingBufferViewSize);

				AssetLibrary* assetLibrary = getLayer()->getApplication()->getAssetLibrary();

				Vertex* vertexMappedRange = vertexStagingBufferView.map();
				vertexMappedRange[0] = {{-0.5f,  0.5f}};
				vertexMappedRange[1] = {{ 0.5f,  0.5f}};
				vertexMappedRange[2] = {{ 0.5f, -0.5f}};
				vertexMappedRange[3] = {{-0.5f,  0.5f}};
				vertexMappedRange[4] = {{ 0.5f, -0.5f}};
				vertexMappedRange[5] = {{-0.5f, -0.5f}};

				Instance* instanceMappedRange = instanceStagingBufferView.map();
				instanceMappedRange[0] = {{-0.5f,  0.5f}, 0};
				instanceMappedRange[1] = {{ 0.5f,  0.5f}, 1};
				instanceMappedRange[2] = {{-0.5f, -0.5f}, 2};
				instanceMappedRange[3] = {{ 0.5f, -0.5f}, 3};

				uint8* imageMappedRange = imageStagingBufferView.map();
				uint32 imageIndex = 0;

				while (imageIndex < IMAGE_COUNT) {
					const view<char8> imageAssetPath = IMAGE_ASSET_PATHS[imageIndex];
					const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(imageAssetPath);

					std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), imageMappedRange);

					imageMappedRange += imageAsset->getSize();
					imageIndex += 1;
				}

				stagingBuffer->unmap();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Copy buffers/images",
					#endif
				});

				commandEncoder->begin();

				const BufferCopyRegion bufferCopyRegion = {
					.sourceOffset = vertexStagingBufferView.getOffset(),
					.destinationOffset = 0,
					.size = vertexStagingBufferView.getSize() + instanceStagingBufferView.getSize(),
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegion);

				BufferImageCopyRegion bufferImageCopyRegion = {
					.bufferOffset = imageStagingBufferView.getOffset(),
					.imageOffsetX = 0,
					.imageOffsetY = 0,
					.imageOffsetZ = 0,
					.imageMipLevel = 0,
					.imageLayerOffset = 0,
					.imageLayerCount = 1,
				};

				ImageMemoryBarrier imageMemoryBarrier;

				imageIndex = 0;

				while (imageIndex < IMAGE_COUNT) {
					const ref<Image> image = images[imageIndex];

					imageMemoryBarrier = {
						.image = image,
						.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
						.newAccess = Access::TRANSFER_WRITE,
					};

					commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

					bufferImageCopyRegion.imageExtent = image->getExtent();

					commandEncoder->copyBufferToImage(stagingBuffer, image, bufferImageCopyRegion);

					imageMemoryBarrier = {
						.image = image,
						.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.newAccess = Access::SHADER_READ,
					};

					commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

					bufferImageCopyRegion.bufferOffset += image->getSize();
					imageIndex += 1;
				}

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
					.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}},
				});
				commandEncoder->bindGraphicsPipeline(arrayTexturePipeline);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindDescriptorSets(arrayTexturePipelineLayout, 0, arrayTextureDescriptorSet);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->bindVertexBuffer(1, vertexBuffer, 6 * sizeof(Vertex));
				commandEncoder->draw(6, 4, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				createFramebuffers();
			}

		private:
			ref<RenderPass> createRenderPass() const {
				return Renderer::createRenderPass({
					.attachments = {
						// Swapchain
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

			void createFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor = {
					.renderPass = renderPass,
					.imageViews = {nullptr},
					.extent = swapchain->getExtent(),
				};

				#if defined(DEBUG_OBJECT_NAME)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

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

			ref<Sampler> createSampler() const {
				return Renderer::createSampler({
					.magFilter = Filter::LINEAR,
					.minFilter = Filter::NEAREST,
					.addressModeU = AddressMode::REPEAT,
					.addressModeV = AddressMode::REPEAT,
					.addressModeW = AddressMode::REPEAT,
				});
			}

			ref<DescriptorSetLayout> createArrayTextureDescriptorSetLayout() const {
				return createDescriptorSetLayout({
					// Array texture
					{
						.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
						.descriptorCount = IMAGE_COUNT,
						.shaderStages = ShaderStage::FRAGMENT,
					},
				});
			}

			ref<PipelineLayout> createArrayTexturePipelineLayout() const {
				return createPipelineLayout({
					.descriptorSetLayouts = {arrayTextureDescriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Array texture",
					#endif
				});
			}

			ref<GraphicsPipeline> createArrayTexturePipeline() const {
				VertexInputBinding vertexInputBindings[2] = {
					VertexInputBinding(InputRate::VERTEX),
					VertexInputBinding(InputRate::INSTANCE),
				};

				vertexInputBindings[0].registerAttribute<svec2<float32>>();

				vertexInputBindings[1].registerAttribute<svec2<float32>>();
				vertexInputBindings[1].registerAttribute<uint32>();

				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = false,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = arrayTexturePipelineLayout,
					.vertexInputBindings = view<VertexInputBinding>(vertexInputBindings, 2),
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Array texture vertex",
							#endif
						},
						"Sample/ArrayTexture/ArrayTexture.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Array texture vertex",
							#endif
						},
						"Sample/ArrayTexture/ArrayTexture.frag"
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
						.name = "Array texture",
					#endif
				});
			}

			ref<Buffer> createVertexBuffer() const {
				return createStaticBuffer({
					.size = 6 * sizeof(Vertex) + 4 * sizeof(Instance),
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

			ref<Image> createImage(ref<ImageAsset> imageAsset, uint32 index) const {
				#if defined(DEBUG_OBJECT_NAME)
					const sequence<char8> name = Logger::format("Array texture []", index);
				#endif

				return Renderer::createStaticImage({
					.type = ImageType::TEXTURE_2D,
					.format = ImageFormat::R8G8B8A8_UNORM,
					.extent = imageAsset->getExtent(),
					.mipLevelCount = 1,
					.layerCount = 1,
					.sampleCount = 1,
					.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
					#if defined(DEBUG_OBJECT_NAME)
						.name = name,
					#endif
				});
			}

			ref<ImageView> createImageView(ref<Image> image, uint32 index) {
				#if defined(DEBUG_OBJECT_NAME)
					const sequence<char8> name = Logger::format("Array texture view []", index);
				#endif

				return image->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = name,
					#endif
				});
			}

			ref<Buffer> createStagingBuffer() const {
				uint64 size = vertexBuffer->getSize();
				uint32 imageIndex = 0;

				while (imageIndex < IMAGE_COUNT) {
					const ref<Image> image = images[imageIndex];

					size += image->getSize();

					imageIndex += 1;
				}

				return createStaticBuffer({
					.size = size,
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
			}

			void updateArrayTextureDescriptorSet() {
				DescriptorImage descriptorImages[IMAGE_COUNT];
				uint32 imageIndex = 0;

				while (imageIndex < IMAGE_COUNT) {
					descriptorImages[imageIndex] = {
						.imageView = imageViews[imageIndex],
						.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.sampler = sampler,
					};

					imageIndex += 1;
				}

				const WriteDescriptorSet arrayTextureWriteDescriptorSet = {
					.descriptorSet = arrayTextureDescriptorSet,
					.bindingIndex = 0,
					.descriptorImages = view<DescriptorImage>(&descriptorImages[0], IMAGE_COUNT),
				};

				getDevice()->updateDescriptorSets(arrayTextureWriteDescriptorSet);
			}

		private:
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<DescriptorSetLayout> arrayTextureDescriptorSetLayout;
			ref<PipelineLayout> arrayTexturePipelineLayout;
			ref<GraphicsPipeline> arrayTexturePipeline;
			ref<DescriptorSet> arrayTextureDescriptorSet;
			ref<Buffer> vertexBuffer;
			ref<Image> images[IMAGE_COUNT];
			ref<ImageView> imageViews[IMAGE_COUNT];
			ref<Buffer> stagingBuffer;
			ref<Sampler> sampler;
	};

	DEFINE_SAMPLE_LAYER(SampleArrayTexture);
}