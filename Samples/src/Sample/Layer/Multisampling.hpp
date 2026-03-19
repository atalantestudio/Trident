// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../SampleLayer.hpp"

namespace Trident {
	class SampleMultisamplingRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> position;
				vec3<float32> color;
			};

		private:
			static constexpr uint8 SAMPLE_COUNT = 4;
			static constexpr uint32 VERTEX_BUFFER_SIZE = 3 * sizeof(Vertex);

		public:
			void build() override {
				setupRenderPass();

				recreateImage();
				recreateImageView();
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
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->bindGraphicsPipeline(graphicsPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				recreateImage();
				recreateImageView();
				recreateFramebuffers();
			}

		private:
			void setupRenderPass() {
				Attachment swapchainAttachment{};
				swapchainAttachment.format = getSurface()->getFormat();
				swapchainAttachment.sampleCount = SAMPLE_COUNT;
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

			void recreateImage() {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();

				AttachmentImageDescriptor imageDescriptor{};
				imageDescriptor.type = ImageType::TEXTURE_2D;
				imageDescriptor.extent.width = swapchainExtent.width;
				imageDescriptor.extent.height = swapchainExtent.height;
				imageDescriptor.extent.depth = 1;
				imageDescriptor.mipLevelCount = 1;
				imageDescriptor.layerCount = 1;

				SET_DEBUG_NAME(imageDescriptor, "Color attachment");

				image = renderPass->createColorAttachmentImage(imageDescriptor, 0);
			}

			void recreateImageView() {
				ImageViewDescriptor imageViewDescriptor{};
				imageViewDescriptor.mipLevelOffset = 0;
				imageViewDescriptor.mipLevelCount = 1;
				imageViewDescriptor.layerOffset = 0;
				imageViewDescriptor.layerCount = 1;

				SET_DEBUG_NAME(imageViewDescriptor, "Color attachment");

				imageView = image->createView(imageViewDescriptor);
			}

			void recreateFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {imageView, nullptr};
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

					framebufferDescriptor.imageViews[1] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			void setupGraphicsPipeline() {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};

				SET_DEBUG_NAME(pipelineLayoutDescriptor, "Pipeline layout");

				const ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<svec2<float32>>();
				vertexInputBinding.registerAttribute<vec3<float32>>();

				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Vertex shader module");

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Fragment shader module");

				const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/Multisampling/Multisampling.vert");
				const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Sample/Multisampling/Multisampling.frag");

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				GraphicsPipelineDescriptor graphicsPipelineDescriptor{};
				graphicsPipelineDescriptor.pipelineLayout = pipelineLayout;
				graphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
				graphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
				graphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
				graphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				graphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
				graphicsPipelineDescriptor.cullMode = CullMode::BACK;
				graphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				graphicsPipelineDescriptor.lineWidth = 1.0f;
				graphicsPipelineDescriptor.minSampleShading = 0.0f;
				graphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				graphicsPipelineDescriptor.depthStencilState.depthTest = false;
				graphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				graphicsPipelineDescriptor.renderPass = renderPass;
				graphicsPipelineDescriptor.subpassIndex = 0;

				SET_DEBUG_NAME(graphicsPipelineDescriptor, "Render pipeline");

				graphicsPipeline = createGraphicsPipeline(graphicsPipelineDescriptor);
			}

			void createBuffers() {
				const ref<Buffer<Vertex>> stagingBuffer = createStagingBuffer<Vertex>(VERTEX_BUFFER_SIZE);

				Vertex* vertexStagingBufferMappedRange = stagingBuffer->map(0, stagingBuffer->getSize());
				vertexStagingBufferMappedRange[0].position = {-0.5f, -0.5f};
				vertexStagingBufferMappedRange[0].color = {1.0f, 0.0f, 0.0f};
				vertexStagingBufferMappedRange[1].position = {0.0f, 0.5f};
				vertexStagingBufferMappedRange[1].color = {0.0f, 1.0f, 0.0f};
				vertexStagingBufferMappedRange[2].position = {0.5f, -0.5f};
				vertexStagingBufferMappedRange[2].color = {0.0f, 0.0f, 1.0f};

				stagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
				vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

				SET_DEBUG_NAME(vertexBufferDescriptor, "Vertex buffer");

				vertexBuffer = createBuffer<Vertex>(vertexBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->getSize();

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Buffer copy command buffer");

				const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				transferQueue->submit({commandBuffer});
				transferQueue->waitIdle();
			}

		private:
			ref<RenderPass> renderPass;
			ref<Image> image;
			ref<ImageView> imageView;
			sequence<ref<Framebuffer>> framebuffers;
			ref<GraphicsPipeline> graphicsPipeline;
			ref<Buffer<Vertex>> vertexBuffer;
	};

	DEFINE_SAMPLE_LAYER(SampleMultisampling);
}