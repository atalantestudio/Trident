#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class ComputeShaderIndirectRenderer : public Renderer {
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

				ref<Buffer> vertexStagingBuffer;

				// 5. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 3 * sizeof(Vertex);

					vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

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
				}

				ref<Buffer> indirectStagingBuffer;

				// 6. Create indirect buffer.
				{
					static constexpr uint64 indirectBufferSize = sizeof(DispatchIndirectCommand);

					indirectStagingBuffer = createStagingBuffer(indirectBufferSize);

					DispatchIndirectCommand* dispatchIndirectCommands = indirectStagingBuffer->map<DispatchIndirectCommand>(0, indirectBufferSize);
					dispatchIndirectCommands[0].x = 2;
					dispatchIndirectCommands[0].y = 2;
					dispatchIndirectCommands[0].z = 1;

					indirectStagingBuffer->unmap();

					BufferDescriptor indirectBufferDescriptor{};
					indirectBufferDescriptor.size = indirectBufferSize;
					indirectBufferDescriptor.usage = BufferUsage::INDIRECT | BufferUsage::TRANSFER_DST;
					indirectBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
					indirectBufferDescriptor.persistent = false;

					indirectBuffer = createBuffer(indirectBufferDescriptor);
				}

				// 7. Write to vertex and indirect buffers.
				{
					sequence<BufferCopyRegion> vertexBufferCopyRegions(1);

					BufferCopyRegion& vertexBufferCopyRegion = vertexBufferCopyRegions[0];
					vertexBufferCopyRegion.sourceOffset = 0;
					vertexBufferCopyRegion.destinationOffset = 0;
					vertexBufferCopyRegion.size = vertexStagingBuffer->size;

					sequence<BufferCopyRegion> indirectBufferCopyRegions(1);

					BufferCopyRegion& indirectBufferCopyRegion = indirectBufferCopyRegions[0];
					indirectBufferCopyRegion.sourceOffset = 0;
					indirectBufferCopyRegion.destinationOffset = 0;
					indirectBufferCopyRegion.size = indirectStagingBuffer->size;

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
					const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, vertexBufferCopyRegions);
					commandEncoder->copyBufferToBuffer(indirectStagingBuffer, indirectBuffer, indirectBufferCopyRegions);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}

				ref<PipelineLayout> computePipelineLayout;

				// 8. Create compute pipeline layout.
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

				// 9. Create compute pipeline.
				{
					ShaderModuleDescriptor computeShaderModuleDescriptor{};
					computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
					computeShaderModuleDescriptor.entryPoint = "main";

					ComputePipelineDescriptor computePipelineDescriptor;
					computePipelineDescriptor.pipelineLayout = computePipelineLayout;
					computePipelineDescriptor.computeShaderModule = loadShaderModule("Asset/Shader/ImageUVGradient.comp", computeShaderModuleDescriptor);

					computePipeline = createComputePipeline(computePipelineDescriptor);
				}

				// 10. Dispatch work groups.
				{
					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> computeQueue = device->getQueue(QueueFamilyCapability::COMPUTE);
					const ref<CommandEncoder> commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->bindComputePipeline(computePipeline);
					commandEncoder->bindAllDescriptorSets();
					commandEncoder->dispatchIndirect(indirectBuffer, 0);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					computeQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
				}

				// 11. Transition image layout.
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

				// 12. Create render pass and subpass.
				{
					const ref<Surface> surface = getSurface();

					ColorAttachment colorAttachment{};
					colorAttachment.format = surface->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;
					colorAttachment.finalImageLayout = ImageLayout::PRESENT_SRC;

					SubpassDescriptor subpassDescriptor{};
					subpassDescriptor.colorAttachments = {colorAttachment};
					subpassDescriptor.depthStencil = false;

					subpass = createSubpass(subpassDescriptor);

					RenderPassDescriptor renderPassDescriptor{};
					renderPassDescriptor.subpasses = {subpass};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 13. Create framebuffers.
				recreateFramebuffers();

				// 14. Create render pipeline layout.
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

				// 15. Create render pipeline.
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
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Base/TextureRGBA.frag", fragmentShaderModuleDescriptor);
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
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				const ref<Swapchain>& swapchain = getSwapchain();
				const float32 width = static_cast<float32>(swapchain->getExtent().width);
				const float32 height = static_cast<float32>(swapchain->getExtent().height);

				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtent());
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
			}

		private:
			void recreateFramebuffers() {
				const ref<Swapchain>& swapchain = getSwapchain();

				if (framebuffers.count > 0) {
					for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
						framebuffers[framebufferIndex].~shared_ptr();
					}
				}

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = sequence<ref<ImageView>>(1);
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

			ref<Subpass> subpass;
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indirectBuffer;
	};
}