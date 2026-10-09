// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class ImageSampleRenderer : public Renderer {
		public:
			void build() override {
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
				imageView = createImageView(0);

				updateDescriptorSets();
			}

			void encodeOneTimeCommands() const override {
				const BufferView<uint8> imageView = stagingBuffer->createView<uint8>(0, stagingBuffer->getSize());
				const view<uint8> data = imageAsset->getData();

				if (imageAsset->getFormat() == ImageFormat::R8G8B8_UNORM) {
					convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(data, imageView.map(), imageView.getSize());
				}
				else {
					std::copy(data.begin(), data.end(), imageView.map());
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
					.imageOffset = {0, 0, 0},
					.imageExtent = image->getExtent(),
					.imageMipLevel = 0,
					.imageLayerOffset = 0,
					.imageLayerCount = image->getLayerCount(),
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
				const Extent2D extent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(extent.x);
				const float32 height = static_cast<float32>(extent.y);

				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindGraphicsPipeline(pipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, descriptorSet);
				commandEncoder->draw(3, 1, 0, 0);
			}

			void onEvent(Event& event) override {
				EventDispatcher eventDispatcher(event);
				eventDispatcher.dispatch<KeyDownEvent>(BIND(onKeyDown));

				Renderer::onEvent(event);
			}

		private:
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
					.type = ImageType::TYPE_2D,
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

			ref<ImageView> createImageView(uint32 layerOffset) const {
				return image->createView({
					.type = ImageViewType::TYPE_2D,
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = layerOffset,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image view",
					#endif
				});
			}

			ref<DescriptorSetLayout> createDescriptorSetLayout() const {
				const DescriptorSetLayoutBinding bindings[] = {
					{
						.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
						.descriptorCount = 1,
						.stageFlags = ShaderStage::FRAGMENT,
					},
				};

				return Renderer::createDescriptorSetLayout({
					.bindings = bindings,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Descriptor set layout",
					#endif
				});
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
					.renderPass = pointer_cast<const RenderPassLayer*>(getLayer())->renderPass,
					.subpassIndex = 0,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Pipeline",
					#endif
				});
			}

			ref<Buffer> createStagingBuffer() const {
				const Extent3D extent = imageAsset->getExtent();

				return createStaticBuffer({
					.size = getByteSize(imageFormat) * extent.x * extent.y * extent.z * imageAsset->getLayerCount(),
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
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Sampler",
					#endif
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

			bool onKeyDown(KeyDownEvent& event) {
				const KeyCode keyCode = event.getKeyCode();

				uint32 layerOffset = 0;

				if (keyCode >= KeyCode::DIGIT_0 && keyCode <= KeyCode::DIGIT_9) {
					layerOffset = static_cast<uint32>(keyCode) - static_cast<uint32>(KeyCode::DIGIT_0);
				}
				else if (keyCode == KeyCode::A) {
					layerOffset = 10;
				}
				else if (keyCode == KeyCode::Z) {
					layerOffset = 11;
				}
				else if (keyCode == KeyCode::E) {
					layerOffset = 12;
				}
				else {
					return true;
				}

				if (layerOffset >= image->getLayerCount()) {
					return true;
				}

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);

				queue->waitIdle();

				imageView = createImageView(layerOffset);

				updateDescriptorSets();

				return true;
			}

		private:
			ref<ImageAsset> imageAsset;
			ImageFormat imageFormat;
			ref<Image> image;
			ref<ImageView> imageView;
			ref<DescriptorSetLayout> descriptorSetLayout;
			ref<DescriptorSet> descriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> pipeline;
			ref<Buffer> stagingBuffer;
			ref<Sampler> sampler;
	};

	DEFINE_SAMPLE_LAYER(ImageSample);
}