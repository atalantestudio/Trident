#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class DepthTestRenderer : public Renderer {
		public:
			DepthTestRenderer() {
				// 1. Create render pass.
				{
					const ref<Surface> surface = getSurface();

					ColorAttachment colorAttachment{};
					colorAttachment.format = surface->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;

					DepthStencilAttachment depthStencilAttachment{};
					depthStencilAttachment.format = ImageFormat::D32_SFLOAT;
					depthStencilAttachment.sampleCount = 1;
					depthStencilAttachment.loadOperation = LoadOperation::CLEAR;

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.colorAttachments = {colorAttachment};
					renderPassDescriptor.depthStencilAttachment = depthStencilAttachment;
					renderPassDescriptor.resolve = false;
					renderPassDescriptor.depthStencil = true;
					renderPassDescriptor.presentAttachmentIndex = 0;

					renderPass = createRenderPass(renderPassDescriptor);

					updateDepthStencilAttachment();

					const ref<Swapchain>& swapchain = getSwapchain();

					swapchain->getImages(renderPass);
				}

				// 2. Create pipeline layout.
				{
					const PipelineLayoutDescriptor pipelineLayoutDescriptor;

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 3. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<vec3<float32>>();

					VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
					instanceInputBinding.registerAttribute<vec3<float32>>();

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
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Base/Vertex3DRGB.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Base/ColorRGB.frag", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					renderPipelineDescriptor.cullMode = CullMode::BACK;
					renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
					renderPipelineDescriptor.lineWidth = 1.0f;
					renderPipelineDescriptor.minSampleShading = 0.0f;
					renderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
					renderPipelineDescriptor.depthStencilState.depthTest = true;
					renderPipelineDescriptor.depthStencilState.depthWrite = true;
					renderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
					renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}

				// 4. Create vertex buffers.
				{
					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(VERTEX_BUFFER_SIZE);

					Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, VERTEX_BUFFER_SIZE);

					vertexMappedRange[0].vertex = {-0.75f,  0.75f,  1.0f};
					vertexMappedRange[1].vertex = { 0.25f,  0.75f,  1.0f};
					vertexMappedRange[2].vertex = {-0.75f, -0.75f,  1.0f};
					vertexMappedRange[3].vertex = {-0.5f,   0.5f,   0.0f};
					vertexMappedRange[4].vertex = { 0.5f,   0.5f,   0.0f};
					vertexMappedRange[5].vertex = {-0.5f,  -0.5f,   0.0f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					vertexBuffer = createBuffer(vertexBufferDescriptor);

					const ref<Buffer> instanceStagingBuffer = createStagingBuffer(INSTANCE_BUFFER_SIZE);

					Instance* instances = instanceStagingBuffer->map<Instance>(0, INSTANCE_BUFFER_SIZE);

					instances[0].color = {1.0f, 1.0f, 1.0f};
					instances[1].color = {1.0f, 0.2f, 0.0f};

					instanceStagingBuffer->unmap();

					BufferDescriptor instanceBufferDescriptor;
					instanceBufferDescriptor.size = INSTANCE_BUFFER_SIZE;
					instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					instanceBuffer = createBuffer(instanceBufferDescriptor);

					const ref<Device> device = getDevice();

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, 0, 0, VERTEX_BUFFER_SIZE);
					commandEncoder->copyBufferToBuffer(instanceStagingBuffer, instanceBuffer, 0, 0, INSTANCE_BUFFER_SIZE);

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
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};
				beginRenderPassDescriptor.imageIndex = getImageIndex();

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

			void resize(WindowResizeEvent& event) override {
				updateDepthStencilAttachment();

				const ref<Swapchain>& swapchain = getSwapchain();

				swapchain->getImages(renderPass);
			}

		private:
			void updateDepthStencilAttachment() {
				const ref<Swapchain>& swapchain = getSwapchain();

				AttachmentImageDescriptor depthStencilAttachmentImageDescriptor{};
				depthStencilAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
				depthStencilAttachmentImageDescriptor.width = swapchain->getExtentWidth();
				depthStencilAttachmentImageDescriptor.height = swapchain->getExtentHeight();
				depthStencilAttachmentImageDescriptor.depth = 1;
				depthStencilAttachmentImageDescriptor.mipLevelCount = 1;
				depthStencilAttachmentImageDescriptor.layerCount = 1;

				depthStencilImage = renderPass->createDepthStencilAttachmentImage(depthStencilAttachmentImageDescriptor);

				constexpr ImageViewDescriptor depthStencilImageViewDescriptor;

				depthStencilImageView = depthStencilImage->createView(depthStencilImageViewDescriptor);

				renderPass->setDepthStencilAttachmentImageView(depthStencilImageView);
			}

		private:
			struct Vertex {
				vec3<float32> vertex;
			};

			struct Instance {
				vec3<float32> color;
			};

		private:
			static constexpr uint64 INSTANCE_COUNT = 2;
			static constexpr uint64 VERTEX_BUFFER_SIZE = 3 * INSTANCE_COUNT * sizeof(Vertex);
			static constexpr uint64 INSTANCE_BUFFER_SIZE = INSTANCE_COUNT * sizeof(Instance);

		private:
			ref<RenderPass> renderPass;

			ref<Image> depthStencilImage;
			ref<ImageView> depthStencilImageView;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> instanceBuffer;
	};
}