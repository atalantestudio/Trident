#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextureArrayRenderer : public Renderer {
		public:
			TextureArrayRenderer() {
				const ref<Device> device = getDevice();

				std::array<PNGFile, ASSET_PATHS.size()> assets;
				uint64 size = 0;

				// 1. Load assets.
				{
					for (uint32 assetIndex = 0; assetIndex < assets.size(); assetIndex += 1) {
						const std::string_view path = ASSET_PATHS[assetIndex];

						assets[assetIndex] = Filesystem::openTemporary<PNGFile>(path);

						size += assets[assetIndex].data.count;
					}
				}

				ref<Buffer> textureArrayStagingBuffer;

				// 2. Create staging buffer to hold asset data.
				{
					textureArrayStagingBuffer = createStagingBuffer(size);
					uint8* textureArrayMappedRange = textureArrayStagingBuffer->map<uint8>(0, size);

					for (uint32 assetIndex = 0; assetIndex < assets.size(); assetIndex += 1) {
						const PNGFile& asset = assets[assetIndex];

						std::copy(asset.data.begin(), asset.data.end(), textureArrayMappedRange);

						textureArrayMappedRange += asset.data.count;
					}

					textureArrayStagingBuffer->unmap();
				}

				// 3. Create texture array.
				{
					ImageDescriptor textureArrayDescriptor;
					textureArrayDescriptor.type = ImageType::TEXTURE_2D;
					textureArrayDescriptor.format = ImageFormat::R8G8B8A8_SRGB;
					textureArrayDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
					textureArrayDescriptor.width = TEXTURE_ARRAY_WIDTH;
					textureArrayDescriptor.height = TEXTURE_ARRAY_HEIGHT;
					textureArrayDescriptor.depth = 1;
					textureArrayDescriptor.mipLevelCount = 1;
					textureArrayDescriptor.layerCount = 4;
					textureArrayDescriptor.sampleCount = 1;
					textureArrayDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

					textureArray = createImage(textureArrayDescriptor);
				}

				// 4. Copy asset data to the texture array.
				{
					CommandBufferDescriptor commandBufferDescriptor;
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->setImageMemoryBarrier(textureArray, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					sequence<BufferImageCopyRegion> copyRegions(1);

					BufferImageCopyRegion& copyRegion = copyRegions[0];
					copyRegion.bufferOffset = 0;
					copyRegion.imageOffsetX = 0;
					copyRegion.imageOffsetY = 0;
					copyRegion.imageOffsetZ = 0;
					copyRegion.imageExtent = textureArray->getExtent();
					copyRegion.imageMipLevel = 0;
					copyRegion.imageLayerOffset = 0;
					copyRegion.imageLayerCount = static_cast<uint32>(assets.size());

					commandEncoder->copyBufferToImage(textureArrayStagingBuffer, textureArray, copyRegions);
					commandEncoder->setImageMemoryBarrier(textureArray, ImageLayout::TRANSFER_DST_OPTIMAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				// 5. Create texture array view.
				{
					constexpr ImageViewDescriptor textureArrayViewDescriptor;

					textureArrayView = textureArray->createView(textureArrayViewDescriptor);
				}

				// 6. Create sampler.
				{
					SamplerDescriptor samplerDescriptor{};
					samplerDescriptor.magFilter = SamplerFilter::LINEAR;
					samplerDescriptor.minFilter = SamplerFilter::NEAREST;

					sampler = createSampler(samplerDescriptor);
				}

				// 7. Create render pass.
				{
					const ref<Surface> surface = getSurface();

					ColorAttachment colorAttachment{};
					colorAttachment.format = surface->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.colorAttachments = {colorAttachment};
					renderPassDescriptor.resolve = false;
					renderPassDescriptor.depthStencil = false;
					renderPassDescriptor.presentAttachmentIndex = 0;

					renderPass = createRenderPass(renderPassDescriptor);

					const ref<Swapchain>& swapchain = getSwapchain();

					swapchain->getImages(renderPass);
				}

				// 8. Create pipeline layout.
				{
					DescriptorSetLayoutBinding textureArrayDescriptorSetLayoutBinding;
					textureArrayDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
					textureArrayDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
					textureArrayDescriptorSetLayoutBinding.imageViews = {textureArrayView};
					textureArrayDescriptorSetLayoutBinding.samplers = {sampler};

					DescriptorSetLayout descriptorSetLayout;
					descriptorSetLayout.bindings = {textureArrayDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor;
					pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 9. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>();

					VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
					instanceInputBinding.registerAttribute<svec2<float32>>();
					instanceInputBinding.registerAttribute<uint32>();

					ShaderModuleDescriptor vertexShaderModuleDescriptor{};
					vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
					vertexShaderModuleDescriptor.entryPoint = "main";

					ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
					fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
					fragmentShaderModuleDescriptor.entryPoint = "main";

					ColorBlendAttachment colorBlendAttachment{};
					colorBlendAttachment.blend = false;

					RenderPipelineDescriptor renderPipelineDescriptor{};
					renderPipelineDescriptor.renderPass = renderPass;
					renderPipelineDescriptor.pipelineLayout = pipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/ArrayTextureOrTextureArray.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/TextureArray.frag", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					renderPipelineDescriptor.cullMode = CullMode::BACK;
					renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
					renderPipelineDescriptor.lineWidth = 1.0f;
					renderPipelineDescriptor.minSampleShading = 0.0f;
					renderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
					renderPipelineDescriptor.depthStencilState.depthTest = false;
					renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}

				// 10. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 6 * sizeof(svec2<float32>);

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					svec2<float32>* vertexMappedRange = vertexStagingBuffer->map<svec2<float32>>(0, vertexBufferSize);

					vertexMappedRange[0] = {-0.5f,  0.5f};
					vertexMappedRange[1] = { 0.5f,  0.5f};
					vertexMappedRange[2] = { 0.5f, -0.5f};
					vertexMappedRange[3] = {-0.5f,  0.5f};
					vertexMappedRange[4] = { 0.5f, -0.5f};
					vertexMappedRange[5] = {-0.5f, -0.5f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = vertexBufferSize;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					vertexBuffer = createBuffer(vertexBufferDescriptor);

					sequence<BufferCopyRegion> bufferCopyRegions(1);

					BufferCopyRegion& vertexBufferCopyRegion = bufferCopyRegions[0];
					vertexBufferCopyRegion.sourceOffset = 0;
					vertexBufferCopyRegion.destinationOffset = 0;
					vertexBufferCopyRegion.size = vertexBufferSize;

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, bufferCopyRegions);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				// 11. Create instance buffer.
				{
					const ref<Device> device = getDevice();

					constexpr uint64 instanceVertexBufferSize = 4 * sizeof(Instance);

					const ref<Buffer> instanceVertexStagingBuffer = createStagingBuffer(instanceVertexBufferSize);

					Instance* instances = instanceVertexStagingBuffer->map<Instance>(0, instanceVertexBufferSize);

					instances[0].vertexOffset = {-0.5f, 0.5f};
					instances[0].textureArrayIndex = 0;
					instances[1].vertexOffset = {0.5f, 0.5f};
					instances[1].textureArrayIndex = 1;
					instances[2].vertexOffset = {-0.5f, -0.5f};
					instances[2].textureArrayIndex = 2;
					instances[3].vertexOffset = {0.5f, -0.5f};
					instances[3].textureArrayIndex = 3;

					instanceVertexStagingBuffer->unmap();

					BufferDescriptor instanceVertexBufferDescriptor;
					instanceVertexBufferDescriptor.size = instanceVertexBufferSize;
					instanceVertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					instanceVertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					instanceVertexBuffer = createBuffer(instanceVertexBufferDescriptor);

					sequence<BufferCopyRegion> bufferCopyRegions(1);

					BufferCopyRegion& instanceBufferCopyRegion = bufferCopyRegions[0];
					instanceBufferCopyRegion.sourceOffset = 0;
					instanceBufferCopyRegion.destinationOffset = 0;
					instanceBufferCopyRegion.size = instanceVertexBufferSize;

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(instanceVertexStagingBuffer, instanceVertexBuffer, bufferCopyRegions);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
					device->waitIdle();
				}
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				const ref<Swapchain>& swapchain = getSwapchain();
				const float32 width = static_cast<float32>(swapchain->getExtentWidth());
				const float32 height = static_cast<float32>(swapchain->getExtentHeight());

				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.renderPass = renderPass;
				beginRenderPassDescriptor.swapchain = swapchain;
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};
				beginRenderPassDescriptor.imageIndex = getImageIndex();

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(svec2<float32>));
				commandEncoder->bindVertexBuffer(1, instanceVertexBuffer, 0, sizeof(Instance));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
				commandEncoder->draw(24, 4, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				const ref<Swapchain>& swapchain = getSwapchain();

				swapchain->getImages(renderPass);
			}

		private:
			struct Instance {
				svec2<float32> vertexOffset;
				uint32 textureArrayIndex;
			};

		private:
			static constexpr std::array<std::string_view, 4> ASSET_PATHS = {
				"Asset/Image/Baboon.png",
				"Asset/Image/Cameraman.png",
				"Asset/Image/Lenna.png",
				"Asset/Image/Pepper.png",
			};

			static constexpr uint32 TEXTURE_ARRAY_WIDTH = 512;
			static constexpr uint32 TEXTURE_ARRAY_HEIGHT = 512;

		private:
			ref<Image> textureArray;
			ref<ImageView> textureArrayView;

			ref<Sampler> sampler;

			ref<RenderPass> renderPass;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;

			ref<Buffer> instanceVertexBuffer;
	};
}