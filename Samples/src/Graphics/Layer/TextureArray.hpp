// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class SampleTextureArrayRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> position;
			};

			struct Instance {
				svec2<float32> vertexOffset;
				uint32 textureArrayIndex;
			};

		private:
			static constexpr uint32 ASSET_COUNT = 4;
			static constexpr uint32 TEXTURE_ARRAY_IMAGE_WIDTH = 512;
			static constexpr uint32 TEXTURE_ARRAY_IMAGE_HEIGHT = 512;
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
				createTextureArrayImage();
				createTextureArrayImageView();
				loadAssets();
				setupSampler();
				createTextureArrayImageDescriptorSetLayout();
				allocateTextureArrayImageDescriptorSet();
				updateTextureArrayImageDescriptorSet();
				setupPipelineLayout();
				setupGraphicsPipeline();

				vertexBuffer = createVertexBuffer();
				instanceBuffer = createInstanceBuffer();
			}

			void encodeOneTimeCommands() const override {
				const ref<Buffer> stagingBuffer = createDynamicBuffer({
					.size = vertexBuffer->getSize() + instanceBuffer->getSize(),
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
				const BufferView<Vertex> vertexView = stagingBuffer->createView<Vertex>(0, vertexBuffer->getSize());
				const BufferView<Instance> instanceView = stagingBuffer->createView<Instance>(vertexBuffer->getSize(), instanceBuffer->getSize());

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

				stagingBuffer->unmap();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Copy buffers",
					#endif
				});

				commandEncoder->begin();

				BufferCopyRegion bufferCopyRegion = {
					.sourceOffset = vertexView.getOffset(),
					.destinationOffset = 0,
					.size = vertexView.getSize(),
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegion);

				bufferCopyRegion = {
					.sourceOffset = instanceView.getOffset(),
					.destinationOffset = 0,
					.size = instanceView.getSize(),
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, bufferCopyRegion);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D extent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(extent.width);
				const float32 height = static_cast<float32>(extent.height);

				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindGraphicsPipeline(graphicsPipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, textureArrayDescriptorSet);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->bindVertexBuffer(1, instanceBuffer, 0);
				commandEncoder->draw(6, 4, 0, 0);
			}

		private:
			void createTextureArrayImage() {
				ImageDescriptor textureArrayImageDescriptor{};
				textureArrayImageDescriptor.type = ImageType::TEXTURE_2D;
				textureArrayImageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
				textureArrayImageDescriptor.extent.width = TEXTURE_ARRAY_IMAGE_WIDTH;
				textureArrayImageDescriptor.extent.height = TEXTURE_ARRAY_IMAGE_HEIGHT;
				textureArrayImageDescriptor.extent.depth = 1;
				textureArrayImageDescriptor.mipLevelCount = 1;
				textureArrayImageDescriptor.layerCount = 4;
				textureArrayImageDescriptor.sampleCount = 1;
				textureArrayImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
				textureArrayImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

				#if defined(DEBUG_OBJECT_NAME)
					textureArrayImageDescriptor.name = "Texture array";
				#endif

				textureArrayImage = createDynamicImage(textureArrayImageDescriptor);
			}

			void createTextureArrayImageView() {
				ImageViewDescriptor textureArrayImageViewDescriptor{};
				textureArrayImageViewDescriptor.mipLevelOffset = 0;
				textureArrayImageViewDescriptor.mipLevelCount = 1;
				textureArrayImageViewDescriptor.layerOffset = 0;
				textureArrayImageViewDescriptor.layerCount = textureArrayImage->getLayerCount();

				#if defined(DEBUG_OBJECT_NAME)
					textureArrayImageViewDescriptor.name = "Texture array";
				#endif

				textureArrayImageView = textureArrayImage->createView(textureArrayImageViewDescriptor);
			}

			void loadAssets() {
				sequence<ref<ImageAsset>> imageAssets(ASSET_COUNT);
				AssetLibrary* assetLibrary = getLayer()->getApplication()->getAssetLibrary();
				uint64 textureArrayImageSize = 0;
				uint32 assetIndex = 0;

				while (assetIndex < ASSET_COUNT) {
					const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(IMAGE_ASSET_PATHS[assetIndex]);

					textureArrayImageSize += imageAsset->getData().count();

					imageAssets[assetIndex] = imageAsset;

					assetIndex += 1;
				}

				const ref<Buffer> textureArrayImageStagingBuffer = createDynamicBuffer({
					.size = textureArrayImageSize,
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Texture array staging",
					#endif
				});
				const BufferView<uint8> textureArrayImageStagingView = textureArrayImageStagingBuffer->createView<uint8>(0, textureArrayImageStagingBuffer->getSize());

				uint8* textureArrayImageStagingBufferMappedRange = textureArrayImageStagingView.map();

				assetIndex = 0;

				while (assetIndex < ASSET_COUNT) {
					const ref<ImageAsset> imageAsset = imageAssets[assetIndex];

					std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), textureArrayImageStagingBufferMappedRange);

					textureArrayImageStagingBufferMappedRange += imageAsset->getData().count();

					assetIndex += 1;
				}

				textureArrayImageStagingBuffer->unmap();

				BufferImageCopyRegion bufferImageCopyRegion{};
				bufferImageCopyRegion.bufferOffset = 0;
				bufferImageCopyRegion.imageOffsetX = 0;
				bufferImageCopyRegion.imageOffsetY = 0;
				bufferImageCopyRegion.imageOffsetZ = 0;
				bufferImageCopyRegion.imageExtent = textureArrayImage->getExtent();
				bufferImageCopyRegion.imageMipLevel = 0;
				bufferImageCopyRegion.imageLayerOffset = 0;
				bufferImageCopyRegion.imageLayerCount = static_cast<uint32>(imageAssets.count());

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				#if defined(DEBUG_OBJECT_NAME)
					commandBufferDescriptor.name = "Image copy command buffer";
				#endif

				const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();

				ImageMemoryBarrier textureArrayImageMemoryBarrier = {
					.image = textureArrayImage,
					.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
					.newAccess = Access::TRANSFER_WRITE,
				};

				commandEncoder->insertMemoryBarriers({}, {textureArrayImageMemoryBarrier});
				commandEncoder->copyBufferToImage(textureArrayImageStagingBuffer, textureArrayImage, {bufferImageCopyRegion});

				textureArrayImageMemoryBarrier = {
					.image = textureArrayImage,
					.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({}, {textureArrayImageMemoryBarrier});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				graphicsQueue->submit(commandBuffer);
				graphicsQueue->waitIdle();
			}

			void setupSampler() {
				sampler = createSampler({
					.magFilter = Filter::LINEAR,
					.minFilter = Filter::NEAREST,
					.addressModeU = AddressMode::REPEAT,
					.addressModeV = AddressMode::REPEAT,
					.addressModeW = AddressMode::REPEAT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Sampler",
					#endif
				});
			}

			void createTextureArrayImageDescriptorSetLayout() {
				const DescriptorSetLayoutBinding bindings[] = {
					{
						.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
						.descriptorCount = 1,
						.stageFlags = ShaderStage::FRAGMENT,
					},
				};

				textureArrayDescriptorSetLayout = createDescriptorSetLayout({
					.bindings = bindings,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Texture array",
					#endif
				});
			}

			void allocateTextureArrayImageDescriptorSet() {
				// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(textureArrayDescriptorSetLayout);

				textureArrayDescriptorSet = descriptorSets[0];
			}

			void updateTextureArrayImageDescriptorSet() {
				DescriptorImage textureArrayDescriptorImage{};
				textureArrayDescriptorImage.imageView = textureArrayImageView;
				textureArrayDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				textureArrayDescriptorImage.sampler = sampler;

				WriteDescriptorSet textureArrayWriteDescriptorSet{};
				textureArrayWriteDescriptorSet.descriptorSet = textureArrayDescriptorSet;
				textureArrayWriteDescriptorSet.bindingIndex = 0;
				textureArrayWriteDescriptorSet.descriptorImages = {textureArrayDescriptorImage};

				getDevice()->updateDescriptorSets({textureArrayWriteDescriptorSet});
			}

			void setupPipelineLayout() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};
				pipelineLayoutDescriptor.descriptorSetLayouts = {textureArrayDescriptorSetLayout};

				#if defined(DEBUG_OBJECT_NAME)
					pipelineLayoutDescriptor.name = "Pipeline layout";
				#endif

				pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
			}

			void setupGraphicsPipeline() {
				VertexInputBinding vertexInputBindings[2] = {
					VertexInputBinding(InputRate::VERTEX),
					VertexInputBinding(InputRate::INSTANCE),
				};

				vertexInputBindings[0].registerAttribute<svec2<float32>>();

				vertexInputBindings[1].registerAttribute<svec2<float32>>();
				vertexInputBindings[1].registerAttribute<uint32>();

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

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/TextureArray/TextureArray.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/TextureArray/TextureArray.frag");

				const ColorBlendAttachment colorBlendAttachments[] = {
					{
						.blend = false,
					},
				};

				GraphicsPipelineDescriptor graphicsPipelineDescriptor{};
				graphicsPipelineDescriptor.pipelineLayout = pipelineLayout;
				graphicsPipelineDescriptor.vertexInputBindings = vertexInputBindings;
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
				graphicsPipelineDescriptor.renderPass = pointer_cast<const RenderPassLayer*>(getLayer())->renderPass;
				graphicsPipelineDescriptor.subpassIndex = 0;

				#if defined(DEBUG_OBJECT_NAME)
					graphicsPipelineDescriptor.name = "Render pipeline";
				#endif

				graphicsPipeline = createGraphicsPipeline(graphicsPipelineDescriptor);
			}

			ref<Buffer> createVertexBuffer() const {
				return createDynamicBuffer({
					.size = VERTEX_BUFFER_SIZE,
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

			ref<Buffer> createInstanceBuffer() const {
				return createDynamicBuffer({
					.size = INSTANCE_BUFFER_SIZE,
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Instance",
					#endif
				});
			}

		private:
			ref<Image> textureArrayImage;
			ref<ImageView> textureArrayImageView;
			ref<Sampler> sampler;
			ref<DescriptorSetLayout> textureArrayDescriptorSetLayout;
			ref<DescriptorSet> textureArrayDescriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> graphicsPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> instanceBuffer;
	};

	DEFINE_SAMPLE_LAYER(SampleTextureArray);
}