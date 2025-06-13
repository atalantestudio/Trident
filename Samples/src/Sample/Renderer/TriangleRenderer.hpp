#pragma once

#include "ProjectA.hpp"

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TriangleRenderer : public Renderer {
		public:
			TriangleRenderer() {
				// 1. Create render pass.
				{
					const ref<Surface> surface = getSurface();

					ColorAttachment colorAttachment{};
					colorAttachment.format = surface->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.colorAttachments = {colorAttachment};
					renderPassDescriptor.resolve = false;
					renderPassDescriptor.depthStencil = false;
					renderPassDescriptor.presentAttachmentIndex = 0;

					renderPass = createRenderPass(renderPassDescriptor);

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
					vertexInputBinding.registerAttribute<svec2<float32>>();
					vertexInputBinding.registerAttribute<vec3<float32>>();

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
					renderPipelineDescriptor.pipelineLayout = pipelineLayout;
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Base/Vertex2DRGB.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Base/ColorRGB.frag", fragmentShaderModuleDescriptor);
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

				// 4. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 3 * sizeof(Vertex);

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

					vertexMappedRange[0].vertex = {-0.5f, -0.5f};
					vertexMappedRange[0].color = {1.0f, 0.0f, 0.0f};
					vertexMappedRange[1].vertex = {0.0f, 0.5f};
					vertexMappedRange[1].color = {0.0f, 1.0f, 0.0f};
					vertexMappedRange[2].vertex = {0.5f, -0.5f};
					vertexMappedRange[2].color = {0.0f, 0.0f, 1.0f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = vertexBufferSize;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					vertexBuffer = createBuffer(vertexBufferDescriptor);

					const ref<Device> device = getDevice();

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, 0, 0, vertexBufferSize);

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
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};
				beginRenderPassDescriptor.imageIndex = getImageIndex();

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
				commandEncoder->draw(3, 1, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				const ref<Swapchain>& swapchain = getSwapchain();

				swapchain->getImages(renderPass);
			}

		private:
			struct Vertex {
				svec2<float32> vertex;
				vec3<float32> color;
			};

		private:
			ref<RenderPass> renderPass;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
	};
}