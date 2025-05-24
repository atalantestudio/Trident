#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class MultisampleRenderer : public Renderer {
		public:
			MultisampleRenderer() {
				// 1. Create swapchain.
				{
					SwapchainDescriptor swapchainDescriptor;
					swapchainDescriptor.extentWidth = Application::get().getWindow().getSize()[0];
					swapchainDescriptor.extentHeight = Application::get().getWindow().getSize()[1];

					swapchain = createSwapchain(swapchainDescriptor);
				}

				// 2. Create render pass.
				{
					ColorAttachment multisampleColorAttachment{};
					multisampleColorAttachment.format = getSurface()->getFormat();
					multisampleColorAttachment.sampleCount = 4;
					multisampleColorAttachment.loadOperation = LoadOperation::CLEAR;
					multisampleColorAttachment.storeOperation = StoreOperation::STORE;

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.colorAttachments = {multisampleColorAttachment};
					renderPassDescriptor.resolve = true;
					renderPassDescriptor.depthStencil = false;
					renderPassDescriptor.presentAttachmentIndex = 0;

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 3. Create multisample image attachment.
				updateMultisampleAttachment();

				swapchain->getImages(renderPass);

				// 4. Create pipeline layout.
				{
					const PipelineLayoutDescriptor pipelineLayoutDescriptor;

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 5. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>(false);
					vertexInputBinding.registerAttribute<svec3<float32>>(false);

					ShaderModuleDescriptor vertexShaderModuleDescriptor{};
					vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
					vertexShaderModuleDescriptor.entryPoint = "main";

					ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
					fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
					fragmentShaderModuleDescriptor.entryPoint = "main";

					RenderPipelineDescriptor renderPipelineDescriptor;
					renderPipelineDescriptor.renderPass = renderPass;
					renderPipelineDescriptor.pipelineLayout = pipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Triangle.vert.spv", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Triangle.frag.spv", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					renderPipelineDescriptor.depthStencilState.depthTest = false;
					renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}

				// 6. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 3 * sizeof(Vertex);

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					Vertex* vertices = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

					vertices[0].position = {-0.5f, -0.5f};
					vertices[0].color = {1.0f, 0.0f, 0.0f};
					vertices[1].position = {0.0f, 0.5f};
					vertices[1].color = {0.0f, 1.0f, 0.0f};
					vertices[2].position = {0.5f, -0.5f};
					vertices[2].color = {0.0f, 0.0f, 1.0f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = vertexBufferSize;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

					vertexBuffer = createBuffer(vertexBufferDescriptor);

					const ref<Device> device = getDevice();

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
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};
				beginRenderPassDescriptor.imageIndex = acquireNextImage(swapchain);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
				commandEncoder->draw(3, 1, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				const ref<Device> device = getDevice();

				device->waitIdle();

				updateMultisampleAttachment();

				SwapchainDescriptor swapchainDescriptor;
				swapchainDescriptor.extentWidth = event.getWidth();
				swapchainDescriptor.extentHeight = event.getHeight();

				swapchain->update(swapchainDescriptor);
				swapchain->getImages(renderPass);
			}

		private:
			void updateMultisampleAttachment() {
				AttachmentImageDescriptor multisampleAttachmentImageDescriptor{};
				multisampleAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
				multisampleAttachmentImageDescriptor.width = Application::get().getWindow().getSize()[0];
				multisampleAttachmentImageDescriptor.height = Application::get().getWindow().getSize()[1];
				multisampleAttachmentImageDescriptor.depth = 1;
				multisampleAttachmentImageDescriptor.mipLevelCount = 1;
				multisampleAttachmentImageDescriptor.arrayLayerCount = 1;

				multisampleImage = renderPass->createColorAttachmentImage(multisampleAttachmentImageDescriptor, 0);

				constexpr ImageViewDescriptor multisampleImageViewDescriptor;

				multisampleImageView = multisampleImage->createView(multisampleImageViewDescriptor);

				renderPass->setColorAttachmentImageView(multisampleImageView, 0);
			}

		private:
			struct Vertex {
				svec2<float32> position;
				svec3<float32> color;
			};

		private:
			ref<Image> multisampleImage;
			ref<ImageView> multisampleImageView;

			ref<Swapchain> swapchain;

			ref<RenderPass> renderPass;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
	};
}