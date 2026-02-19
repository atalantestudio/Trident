// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	class SampleRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> vertex;
			};

		private:
			static constexpr uint32 VERTEX_COUNT = 3;

		public:
			void build() override {
				createOutputImage();
				createOutputImageView();
				createBuffers();
				writeImage();

				setupRenderPass();
				setupSampler();

				createImageDescriptorSetLayout();
				allocateImageDescriptorSetLayout();
				updateImageDescriptorSetLayout();

				setupPipelineLayout();
				setupRenderPipeline();

				recreateFramebuffers();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getSwapchainImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, {imageDescriptorSet});
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateFramebuffers();
			}

		private:
			void createOutputImage() {
				ImageDescriptor outputImageDescriptor{};
				outputImageDescriptor.type = ImageType::TEXTURE_2D;
				outputImageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
				outputImageDescriptor.extent.width = 64;
				outputImageDescriptor.extent.height = 64;
				outputImageDescriptor.extent.depth = 1;
				outputImageDescriptor.mipLevelCount = 1;
				outputImageDescriptor.layerCount = 1;
				outputImageDescriptor.sampleCount = 1;
				outputImageDescriptor.usage = ImageUsage::STORAGE | ImageUsage::SAMPLED;
				outputImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				SET_DEBUG_NAME(outputImageDescriptor, "Output");

				outputImage = createImage(outputImageDescriptor);
			}

			void createOutputImageView() {
				ImageViewDescriptor outputImageViewDescriptor{};
				outputImageViewDescriptor.mipLevelOffset = 0;
				outputImageViewDescriptor.mipLevelCount = 1;
				outputImageViewDescriptor.layerOffset = 0;
				outputImageViewDescriptor.layerCount = 1;

				SET_DEBUG_NAME(outputImageViewDescriptor, "Output");

				outputImageView = outputImage->createView(outputImageViewDescriptor);
			}

			void setupRenderPass() {
				Attachment swapchainAttachment{};
				swapchainAttachment.format = getSurface()->getFormat();
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

			void createImageDescriptorSetLayout() {
				DescriptorSetLayoutBinding imageBinding{};
				imageBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
				imageBinding.descriptorCount = 1;
				imageBinding.shaderStages = ShaderStage::FRAGMENT;

				imageDescriptorSetLayout = createDescriptorSetLayout({imageBinding});
			}

			void allocateImageDescriptorSetLayout() {
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({imageDescriptorSetLayout});

				imageDescriptorSet = descriptorSets[0];
			}

			void updateImageDescriptorSetLayout() {
				DescriptorImage descriptorImage{};
				descriptorImage.imageView = outputImageView;
				descriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
				descriptorImage.sampler = sampler;

				WriteDescriptorSet imageWriteDescriptorSet{};
				imageWriteDescriptorSet.descriptorSet = imageDescriptorSet;
				imageWriteDescriptorSet.bindingIndex = 0;
				imageWriteDescriptorSet.descriptorImages = {descriptorImage};

				getDevice()->updateDescriptorSets({imageWriteDescriptorSet});
			}

			void setupPipelineLayout() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};
				pipelineLayoutDescriptor.descriptorSetLayouts = {imageDescriptorSetLayout};

				SET_DEBUG_NAME(pipelineLayoutDescriptor, "Render pipeline layout");

				pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
			}

			void setupRenderPipeline() {
				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<svec2<float32>>();

				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Vertex shader module");

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Fragment shader module");

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/ComputeShader/ComputeShader.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/ComputeShader/ComputeShader.frag");

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				RenderPipelineDescriptor renderPipelineDescriptor{};
				renderPipelineDescriptor.pipelineLayout = pipelineLayout;
				renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
				renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
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

				SET_DEBUG_NAME(renderPipelineDescriptor, "Render pipeline");

				renderPipeline = createRenderPipeline(renderPipelineDescriptor);
			}

			void createBuffers() {
				const ref<Buffer> stagingBuffer = createStagingBuffer(VERTEX_COUNT * sizeof(Vertex) + sizeof(DispatchIndirectCommand));

				uint8* stagingBufferMappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->getSize());

				Vertex* vertexStagingBufferMappedRange = pointer_cast<Vertex*>(stagingBufferMappedRange);
				vertexStagingBufferMappedRange[0].vertex = {-1.0f,  1.0f};
				vertexStagingBufferMappedRange[1].vertex = { 3.0f,  1.0f};
				vertexStagingBufferMappedRange[2].vertex = {-1.0f, -3.0f};

				DispatchIndirectCommand* indirectStagingBufferMappedRange = pointer_cast<DispatchIndirectCommand*>(stagingBufferMappedRange + VERTEX_COUNT * sizeof(Vertex));
				indirectStagingBufferMappedRange[0].x = 2;
				indirectStagingBufferMappedRange[0].y = 2;
				indirectStagingBufferMappedRange[0].z = 1;

				stagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = VERTEX_COUNT * sizeof(Vertex);
				vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				SET_DEBUG_NAME(vertexBufferDescriptor, "Vertex buffer");

				BufferDescriptor indirectBufferDescriptor{};
				indirectBufferDescriptor.size = sizeof(DispatchIndirectCommand);
				indirectBufferDescriptor.usage = BufferUsage::INDIRECT | BufferUsage::TRANSFER_DST;
				indirectBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				SET_DEBUG_NAME(indirectBufferDescriptor, "Indirect buffer");

				vertexBuffer = createBuffer(vertexBufferDescriptor);
				indirectBuffer = createBuffer(indirectBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->getSize();

				BufferCopyRegion indirectBufferCopyRegion{};
				indirectBufferCopyRegion.sourceOffset = vertexBuffer->getSize();
				indirectBufferCopyRegion.destinationOffset = 0;
				indirectBufferCopyRegion.size = indirectBuffer->getSize();

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Buffer copy command buffer");

				const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, indirectBuffer, {indirectBufferCopyRegion});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit({commandBuffer});
				transferQueue->waitIdle();
			}

			void setupSampler() {
				SamplerDescriptor samplerDescriptor{};
				samplerDescriptor.magFilter = SamplerFilter::NEAREST;
				samplerDescriptor.minFilter = SamplerFilter::NEAREST;

				sampler = createSampler(samplerDescriptor);
			}

			void writeImage() const {
				DescriptorSetLayoutBinding imageBinding{};
				imageBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
				imageBinding.descriptorCount = 1;
				imageBinding.shaderStages = ShaderStage::COMPUTE;

				const ref<DescriptorSetLayout> imageDescriptorSetLayout = createDescriptorSetLayout({imageBinding});
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({imageDescriptorSetLayout});
				const ref<DescriptorSet> imageDescriptorSet = descriptorSets[0];

				DescriptorImage descriptorImage{};
				descriptorImage.imageView = outputImageView;
				descriptorImage.imageLayout = ImageLayout::GENERAL;
				descriptorImage.sampler = nullptr;

				WriteDescriptorSet imageWriteDescriptorSet{};
				imageWriteDescriptorSet.descriptorSet = imageDescriptorSet;
				imageWriteDescriptorSet.bindingIndex = 0;
				imageWriteDescriptorSet.descriptorImages = {descriptorImage};

				getDevice()->updateDescriptorSets({imageWriteDescriptorSet});

				PipelineLayoutDescriptor pipelineLayoutDescriptor{};
				pipelineLayoutDescriptor.descriptorSetLayouts = {imageDescriptorSetLayout};

				SET_DEBUG_NAME(pipelineLayoutDescriptor, "Compute pipeline layout");

				const ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				ShaderModuleDescriptor computeShaderModuleDescriptor{};
				computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
				computeShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(computeShaderModuleDescriptor, "Compute shader module");

				const ref<ShaderModule> computeShaderModule = loadShaderModule(computeShaderModuleDescriptor, "Sample/ComputeShader/ComputeShader.comp");

				ComputePipelineDescriptor computePipelineDescriptor{};
				computePipelineDescriptor.pipelineLayout = pipelineLayout;
				computePipelineDescriptor.computeShaderModule = computeShaderModule;

				SET_DEBUG_NAME(computePipelineDescriptor, "Compute pipeline");

				const ref<ComputePipeline> computePipeline = createComputePipeline(computePipelineDescriptor);

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Compute command buffer");

				const ref<Queue> computeQueue = getDevice()->getQueue(QueueFamilyCapability::COMPUTE);
				const ref<CommandEncoder> commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->setImageMemoryBarrier(outputImage, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
				commandEncoder->bindComputePipeline(computePipeline);
				commandEncoder->bindDescriptorSets(pipelineLayout, 0, {imageDescriptorSet});
				commandEncoder->dispatchIndirect(indirectBuffer, 0);
				commandEncoder->setImageMemoryBarrier(outputImage, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				computeQueue->submit({commandBuffer});
				computeQueue->waitIdle();
			}

			void recreateFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr};
				framebufferDescriptor.extent = swapchain->getExtent();

				#if defined(BUILD_MODE_DEBUG)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

				while (framebufferIndex < framebuffers.count()) {
					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("In flight framebuffer []", framebufferIndex);

						SET_DEBUG_NAME(framebufferDescriptor, name);
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

		private:
			ref<Image> outputImage;
			ref<ImageView> outputImageView;
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<Sampler> sampler;
			ref<DescriptorSetLayout> imageDescriptorSetLayout;
			ref<DescriptorSet> imageDescriptorSet;
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indirectBuffer;
	};
}