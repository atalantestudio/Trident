#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class ComputeShaderRenderer : public Renderer {
		public:
			ComputeShaderRenderer() {
				const ref<Device> device = getDevice();

				// 1. Create image.
				{
					ImageDescriptor imageDescriptor{};
					imageDescriptor.type = ImageType::TEXTURE_2D;
					imageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
					imageDescriptor.usage = ImageUsage::STORAGE | ImageUsage::SAMPLED;
					imageDescriptor.width = 64;
					imageDescriptor.height = 64;
					imageDescriptor.depth = 1;
					imageDescriptor.mipLevelCount = 1;
					imageDescriptor.arrayLayerCount = 1;
					imageDescriptor.sampleCount = 1;

					image = createImage(imageDescriptor);
				}

				// 2. Transition image layout.
				{
					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->setImageMemoryBarrier(image, ImageLayout::UNDEFINED, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				// 3. Create image view.
				{
					constexpr ImageViewDescriptor imageViewDescriptor;

					imageView = image->createView(imageViewDescriptor);
				}

				// 4. Create sampler.
				{
					SamplerDescriptor samplerDescriptor{};
					samplerDescriptor.magFilter = SamplerFilter::NEAREST;
					samplerDescriptor.minFilter = SamplerFilter::NEAREST;

					sampler = createSampler(samplerDescriptor);
				}

				ref<PipelineLayout> computePipelineLayout;

				// 5. Create compute pipeline layout.
				{
					DescriptorSetLayoutBinding imageDescriptorSetLayoutBinding;
					imageDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
					imageDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
					imageDescriptorSetLayoutBinding.samplers = {sampler};
					imageDescriptorSetLayoutBinding.imageViews = {imageView};

					DescriptorSetLayout descriptorSetLayout;
					descriptorSetLayout.bindings = {imageDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor;
					pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

					computePipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				ref<ComputePipeline> computePipeline;

				// 6. Create compute pipeline.
				{
					ShaderModuleDescriptor computeShaderModuleDescriptor{};
					computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
					computeShaderModuleDescriptor.entryPoint = "main";

					ComputePipelineDescriptor computePipelineDescriptor;
					computePipelineDescriptor.pipelineLayout = computePipelineLayout;
					computePipelineDescriptor.computeShaderModule = loadShaderModule("Asset/Shader/Compute.comp.spv", computeShaderModuleDescriptor);

					computePipeline = createComputePipeline(computePipelineDescriptor);
				}

				// 7. Dispatch work groups.
				{
					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->bindComputePipeline(computePipeline);
					commandEncoder->bindAllDescriptorSets();
					commandEncoder->dispatchGroups(2, 2, 1);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				// 8. Transition image layout.
				{
					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->setImageMemoryBarrier(image, ImageLayout::GENERAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				// 9. Create swapchain.
				{
					SwapchainDescriptor swapchainDescriptor;
					swapchainDescriptor.extentWidth = Application::get().getWindow().getSize()[0];
					swapchainDescriptor.extentHeight = Application::get().getWindow().getSize()[1];

					swapchain = createSwapchain(swapchainDescriptor);
				}

				// 8. Create render pass.
				{
					ColorAttachment colorAttachment{};
					colorAttachment.format = getSurface()->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.colorAttachments = {colorAttachment};
					renderPassDescriptor.resolve = false;
					renderPassDescriptor.depthStencil = false;
					renderPassDescriptor.presentAttachmentIndex = 0;

					renderPass = createRenderPass(renderPassDescriptor);
				}

				swapchain->getImages(renderPass);

				// 11. Create render pipeline layout.
				{
					DescriptorSetLayoutBinding imageDescriptorSetLayoutBinding;
					imageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
					imageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
					imageDescriptorSetLayoutBinding.samplers = {sampler};
					imageDescriptorSetLayoutBinding.imageViews = {imageView};

					DescriptorSetLayout descriptorSetLayout;
					descriptorSetLayout.bindings = {imageDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor;
					pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

					renderPipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 12. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>(false);

					ShaderModuleDescriptor vertexShaderModuleDescriptor{};
					vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
					vertexShaderModuleDescriptor.entryPoint = "main";

					ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
					fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
					fragmentShaderModuleDescriptor.entryPoint = "main";

					ColorBlendAttachment colorBlendAttachment{};
					colorBlendAttachment.blend = false;

					RenderPipelineDescriptor renderPipelineDescriptor;
					renderPipelineDescriptor.renderPass = renderPass;
					renderPipelineDescriptor.pipelineLayout = renderPipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Compute.vert.spv", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Compute.frag.spv", fragmentShaderModuleDescriptor);
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

				// 13. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 3 * sizeof(Vertex);

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

					vertexMappedRange[0].position = {-1.0f,  1.0f};
					vertexMappedRange[1].position = { 3.0f,  1.0f};
					vertexMappedRange[2].position = {-1.0f, -3.0f};

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
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
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
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
				commandEncoder->draw(3, 1, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				const ref<Device> device = getDevice();

				device->waitIdle();

				SwapchainDescriptor swapchainDescriptor;
				swapchainDescriptor.extentWidth = event.getWidth();
				swapchainDescriptor.extentHeight = event.getHeight();

				swapchain->update(swapchainDescriptor);
				swapchain->getImages(renderPass);
			}

		private:
			struct Vertex {
				svec2<float32> position;
			};

		private:
			// This image is written to by the compute shader.
			ref<Image> image;
			ref<ImageView> imageView;

			ref<Sampler> sampler;

			ref<Swapchain> swapchain;

			ref<RenderPass> renderPass;

			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
	};
}