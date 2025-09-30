#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class ComputeShaderRenderer : public Renderer {
		public:
			void build() override {
				const ref<Device> device = getDevice();

				// 1. Create image.
				{
					ImageDescriptor imageDescriptor{};
					imageDescriptor.type = ImageType::TEXTURE_2D;
					imageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
					imageDescriptor.extent.width = 64;
					imageDescriptor.extent.height = 64;
					imageDescriptor.extent.depth = 1;
					imageDescriptor.mipLevelCount = 1;
					imageDescriptor.layerCount = 1;
					imageDescriptor.sampleCount = 1;
					imageDescriptor.usage = ImageUsage::STORAGE | ImageUsage::SAMPLED;
					imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					image = createImage(imageDescriptor);
				}

				// 2. Transition image layout.
				{
					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
					const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->setImageMemoryBarrier(image, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					graphicsQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				// 3. Create image view.
				{
					ImageViewDescriptor imageViewDescriptor{};
					imageViewDescriptor.mipLevelOffset = 0;
					imageViewDescriptor.mipLevelCount = image->getMipLevelCount();
					imageViewDescriptor.layerOffset = 0;
					imageViewDescriptor.layerCount = image->getLayerCount();

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
					computePipelineDescriptor.computeShaderModule = loadShaderModule("Asset/Shader/ImageUVGradient.comp", computeShaderModuleDescriptor);

					computePipeline = createComputePipeline(computePipelineDescriptor);
				}

				// 7. Dispatch work groups.
				{
					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> computeQueue = device->getQueue(QueueFamilyCapability::COMPUTE);
					const ref<CommandEncoder> commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->bindComputePipeline(computePipeline);
					commandEncoder->bindAllDescriptorSets();
					commandEncoder->dispatch(2, 2, 1);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					computeQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}

				// 8. Transition image layout.
				{
					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
					const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->setImageMemoryBarrier(image, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					graphicsQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}

				// 9. Create render pass and subpass.
				{
					const ref<Surface> surface = getSurface();

					Attachment swapchainAttachment{};
					swapchainAttachment.format = surface->getFormat();
					swapchainAttachment.sampleCount = 1;
					swapchainAttachment.loadOperation = LoadOperation::CLEAR;
					swapchainAttachment.storeOperation = StoreOperation::STORE;
					swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

					AttachmentReference swapchainAttachmentReference{};
					swapchainAttachmentReference.index = 0;
					swapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

					SubpassDescriptor subpassDescriptor{};
					subpassDescriptor.colorAttachmentReferences = {swapchainAttachmentReference};

					const ref<Subpass> subpass = createSubpass(subpassDescriptor);

					RenderPassDescriptor renderPassDescriptor{};
					renderPassDescriptor.attachments = {swapchainAttachment};
					renderPassDescriptor.subpasses = {subpass};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 10. Create framebuffers.
				recreateFramebuffers();

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
					vertexInputBinding.registerAttribute<svec2<float32>>();

					ShaderModuleDescriptor vertexShaderModuleDescriptor{};
					vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
					vertexShaderModuleDescriptor.entryPoint = "main";

					ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
					fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
					fragmentShaderModuleDescriptor.entryPoint = "main";

					ColorBlendAttachment colorBlendAttachment{};
					colorBlendAttachment.blend = false;

					RenderPipelineDescriptor renderPipelineDescriptor{};
					renderPipelineDescriptor.pipelineLayout = renderPipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Base/Vertex2DUV.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Base/TextureRGBAFlipY.frag", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					renderPipelineDescriptor.cullMode = CullMode::BACK;
					renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
					renderPipelineDescriptor.lineWidth = 1.0f;
					renderPipelineDescriptor.minSampleShading = 0.0f;
					renderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
					renderPipelineDescriptor.depthStencilState.depthTest = false;
					renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
					renderPipelineDescriptor.renderPass = renderPass;
					renderPipelineDescriptor.subpassIndex = 0;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}

				// 13. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 3 * sizeof(Vertex);

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

					vertexMappedRange[0].vertex = {-1.0f,  1.0f};
					vertexMappedRange[1].vertex = { 3.0f,  1.0f};
					vertexMappedRange[2].vertex = {-1.0f, -3.0f};

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

					const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
					const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, bufferCopyRegions);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
			}

		private:
			void recreateFramebuffers() {
				const ref<Swapchain>& swapchain = getSwapchain();

				for (ref<Framebuffer>& framebuffer : framebuffers) {
					framebuffer.~shared_ptr();
				}

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr};
				framebufferDescriptor.extent = swapchain->getExtent();
				framebufferDescriptor.layerCount = 1;

				for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
				}
			}

		private:
			struct Vertex {
				svec2<float32> vertex;
			};

		private:
			// This image is written to by the compute shader.
			ref<Image> image;
			ref<ImageView> imageView;

			ref<Sampler> sampler;

			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
	};
}