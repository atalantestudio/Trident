// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class SampleTextureArrayRenderer : public Renderer {
		private:
			struct Vertex {
				vec2<float32> position;
			};

			struct Instance {
				vec2<float32> vertexOffset;
				uint32 textureArrayIndex;
			};

		private:
			static constexpr Extent3D IMAGE_EXTENT = {512, 512, 1};
			static constexpr uint32 IMAGE_COUNT = 4;
			static constexpr view<char8> ASSET_PATHS[IMAGE_COUNT] = {
				"Baboon",
				"Cameraman",
				"Lenna",
				"Pepper",
			};

			static constexpr uint64 VERTEX_SIZE = 6 * sizeof(Vertex);
			static constexpr uint64 INSTANCE_SIZE = 4 * sizeof(Instance);

		public:
			void build() override {
				descriptorSetLayout = createDescriptorSetLayout();

				// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
				sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayout);

				descriptorSet = descriptorSets[0];
				pipelineLayout = createPipelineLayout();
				pipeline = createPipeline();
				buffers[0] = createVertexBuffer();

				AssetLibrary* assetLibrary = getLayer()->getApplication()->getAssetLibrary();
				uint32 imageIndex = 0;
				uint64 imageStagingSize = 0;

				while (imageIndex < IMAGE_COUNT) {
					const view<char8> assetPath = ASSET_PATHS[imageIndex];
					const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(assetPath);

					imageAssets[imageIndex] = imageAsset;

					imageStagingSize += imageAsset->getSize();
					imageIndex += 1;
				}

				buffers[1] = createStagingBuffer(imageStagingSize);
				image = createImage();
				sampler = createSampler();
			}

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override {
				return {
					.buffers = buffers,
					.images = image,
				};
			}

			void postBuild() override {
				imageView = createImageView();

				updateDescriptorSets();
			}

			void encodeOneTimeCommands() const override {
				const BufferView<Vertex> vertexView = buffers[1]->createView<Vertex>(0, VERTEX_SIZE);
				const BufferView<Instance> instanceView = buffers[1]->createView<Instance>(vertexView.getOffset() + vertexView.getSize(), INSTANCE_SIZE);
				const BufferView<uint8> imageStagingView = buffers[1]->createView<uint8>(buffers[0]->getSize(), buffers[1]->getSize() - buffers[0]->getSize());

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

				uint8* imageMappedRange = imageStagingView.map();
				uint32 imageIndex = 0;

				while (imageIndex < IMAGE_COUNT) {
					const ref<ImageAsset> imageAsset = imageAssets[imageIndex];
					const view<uint8> data = imageAsset->getData();

					std::copy(data.begin(), data.end(), imageMappedRange);

					imageMappedRange += imageAsset->getSize();
					imageIndex += 1;
				}

				buffers[1]->unmap();

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

				commandEncoder->copyBufferToBuffer(buffers[1], buffers[0], bufferCopyRegion);

				ImageMemoryBarrier imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
					.newAccess = Access::TRANSFER_WRITE,
				};

				commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

				BufferImageCopyRegion bufferImageCopyRegion = {
					.bufferOffset = imageStagingView.getOffset(),
					.imageOffset = {0, 0, 0},
					.imageExtent = image->getExtent(),
					.imageMipLevel = imageView->getMipLevelOffset(),
					.imageLayerOffset = imageView->getLayerOffset(),
					.imageLayerCount = imageView->getLayerCount(),
				};

				commandEncoder->copyBufferToImage(buffers[1], image, bufferImageCopyRegion);

				imageMemoryBarrier.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				imageMemoryBarrier.newAccess = Access::SHADER_READ;

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
				commandEncoder->bindVertexBuffer(0, buffers[0], 0);
				commandEncoder->bindVertexBuffer(1, buffers[0], VERTEX_SIZE);
				commandEncoder->draw(6, 4, 0, 0);
			}

		private:
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
						.name = "Pipeline layout",
					#endif
				});
			}

			ref<GraphicsPipeline> createPipeline() const {
				VertexInputBinding vertexInputBindings[2] = {
					VertexInputBinding(InputRate::VERTEX),
					VertexInputBinding(InputRate::INSTANCE),
				};

				vertexInputBindings[0].registerAttribute<decltype(Vertex::position)>();
				vertexInputBindings[1].registerAttribute<decltype(Instance::vertexOffset)>();
				vertexInputBindings[1].registerAttribute<decltype(Instance::textureArrayIndex)>();

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
						"Sample/TextureArray/TextureArray.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Fragment",
							#endif
						},
						"Sample/TextureArray/TextureArray.frag"
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

			ref<Buffer> createStagingBuffer(uint64 imageStagingSize) const {
				return createStaticBuffer({
					.size = buffers[0]->getSize() + imageStagingSize,
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
			}

			ref<Image> createImage() const {
				return createStaticImage({
					.type = ImageType::TYPE_2D,
					.format = ImageFormat::R8G8B8A8_UNORM,
					.extent = IMAGE_EXTENT,
					.mipLevelCount = 1,
					.layerCount = IMAGE_COUNT,
					.sampleCount = 1,
					.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image",
					#endif
				});
			}

			ref<ImageView> createImageView() const {
				return image->createView({
					.type = ImageViewType::TYPE_2D,
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = image->getLayerCount(),
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Image view",
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
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Sampler",
					#endif
				});
			}

			void updateDescriptorSets() {
				const DescriptorImage descriptorImages[] = {
					{
						.imageView = imageView,
						.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.sampler = sampler,
					},
				};

				const WriteDescriptorSet writeDescriptorSets[] = {
					{
						.descriptorSet = descriptorSet,
						.bindingIndex = 0,
						.descriptorImages = descriptorImages[0],
					},
				};

				getDevice()->updateDescriptorSets(writeDescriptorSets);
			}

		private:
			ref<DescriptorSetLayout> descriptorSetLayout;
			ref<DescriptorSet> descriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> pipeline;
			ref<Buffer> buffers[2];
			ref<ImageAsset> imageAssets[IMAGE_COUNT];
			ref<Image> image;
			ref<ImageView> imageView;
			ref<Sampler> sampler;
	};

	DEFINE_SAMPLE_LAYER(SampleTextureArray);
}