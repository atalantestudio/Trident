// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class SampleMipLevelsRenderer : public Renderer {
		private:
			struct MipLevelArguments {
				/// Total number of thread groups.
				uint32 groupCount;

				/// Number of mip levels to generate.
				uint32 mipLevelCount;
			};

		public:
			void build() override {
				loadImage();

				const uint32 mipLevelCount = ((Layer*) getLayer())->getApplication()->getPropertyFile()->getProperty<uint32>("MipLevelCount");

				generateMipLevels(image, mipLevelCount);

				renderPass = createRenderPass();

				createFramebuffers();

				debugImageView = createDebugImageView();

				debugSampler = createDebugSampler();

				mipLevelBuffer = createMipLevelBuffer();

				const ref<BufferView<uint32>> mipLevelView = mipLevelBuffer->createView<uint32>(0, mipLevelBuffer->getSize());

				mipLevelMappedRange = mipLevelView->map();
				mipLevelMappedRange[0] = 0;

				debugImageDescriptorSetLayout = createImageDescriptorSetLayout();

				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(debugImageDescriptorSetLayout);

				debugImageDescriptorSet = descriptorSets[0];

				updateDebugImageDescriptorSet();

				debugImagePipelineLayout = createDebugImagePipelineLayout();

				debugImagePipeline = createDebugImagePipeline();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffers[getSwapchainImageIndex()],
					.clearValues = {{1.0f, 1.0f, 1.0f, 1.0f}},
				});
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->bindGraphicsPipeline(debugImagePipeline);
				commandEncoder->bindDescriptorSets(debugImagePipelineLayout, 0, debugImageDescriptorSet);
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void onEvent(Event& event) override {
				EventDispatcher eventDispatcher(event);
				eventDispatcher.dispatch<KeyDownEvent>(BIND(onKeyDown));

				Renderer::onEvent(event);
			}

			bool onKeyDown(KeyDownEvent& event) {
				const KeyCode keyCode = event.getKeyCode();

				if (keyCode >= KeyCode::DIGIT_0 && keyCode <= KeyCode::DIGIT_9) {
					*mipLevelMappedRange = static_cast<uint32>(keyCode) - static_cast<uint32>(KeyCode::DIGIT_0);
				}
				else if (keyCode == KeyCode::A) {
					*mipLevelMappedRange = 10;
				}
				else if (keyCode == KeyCode::Z) {
					*mipLevelMappedRange = 11;
				}
				else if (keyCode == KeyCode::E) {
					*mipLevelMappedRange = 12;
				}

				return true;
			}

			void resize(WindowResizeEvent& event) override {
				createFramebuffers();
			}

		private:
			void loadImage() {
				const view<char8> assetName = ((Layer*) getLayer())->getApplication()->getPropertyFile()->getProperty<view<char8>>("AssetName");
				const ref<ImageAsset> imageAsset = getLayer()->getApplication()->getAssetLibrary()->getAsset<ImageAsset>(assetName);
				const Extent3D imageExtent = imageAsset->getExtent();
				const ref<Buffer> imageStagingBuffer = createDynamicBuffer({
					.size = imageExtent.width * imageExtent.height * imageExtent.depth * imageAsset->getLayerCount() * getChannelCount(ImageFormat::R8G8B8A8_UNORM),
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image staging",
					#endif
				});
				const ref<BufferView<uint8>> imageStagingView = imageStagingBuffer->createView<uint8>(0, imageStagingBuffer->getSize());

				uint8* imageStagingBufferMappedRange = imageStagingView->map();

				switch (imageAsset->getFormat()) {
					case ImageFormat::R8_UNORM: {
						convert<ImageFormat::R8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &imageStagingBufferMappedRange[0], imageStagingBuffer->getSize());

						break;
					}
					case ImageFormat::R8G8B8_UNORM: {
						convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &imageStagingBufferMappedRange[0], imageStagingBuffer->getSize());

						break;
					}
					case ImageFormat::R8G8B8A8_UNORM: {
						std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), imageStagingBufferMappedRange);

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				imageStagingBuffer->unmap();

				image = createDynamicImage({
					.type = ImageType::TEXTURE_2D,
					.format = ImageFormat::R8G8B8A8_UNORM,
					.extent = imageAsset->getExtent(),
					.mipLevelCount = calculateMaxMipLevelCount(imageAsset->getExtent()),
					.layerCount = 1,
					.sampleCount = 1,
					.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED | ImageUsage::STORAGE,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Input",
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

				const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image copy command buffer",
					#endif
				});

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

			void generateMipLevels(ref<Image> image, uint32 mipLevelCount) const {
				ENGINE_ASSERT(mipLevelCount > 0, "The number of mip levels to generate must be positive.");
				ENGINE_ASSERT(mipLevelCount < image->getMipLevelCount(), "The number of mip levels to generate must be less than the maximum number of mip levels in the image ([]).", image->getMipLevelCount());

				const Extent3D extent = image->getExtent();
				const uint32 groupCountX = (extent.width + 0b111111) >> 6;
				const uint32 groupCountY = (extent.height + 0b111111) >> 6;
				const uint32 groupCountZ = 1;

				sequence<ref<ImageView>> imageViews(mipLevelCount + 1);
				uint32 mipLevelIndex = 0;

				#if defined(DEBUG_OBJECT_NAME)
					sequence<char8> name;
				#endif

				while (mipLevelIndex < mipLevelCount + 1) {
					#if defined(DEBUG_OBJECT_NAME)
						name = Logger::format("Mip level []", mipLevelIndex);
					#endif

					imageViews[mipLevelIndex] = image->createView({
						.mipLevelOffset = mipLevelIndex,
						.mipLevelCount = 1,
						.layerOffset = 0,
						.layerCount = 1,
						#if defined(DEBUG_OBJECT_NAME)
							.name = name,
						#endif
					});

					mipLevelIndex += 1;
				}

				const uint64 minStorageBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
				const uint64 minUniformBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

				const ref<Buffer> atomicCounterBuffer = createDynamicBuffer({
					.size = ceil(sizeof(uint32), minStorageBufferOffsetAlignment),
					.usage = BufferUsage::READ_WRITE_STORAGE,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Atomic counter",
					#endif
				});
				const ref<Buffer> argumentBuffer = createDynamicBuffer({
					.size = ceil(sizeof(MipLevelArguments), minUniformBufferOffsetAlignment),
					.usage = BufferUsage::UNIFORM,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Mip level arguments",
					#endif
				});

				const ref<BufferView<uint32>> atomicCounterView = atomicCounterBuffer->createView<uint32>(0, atomicCounterBuffer->getSize());
				const ref<BufferView<MipLevelArguments>> argumentView = argumentBuffer->createView<MipLevelArguments>(0, argumentBuffer->getSize());

				MipLevelArguments* arguments = argumentView->map();
				arguments[0].groupCount = groupCountX * groupCountY * groupCountZ;
				arguments[0].mipLevelCount = mipLevelCount;

				argumentBuffer->unmap();

				const sequence<DescriptorSetLayoutBinding> bindings = {
					// Arguments
					{
						.descriptorType = DescriptorType::UNIFORM_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::COMPUTE,
					},

					// Source
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::COMPUTE,
					},

					// Destination
					{
						.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
						.descriptorCount = 12,
						.shaderStages = ShaderStage::COMPUTE,
					},

					// Atomic counter
					{
						.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::COMPUTE,
					},
				};

				const ref<DescriptorSetLayout> descriptorSetLayout = createDescriptorSetLayout(bindings);
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayout);
				const ref<DescriptorSet> descriptorSet = descriptorSets[0];

				sequence<DescriptorImage> descriptorImages(13);

				mipLevelIndex = 0;

				while (mipLevelIndex < mipLevelCount + 1) {
					descriptorImages[mipLevelIndex] = {
						.imageView = imageViews[mipLevelIndex],
						.imageLayout = ImageLayout::GENERAL,
						.sampler = nullptr,
					};

					mipLevelIndex += 1;
				}

				while (mipLevelIndex < 13) {
					descriptorImages[mipLevelIndex] = {
						.imageView = imageViews[0],
						.imageLayout = ImageLayout::GENERAL,
						.sampler = nullptr,
					};

					mipLevelIndex += 1;
				}

				descriptorImages[0].imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

				const sequence<WriteDescriptorSet> writeDescriptorSets = {
					// Arguments
					{
						.descriptorSet = descriptorSet,
						.bindingIndex = 0,
						.descriptorBufferViews = argumentView,
					},

					// Source
					{
						.descriptorSet = descriptorSet,
						.bindingIndex = 1,
						.descriptorImages = descriptorImages[0],
					},

					// Destination
					{
						.descriptorSet = descriptorSet,
						.bindingIndex = 2,
						.descriptorImages = view<DescriptorImage>(&descriptorImages[1], 12),
					},

					// Atomic counter
					{
						.descriptorSet = descriptorSet,
						.bindingIndex = 3,
						.descriptorBufferViews = atomicCounterView,
					},
				};

				getDevice()->updateDescriptorSets(writeDescriptorSets);

				const ref<PipelineLayout> pipelineLayout = createPipelineLayout({
					.descriptorSetLayouts = {descriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Generate mip levels",
					#endif
				});

				const ref<ComputePipeline> computePipeline = createComputePipeline({
					.pipelineLayout = pipelineLayout,
					.computeShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::COMPUTE,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Generate mip levels compute",
							#endif
						},
						"Sample/MipLevels/GenerateMipLevels.comp"
					),
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Generate mip levels",
					#endif
				});

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::COMPUTE);

				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Generate mip levels",
					#endif
				});

				commandEncoder->begin();
				commandEncoder->bindComputePipeline(computePipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, descriptorSet);
				commandEncoder->dispatch(groupCountX, groupCountY, groupCountZ);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

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
						name = Logger::format("In flight framebuffer []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			ref<ImageView> createDebugImageView() const {
				return image->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = image->getMipLevelCount(),
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Debug",
					#endif
				});
			}

			ref<Sampler> createDebugSampler() const {
				return createSampler({
					.magFilter = Filter::NEAREST,
					.minFilter = Filter::NEAREST,
				});
			}

			ref<Buffer> createMipLevelBuffer() const {
				const uint64 minUniformBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

				return createDynamicBuffer({
					.size = ceil(sizeof(uint32), minUniformBufferOffsetAlignment),
					.usage = BufferUsage::UNIFORM,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
					.persistent = true,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Mip level",
					#endif
				});
			}

			ref<DescriptorSetLayout> createImageDescriptorSetLayout() const {
				const sequence<DescriptorSetLayoutBinding> bindings = {
					// Mip level
					{
						.descriptorType = DescriptorType::UNIFORM_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::FRAGMENT,
					},

					// Image
					{
						.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::FRAGMENT,
					},
				};

				return createDescriptorSetLayout(bindings);
			}

			void updateDebugImageDescriptorSet() {
				const ref<BufferView<uint32>> mipLevelView = mipLevelBuffer->createView<uint32>(0, mipLevelBuffer->getSize());

				const DescriptorImage debugDescriptorImage = {
					.imageView = debugImageView,
					.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.sampler = debugSampler,
				};

				const sequence<WriteDescriptorSet> writeDescriptorSets = {
					{
						.descriptorSet = debugImageDescriptorSet,
						.bindingIndex = 0,
						.descriptorBufferViews = mipLevelView,
					},
					{
						.descriptorSet = debugImageDescriptorSet,
						.bindingIndex = 1,
						.descriptorImages = debugDescriptorImage,
					},
				};

				getDevice()->updateDescriptorSets(writeDescriptorSets);
			}

			ref<PipelineLayout> createDebugImagePipelineLayout() const {
				return createPipelineLayout({
					.descriptorSetLayouts = {debugImageDescriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Debug image",
					#endif
				});
			}

			ref<GraphicsPipeline> createDebugImagePipeline() const {
				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = false,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = debugImagePipelineLayout,
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Image vertex",
							#endif
						},
						"Sample/Image/Image.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Image fragment",
							#endif
						},
						"Sample/MipLevels/DebugImage.frag"
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
						.name = "Debug image",
					#endif
				});
			}

		private:
			ref<Image> image;

			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<DescriptorSetLayout> debugImageDescriptorSetLayout;
			ref<DescriptorSet> debugImageDescriptorSet;
			ref<PipelineLayout> debugImagePipelineLayout;
			ref<GraphicsPipeline> debugImagePipeline;
			ref<Buffer> mipLevelBuffer;
			ref<ImageView> debugImageView;
			ref<Sampler> debugSampler;

			uint32* mipLevelMappedRange = nullptr;
	};

	DEFINE_SAMPLE_LAYER(SampleMipLevels);
}