#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextureArrayRenderer : public Renderer {
		public:
			TextureArrayRenderer() :
				baboonImage(loadImage(BABOON_IMAGE_PATH)),
				cameramanImage(loadImage(CAMERAMAN_IMAGE_PATH)),
				lennaImage(loadImage(LENNA_IMAGE_PATH)),
				pepperImage(loadImage(PEPPER_IMAGE_PATH))
			{
				const ref<Device> device = getDevice();

				// 1. Create image views.
				{
					ImageViewDescriptor imageViewDescriptor{};
					imageViewDescriptor.imageAspect = ImageAspect::COLOR;

					baboonImageView = baboonImage->createView(imageViewDescriptor);
					cameramanImageView = cameramanImage->createView(imageViewDescriptor);
					lennaImageView = lennaImage->createView(imageViewDescriptor);
					pepperImageView = pepperImage->createView(imageViewDescriptor);
				}

				// 2. Create sampler.
				{
					SamplerDescriptor samplerDescriptor{};
					samplerDescriptor.magFilter = SamplerFilter::LINEAR;
					samplerDescriptor.minFilter = SamplerFilter::NEAREST;

					sampler = createSampler(samplerDescriptor);
				}

				// 3. Create render pass.
				{
					Attachment colorAttachment{};
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.colorAttachments = {colorAttachment};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 4. Create swapchain.
				{
					SwapchainDescriptor swapchainDescriptor;
					swapchainDescriptor.renderPass = renderPass;
					swapchainDescriptor.extentWidth = Application::get().getWindow().getSize()[0];
					swapchainDescriptor.extentHeight = Application::get().getWindow().getSize()[1];

					swapchain = createSwapchain(swapchainDescriptor);
				}

				// 5. Create pipeline layout.
				{
					DescriptorSetLayoutBinding textureArrayDescriptorSetLayoutBinding;
					textureArrayDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
					textureArrayDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;

					// TODO: Don't provide a copy of the sampler for each image.
					textureArrayDescriptorSetLayoutBinding.imageViews = {baboonImageView, cameramanImageView, lennaImageView, pepperImageView};
					textureArrayDescriptorSetLayoutBinding.samplers = {sampler, sampler, sampler, sampler};

					DescriptorSetLayout descriptorSetLayout;
					descriptorSetLayout.bindings = {textureArrayDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor;
					pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 6. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>(false);

					VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
					instanceInputBinding.registerAttribute<vec4<float32>>(false);
					instanceInputBinding.registerAttribute<uint32>(false);

					ShaderModuleDescriptor vertexShaderModuleDescriptor{};
					vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
					vertexShaderModuleDescriptor.entryPoint = "main";

					ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
					fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
					fragmentShaderModuleDescriptor.entryPoint = "main";

					RenderPipelineDescriptor renderPipelineDescriptor;
					renderPipelineDescriptor.renderPass = renderPass;
					renderPipelineDescriptor.pipelineLayout = pipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/TextureArray.vert.spv", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/TextureArray.frag.spv", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}

				// 7. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 6 * sizeof(Vertex);

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					Vertex* vertices = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

					vertices[0].position = {-0.5f,  0.5f};
					vertices[1].position = { 0.5f,  0.5f};
					vertices[2].position = { 0.5f, -0.5f};
					vertices[3].position = {-0.5f,  0.5f};
					vertices[4].position = { 0.5f, -0.5f};
					vertices[5].position = {-0.5f, -0.5f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = vertexBufferSize;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

					vertexBuffer = createBuffer(vertexBufferDescriptor);

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, 0, 0, vertexBufferSize);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				// 8. Create instance buffer.
				{
					constexpr uint64 instanceVertexBufferSize = 4 * sizeof(Instance);

					const ref<Buffer> instanceVertexStagingBuffer = createStagingBuffer(instanceVertexBufferSize);

					Instance* instances = instanceVertexStagingBuffer->map<Instance>(0, instanceVertexBufferSize);

					instances[0].offset = {-0.5f, 0.5f, 0.0f, 0.0f };
					instances[0].textureIndex = 0;
					instances[1].offset = {0.5f, 0.5f, 0.0f, 0.0f };
					instances[1].textureIndex = 1;
					instances[2].offset = {-0.5f, -0.5f, 0.0f, 0.0f };
					instances[2].textureIndex = 2;
					instances[3].offset = {0.5f, -0.5f, 0.0f, 0.0f};
					instances[3].textureIndex = 3;

					instanceVertexStagingBuffer->unmap();

					BufferDescriptor instanceVertexBufferDescriptor;
					instanceVertexBufferDescriptor.size = instanceVertexBufferSize;
					instanceVertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

					instanceVertexBuffer = createBuffer(instanceVertexBufferDescriptor);

					const ref<Device> device = getDevice();

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(instanceVertexStagingBuffer, instanceVertexBuffer, 0, 0, instanceVertexBufferSize);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
					device->waitIdle();
				}
			}

			void render(const ref<CommandEncoder>& commandEncoder) const final override {
				const float32 width = static_cast<float32>(swapchain->getExtentWidth());
				const float32 height = static_cast<float32>(swapchain->getExtentHeight());

				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.renderPass = renderPass;
				beginRenderPassDescriptor.swapchain = swapchain;
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};
				beginRenderPassDescriptor.imageIndex = acquireNextImage(swapchain);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->bindVertexBuffer(1, instanceVertexBuffer, 0, sizeof(Instance));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
				commandEncoder->draw(24, 4, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) final override {
				const ref<Device> device = getDevice();

				device->waitIdle();

				SwapchainDescriptor swapchainDescriptor;
				swapchainDescriptor.renderPass = renderPass;
				swapchainDescriptor.extentWidth = event.getWidth();
				swapchainDescriptor.extentHeight = event.getHeight();

				swapchain->recreate(swapchainDescriptor);
			}

		private:
			ref<Image> loadImage(std::string_view imagePath) {
				const ref<Device> device = getDevice();

				const PNGFile file = Filesystem::load<PNGFile>(imagePath);
				const uint64 imageBufferSize = file.data.size() * sizeof(uint8);

				const ref<Buffer> imageStagingBuffer = createStagingBuffer(imageBufferSize);

				uint8* imageData = imageStagingBuffer->map<uint8>(0, imageBufferSize);

				std::copy(file.data.begin(), file.data.end(), imageData);

				imageStagingBuffer->unmap();

				ImageDescriptor imageDescriptor{};
				imageDescriptor.type = ImageType::TEXTURE_2D;
				imageDescriptor.format = ImageFormat::R8G8B8A8_SRGB;
				imageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
				imageDescriptor.width = file.width;
				imageDescriptor.height = file.height;
				imageDescriptor.depth = 1;
				imageDescriptor.mipLevelCount = 1;
				imageDescriptor.arrayLayerCount = 1;
				imageDescriptor.sampleCount = 1;

				const ref<Image> image = createImage(imageDescriptor);

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->setImageMemoryBarrier(image, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
				commandEncoder->copyBufferToImage(imageStagingBuffer, image, file.width, file.height, 0);
				commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
				device->waitIdle();

				return image;
			}

		private:
			struct Vertex {
				svec2<float32> position;
			};

			struct Instance {
				// TODO: Remove padding.
				vec4<float32> offset;

				uint32 textureIndex;
			};

		private:
			static constexpr std::string_view BABOON_IMAGE_PATH = "Asset/Image/Baboon.png";
			static constexpr std::string_view CAMERAMAN_IMAGE_PATH = "Asset/Image/Cameraman.png";
			static constexpr std::string_view LENNA_IMAGE_PATH = "Asset/Image/Lenna.png";
			static constexpr std::string_view PEPPER_IMAGE_PATH = "Asset/Image/Pepper.png";

		private:
			ref<Image> baboonImage;
			ref<ImageView> baboonImageView;

			ref<Image> cameramanImage;
			ref<ImageView> cameramanImageView;

			ref<Image> lennaImage;
			ref<ImageView> lennaImageView;

			ref<Image> pepperImage;
			ref<ImageView> pepperImageView;

			ref<Sampler> sampler;

			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;

			ref<Buffer> instanceVertexBuffer;
	};
}