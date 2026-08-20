// Copyright 2026 Atalante Studio.
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
				svec2<float32> translation;
				uint32 textureIndex;
			};

		private:
			static constexpr uint32 IMAGE_COUNT = 4;
			static constexpr view<char8> ASSET_PATHS[IMAGE_COUNT] = {
				"Baboon",
				"Cameraman",
				"Lenna",
				"Pepper",
			};

			static constexpr uint64 VERTEX_SIZE = sizeof(Vertex) * 6;
			static constexpr uint64 INSTANCE_SIZE = sizeof(Instance) * 4;

		public:
			SampleArrayTextureRenderer() {}

			~SampleArrayTextureRenderer() {
				vertexBuffer.~ref();
				stagingBuffer.~ref();
			}

			void build() override {
				renderPass = createRenderPass();
				descriptorSetLayout = createDescriptorSetLayout();

				// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayout);

				descriptorSet = descriptorSets[0];
				pipelineLayout = createPipelineLayout();
				pipeline = createPipeline();
				vertexBuffer = createVertexBuffer();

				AssetLibrary* assetLibrary = getLayer()->getApplication()->getAssetLibrary();
				uint32 imageIndex = 0;
				uint64 stagingSize = 0;

				while (imageIndex < IMAGE_COUNT) {
					const view<char8> assetPath = ASSET_PATHS[imageIndex];
					const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(assetPath);

					imageAssets[imageIndex] = imageAsset;
					images[imageIndex] = createImage(imageAsset, imageIndex);

					stagingSize += imageAsset->getSize();
					imageIndex += 1;
				}

				stagingBuffer = createStagingBuffer(stagingSize);
				sampler = createSampler();

				createFramebuffers();
			}

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override {
				return {
					.buffers = staticBuffers,
					.images = images,
				};
			};

			void postBuild() override {
				uint32 imageIndex = 0;

				while (imageIndex < IMAGE_COUNT) {
					imageViews[imageIndex] = createImageView(images[imageIndex], imageIndex);

					imageIndex += 1;
				}

				updateDescriptorSets();
			}

			void encodeOneTimeCommands() const override {
				const BufferView<Vertex> vertexView = stagingBuffer->createView<Vertex>(0, VERTEX_SIZE);
				const BufferView<Instance> instanceView = stagingBuffer->createView<Instance>(vertexView.getOffset() + vertexView.getSize(), INSTANCE_SIZE);
				const BufferView<uint8> imageView = stagingBuffer->createView<uint8>(vertexBuffer->getSize(), stagingBuffer->getSize() - vertexBuffer->getSize());

				Vertex* vertexMappedRange = vertexView.map();
				vertexMappedRange[0] = {{-0.5f,  0.5f}};
				vertexMappedRange[1] = {{ 0.5f,  0.5f}};
				vertexMappedRange[2] = {{ 0.5f, -0.5f}};
				vertexMappedRange[3] = {{-0.5f,  0.5f}};
				vertexMappedRange[4] = {{ 0.5f, -0.5f}};
				vertexMappedRange[5] = {{-0.5f, -0.5f}};

				Instance* instanceMappedRange = instanceView.map();
				instanceMappedRange[0] = {{-0.5f,  0.5f}, 0};
				instanceMappedRange[1] = {{ 0.5f,  0.5f}, 1};
				instanceMappedRange[2] = {{-0.5f, -0.5f}, 2};
				instanceMappedRange[3] = {{ 0.5f, -0.5f}, 3};

				ImageMemoryBarrier imageMemoryBarriers[IMAGE_COUNT];
				uint8* imageMappedRange = imageView.map();
				uint32 imageIndex = 0;

				while (imageIndex < IMAGE_COUNT) {
					const ref<ImageAsset> imageAsset = imageAssets[imageIndex];
					const view<uint8> data = imageAsset->getData();

					std::copy(data.begin(), data.end(), imageMappedRange);

					imageMemoryBarriers[imageIndex] = {
						.image = images[imageIndex],
						.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
						.newAccess = Access::TRANSFER_WRITE,
					};

					imageMappedRange += imageAsset->getSize();
					imageIndex += 1;
				}

				stagingBuffer->unmap();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "One-time commands",
					#endif
				});

				commandEncoder->begin();

				const BufferCopyRegion bufferCopyRegion = {
					.sourceOffset = vertexView.getOffset(),
					.destinationOffset = 0,
					.size = vertexView.getSize() + instanceView.getSize(),
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegion);
				commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

				BufferImageCopyRegion bufferImageCopyRegion = {
					.bufferOffset = imageView.getOffset(),
					.imageOffsetX = 0,
					.imageOffsetY = 0,
					.imageOffsetZ = 0,
					.imageMipLevel = 0,
					.imageLayerOffset = 0,
					.imageLayerCount = 1,
				};

				imageIndex = 0;

				while (imageIndex < IMAGE_COUNT) {
					const ref<Image> image = images[imageIndex];

					bufferImageCopyRegion.imageExtent = image->getExtent();

					commandEncoder->copyBufferToImage(stagingBuffer, image, bufferImageCopyRegion);

					bufferImageCopyRegion.bufferOffset += image->getSize();

					imageMemoryBarriers[imageIndex] = {
						.image = image,
						.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.newAccess = Access::SHADER_READ,
					};

					imageIndex += 1;
				}

				commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

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
				commandEncoder->bindGraphicsPipeline(pipeline);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, descriptorSet);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->bindVertexBuffer(1, vertexBuffer, VERTEX_SIZE);
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

			ref<DescriptorSetLayout> createDescriptorSetLayout() const {
				const DescriptorSetLayoutBinding bindings[] = {
					{
						.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
						.descriptorCount = IMAGE_COUNT,
						.stageFlags = ShaderStage::FRAGMENT,
					},
				};

				return Renderer::createDescriptorSetLayout(bindings);
			}

			ref<PipelineLayout> createPipelineLayout() const {
				return Renderer::createPipelineLayout({
					.descriptorSetLayouts = {descriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Pipeline layout",
					#endif
				});
			}

			ref<GraphicsPipeline> createPipeline() const {
				VertexInputBinding vertexInputBindings[2] = {
					VertexInputBinding(InputRate::VERTEX),
					VertexInputBinding(InputRate::INSTANCE),
				};

				vertexInputBindings[0].registerAttribute<svec2<float32>>();
				vertexInputBindings[1].registerAttribute<svec2<float32>>();
				vertexInputBindings[1].registerAttribute<uint32>();

				const ColorBlendAttachment colorBlendAttachments[] = {
					{
						.blend = false,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = pipelineLayout,
					.vertexInputBindings = vertexInputBindings,
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Vertex",
							#endif
						},
						"Sample/ArrayTexture/ArrayTexture.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Fragment",
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
						.name = "Pipeline",
					#endif
				});
			}

			ref<Buffer> createVertexBuffer() const {
				return createStaticBuffer({
					.size = VERTEX_SIZE + INSTANCE_SIZE,
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

			ref<Buffer> createStagingBuffer(uint64 stagingSize) const {
				return createStaticBuffer({
					.size = vertexBuffer->getSize() + stagingSize,
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
			}

			ref<Image> createImage(ref<ImageAsset> imageAsset, uint32 index) const {
				#if defined(DEBUG_OBJECT_NAME)
					const sequence<char8> name = Logger::format("Image []", index);
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

			ref<ImageView> createImageView(ref<Image> image, uint32 index) const {
				#if defined(DEBUG_OBJECT_NAME)
					const sequence<char8> name = Logger::format("Image []", index);
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

			ref<Sampler> createSampler() const {
				return Renderer::createSampler({
					.magFilter = Filter::LINEAR,
					.minFilter = Filter::NEAREST,
					.addressModeU = AddressMode::REPEAT,
					.addressModeV = AddressMode::REPEAT,
					.addressModeW = AddressMode::REPEAT,
				});
			}

			void updateDescriptorSets() {
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

				const WriteDescriptorSet writeDescriptorSets[] = {
					{
						.descriptorSet = descriptorSet,
						.bindingIndex = 0,
						.descriptorImages = descriptorImages,
					},
				};

				getDevice()->updateDescriptorSets(writeDescriptorSets);
			}

		private:
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<DescriptorSetLayout> descriptorSetLayout;
			ref<DescriptorSet> descriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> pipeline;

			union {
				struct {
					ref<Buffer> vertexBuffer;
					ref<Buffer> stagingBuffer;
				};

				ref<Buffer> staticBuffers[2];
			};

			ref<ImageAsset> imageAssets[IMAGE_COUNT];
			ref<Image> images[IMAGE_COUNT];
			ref<ImageView> imageViews[IMAGE_COUNT];
			ref<Sampler> sampler;
	};

	DEFINE_SAMPLE_LAYER(SampleArrayTexture);
}