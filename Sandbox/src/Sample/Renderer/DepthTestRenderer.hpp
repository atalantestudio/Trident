#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class DepthTestRenderer : public Renderer {
		public:
			DepthTestRenderer() {
				// 1. Create render pass.
				{
					Attachment colorAttachment{};
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;

					DepthStencilAttachment depthStencilAttachment{};
					depthStencilAttachment.enabled = true;
					depthStencilAttachment.sampleCount = 1;
					depthStencilAttachment.loadOperation = LoadOperation::CLEAR;

					RenderPassDescriptor renderPassDescriptor{};
					renderPassDescriptor.colorAttachments = {colorAttachment};
					renderPassDescriptor.depthStencilAttachment = depthStencilAttachment;

					renderPass = createRenderPass(renderPassDescriptor);

					/* Example G-Buffer render pass:

					Attachment positionColorAttachment{};
					positionColorAttachment.sampleCount = 1;
					positionColorAttachment.loadOperation = LoadOperation::CLEAR;
					positionColorAttachment.storeOperation = StoreOperation::STORE;

					Attachment normalColorAttachment{};
					normalColorAttachment.sampleCount = 1;
					normalColorAttachment.loadOperation = LoadOperation::CLEAR;
					normalColorAttachment.storeOperation = StoreOperation::STORE;

					Attachment textureCoordinateColorAttachment{};
					textureCoordinateColorAttachment.sampleCount = 1;
					textureCoordinateColorAttachment.loadOperation = LoadOperation::CLEAR;
					textureCoordinateColorAttachment.storeOperation = StoreOperation::STORE;

					Attachment depthStencilAttachment{};
					depthStencilAttachment.sampleCount = 1;
					depthStencilAttachment.loadOperation = LoadOperation::CLEAR;
					depthStencilAttachment.storeOperation = StoreOperation::STORE;

					RenderPassDescriptor renderPassDescriptor{};
					renderPassDescriptor.colorAttachments = {positionColorAttachment, normalColorAttachment, textureCoordinateColorAttachment};
					renderPassDescriptor.depthStencilAttachment = depthStencilAttachment;
					renderPassDescriptor.enableDepthStencil = true;

					renderPass = createRenderPass(renderPassDescriptor);

					*/
				}

				createAttachments();

				// 2. Create swapchain.
				{
					SwapchainDescriptor swapchainDescriptor;
					swapchainDescriptor.renderPass = renderPass;
					swapchainDescriptor.extentWidth = Application::get().getWindow().getSize()[0];
					swapchainDescriptor.extentHeight = Application::get().getWindow().getSize()[1];

					swapchain = createSwapchain(swapchainDescriptor);
				}

				// 3. Create pipeline layout.
				{
					const PipelineLayoutDescriptor pipelineLayoutDescriptor;

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 4. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec3<float32>>(false);

					VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
					instanceInputBinding.registerAttribute<vec4<float32>>(false);

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
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Vertex.vert.spv", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Color.frag.spv", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}

				// 5. Create vertex buffers.
				{
					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(VERTEX_BUFFER_SIZE);

					Vertex* vertices = vertexStagingBuffer->map<Vertex>(0, VERTEX_BUFFER_SIZE);

					vertices[0].position = {-0.75f,  0.75f,  1.0f};
					vertices[1].position = { 0.25f,  0.75f,  1.0f};
					vertices[2].position = {-0.75f, -0.75f,  1.0f};
					vertices[3].position = {-0.5f,   0.5f,   0.0f};
					vertices[4].position = { 0.5f,   0.5f,   0.0f};
					vertices[5].position = {-0.5f,  -0.5f,   0.0f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

					vertexBuffer = createBuffer(vertexBufferDescriptor);

					const ref<Buffer> instanceStagingBuffer = createStagingBuffer(INSTANCE_BUFFER_SIZE);

					Instance* instances = instanceStagingBuffer->map<Instance>(0, INSTANCE_BUFFER_SIZE);

					instances[0].color = {1.0f, 1.0f, 1.0f, 1.0f};
					instances[1].color = {1.0f, 0.2f, 0.0f, 1.0f};

					instanceStagingBuffer->unmap();

					BufferDescriptor instanceBufferDescriptor;
					instanceBufferDescriptor.size = INSTANCE_BUFFER_SIZE;
					instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

					instanceBuffer = createBuffer(instanceBufferDescriptor);

					const ref<Device> device = getDevice();

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, 0, 0, VERTEX_BUFFER_SIZE);
					commandEncoder->copyBufferToBuffer(instanceStagingBuffer, instanceBuffer, 0, 0, INSTANCE_BUFFER_SIZE);

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
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};
				beginRenderPassDescriptor.imageIndex = acquireNextImage(swapchain);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());

				for (uint32 instanceIndex = 0; instanceIndex < INSTANCE_COUNT; instanceIndex += 1) {
					const uint32 vertexOffset = 3 * static_cast<uint32>(instanceIndex);

					commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance), sizeof(Instance));
					commandEncoder->draw(3, 1, vertexOffset);
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) final override {
				const ref<Device> device = getDevice();

				device->waitIdle();

				createAttachments();

				SwapchainDescriptor swapchainDescriptor;
				swapchainDescriptor.renderPass = renderPass;
				swapchainDescriptor.extentWidth = event.getWidth();
				swapchainDescriptor.extentHeight = event.getHeight();

				swapchain->recreate(swapchainDescriptor);
			}

		private:
			void createAttachments() {
				ImageDescriptor depthStencilImageDescriptor{};
				depthStencilImageDescriptor.type = ImageType::TEXTURE_2D;
				depthStencilImageDescriptor.format = ImageFormat::D32_SFLOAT;
				depthStencilImageDescriptor.usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT;
				depthStencilImageDescriptor.width = Application::get().getWindow().getSize()[0];
				depthStencilImageDescriptor.height = Application::get().getWindow().getSize()[1];
				depthStencilImageDescriptor.depth = 1;
				depthStencilImageDescriptor.mipLevelCount = 1;
				depthStencilImageDescriptor.arrayLayerCount = 1;
				depthStencilImageDescriptor.sampleCount = 1;

				depthStencilImage = createImage(depthStencilImageDescriptor);

				constexpr ImageViewDescriptor depthStencilImageViewDescriptor;

				const ref<ImageView> depthStencilImageView = depthStencilImage->createView(depthStencilImageViewDescriptor);

				renderPass->setAttachmentImageView(1, depthStencilImageView);
			}

		private:
			struct Vertex {
				svec3<float32> position;
			};

			struct Instance {
				vec4<float32> color;
			};

		private:
			static constexpr uint64 INSTANCE_COUNT = 2;
			static constexpr uint64 VERTEX_BUFFER_SIZE = 3 * INSTANCE_COUNT * sizeof(Vertex);
			static constexpr uint64 INSTANCE_BUFFER_SIZE = INSTANCE_COUNT * sizeof(Instance);

		private:
			ref<Image> depthStencilImage;

			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> instanceBuffer;
	};
}