// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	class SampleRenderer : public Renderer {
		private:
			struct Vertex {
				vec3<float32> position;
			};

			struct Instance {
				vec3<float32> color;
			};

		private:
			static constexpr uint64 INSTANCE_COUNT = 2;
			static constexpr uint64 VERTEX_BUFFER_SIZE = 3 * INSTANCE_COUNT * sizeof(Vertex);
			static constexpr uint64 INSTANCE_BUFFER_SIZE = INSTANCE_COUNT * sizeof(Instance);

		public:
			void build() override {
				setupRenderPass();

				recreateDepthStencilImage();
				recreateDepthStencilImageView();
				recreateFramebuffers();

				setupGraphicsPipeline();
				createBuffers();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getSwapchainImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindGraphicsPipeline(graphicsPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				for (uint32 instanceIndex = 0; instanceIndex < INSTANCE_COUNT; instanceIndex += 1) {
					commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance), sizeof(Instance));
					commandEncoder->draw(3, 1, instanceIndex * 3, 0);
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateDepthStencilImage();
				recreateDepthStencilImageView();
				recreateFramebuffers();
			}

		private:
			void setupRenderPass() {
				Attachment swapchainAttachment{};
				swapchainAttachment.format = getSurface()->getFormat();
				swapchainAttachment.sampleCount = 1;
				swapchainAttachment.loadOperation = LoadOperation::CLEAR;
				swapchainAttachment.storeOperation = StoreOperation::STORE;
				swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

				Attachment depthStencilAttachment{};
				depthStencilAttachment.format = ImageFormat::D32_SFLOAT;
				depthStencilAttachment.sampleCount = 1;
				depthStencilAttachment.loadOperation = LoadOperation::CLEAR;
				depthStencilAttachment.storeOperation = StoreOperation::UNDEFINED;
				depthStencilAttachment.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				AttachmentReference swapchainAttachmentReference{};
				swapchainAttachmentReference.index = 0;
				swapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

				AttachmentReference depthStencilAttachmentReference{};
				depthStencilAttachmentReference.index = 1;
				depthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				SubpassDescriptor subpassDescriptor{};
				subpassDescriptor.colorAttachmentReferences = {swapchainAttachmentReference};
				subpassDescriptor.depthStencilAttachmentReference = depthStencilAttachmentReference;

				const ref<Subpass> subpass = createSubpass(subpassDescriptor);

				RenderPassDescriptor renderPassDescriptor{};
				renderPassDescriptor.attachments = {swapchainAttachment, depthStencilAttachment};
				renderPassDescriptor.subpasses = {subpass};

				renderPass = createRenderPass(renderPassDescriptor);
			}

			void recreateDepthStencilImage() {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();

				AttachmentImageDescriptor depthStencilAttachmentImageDescriptor{};
				depthStencilAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
				depthStencilAttachmentImageDescriptor.extent.width = swapchainExtent.width;
				depthStencilAttachmentImageDescriptor.extent.height = swapchainExtent.height;
				depthStencilAttachmentImageDescriptor.extent.depth = 1;
				depthStencilAttachmentImageDescriptor.mipLevelCount = 1;
				depthStencilAttachmentImageDescriptor.layerCount = 1;

				SET_DEBUG_NAME(depthStencilAttachmentImageDescriptor, "Depth stencil image");

				depthStencilImage = renderPass->createDepthStencilAttachmentImage(depthStencilAttachmentImageDescriptor, 0);
			}

			void recreateDepthStencilImageView() {
				ImageViewDescriptor depthStencilImageViewDescriptor{};
				depthStencilImageViewDescriptor.mipLevelOffset = 0;
				depthStencilImageViewDescriptor.mipLevelCount = 1;
				depthStencilImageViewDescriptor.layerOffset = 0;
				depthStencilImageViewDescriptor.layerCount = 1;

				SET_DEBUG_NAME(depthStencilImageViewDescriptor, "Depth stencil image view");

				depthStencilImageView = depthStencilImage->createView(depthStencilImageViewDescriptor);
			}

			void recreateFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr, depthStencilImageView};
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

			void setupGraphicsPipeline() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};

				SET_DEBUG_NAME(pipelineLayoutDescriptor, "Pipeline layout");

				const ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<vec3<float32>>();

				VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
				instanceInputBinding.registerAttribute<vec3<float32>>();

				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Vertex shader module");

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Fragment shader module");

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/DepthTest/DepthTest.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/DepthTest/DepthTest.frag");

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				GraphicsPipelineDescriptor graphicsPipelineDescriptor{};
				graphicsPipelineDescriptor.pipelineLayout = pipelineLayout;
				graphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
				graphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				graphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				graphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				graphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
				graphicsPipelineDescriptor.cullMode = CullMode::BACK;
				graphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				graphicsPipelineDescriptor.lineWidth = 1.0f;
				graphicsPipelineDescriptor.minSampleShading = 0.0f;
				graphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				graphicsPipelineDescriptor.depthStencilState.depthTest = true;
				graphicsPipelineDescriptor.depthStencilState.depthWrite = true;
				graphicsPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
				graphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				graphicsPipelineDescriptor.renderPass = renderPass;
				graphicsPipelineDescriptor.subpassIndex = 0;

				SET_DEBUG_NAME(graphicsPipelineDescriptor, "Render pipeline");

				graphicsPipeline = createGraphicsPipeline(graphicsPipelineDescriptor);
			}

			void createBuffers() {
				const ref<Buffer2<uint8>> stagingBuffer = createStagingBuffer2<uint8>(VERTEX_BUFFER_SIZE + INSTANCE_BUFFER_SIZE);

				uint8* stagingBufferMappedRange = stagingBuffer->map(0, stagingBuffer->getSize());

				Vertex* vertexStagingBufferMappedRange = pointer_cast<Vertex*>(stagingBufferMappedRange);
				vertexStagingBufferMappedRange[0].position = {-0.75f,  0.75f,  1.0f};
				vertexStagingBufferMappedRange[1].position = { 0.25f,  0.75f,  1.0f};
				vertexStagingBufferMappedRange[2].position = {-0.75f, -0.75f,  1.0f};
				vertexStagingBufferMappedRange[3].position = {-0.5f,   0.5f,   0.0f};
				vertexStagingBufferMappedRange[4].position = { 0.5f,   0.5f,   0.0f};
				vertexStagingBufferMappedRange[5].position = {-0.5f,  -0.5f,   0.0f};

				Instance* instanceStagingBufferMappedRange = pointer_cast<Instance*>(stagingBufferMappedRange + VERTEX_BUFFER_SIZE);
				instanceStagingBufferMappedRange[0].color = {1.0f, 1.0f, 1.0f};
				instanceStagingBufferMappedRange[1].color = {1.0f, 0.2f, 0.0f};

				stagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
				vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				SET_DEBUG_NAME(vertexBufferDescriptor, "Vertex buffer");

				BufferDescriptor instanceBufferDescriptor{};
				instanceBufferDescriptor.size = INSTANCE_BUFFER_SIZE;
				instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				SET_DEBUG_NAME(instanceBufferDescriptor, "Instance buffer");

				vertexBuffer = createBuffer2<Vertex>(vertexBufferDescriptor);
				instanceBuffer = createBuffer2<Instance>(instanceBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->getSize();

				BufferCopyRegion instanceBufferCopyRegion{};
				instanceBufferCopyRegion.sourceOffset = vertexBuffer->getSize();
				instanceBufferCopyRegion.destinationOffset = 0;
				instanceBufferCopyRegion.size = instanceBuffer->getSize();

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Buffer copy command buffer");

				const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, {instanceBufferCopyRegion});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				graphicsQueue->submit({commandBuffer});
				graphicsQueue->waitIdle();
			}

		private:
			ref<RenderPass> renderPass;
			ref<Image> depthStencilImage;
			ref<ImageView> depthStencilImageView;
			sequence<ref<Framebuffer>> framebuffers;
			ref<GraphicsPipeline> graphicsPipeline;
			ref<Buffer2<Vertex>> vertexBuffer;
			ref<Buffer2<Instance>> instanceBuffer;
	};
}