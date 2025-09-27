#pragma once

#include "ProjectA.hpp"

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TriangleIndexedRenderer : public Renderer {
		public:
			void build() override {
				// 1. Create render pass and subpass.
				{
					const ref<Surface> surface = getSurface();

					ColorAttachment colorAttachment{};
					colorAttachment.format = surface->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;
					colorAttachment.finalLayout = ImageLayout::PRESENT_SRC;

					SubpassDescriptor subpassDescriptor{};
					subpassDescriptor.colorAttachments = {colorAttachment};
					subpassDescriptor.depthStencil = false;

					subpass = createSubpass(subpassDescriptor);

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.subpasses = {subpass};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 2. Create framebuffers.
				recreateFramebuffers();

				// 3. Create pipeline layout.
				{
					const PipelineLayoutDescriptor pipelineLayoutDescriptor;

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 4. Create render pipeline.
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
					renderPipelineDescriptor.renderPass = renderPass;
					renderPipelineDescriptor.subpassIndex = 0;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}

				ref<Buffer> vertexStagingBuffer;

				// 5. Create vertex buffer.
				{
					static constexpr uint64 vertexBufferSize = 3 * sizeof(Vertex);

					vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

					vertexMappedRange[0].vertex = {-0.5f, -0.5f};
					vertexMappedRange[0].color = {1.0f, 0.0f, 0.0f};
					vertexMappedRange[1].vertex = {0.0f, 0.5f};
					vertexMappedRange[1].color = {0.0f, 1.0f, 0.0f};
					vertexMappedRange[2].vertex = {0.5f, -0.5f};
					vertexMappedRange[2].color = {0.0f, 0.0f, 1.0f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor{};
					vertexBufferDescriptor.size = vertexBufferSize;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
					vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
					vertexBufferDescriptor.persistent = false;

					vertexBuffer = createBuffer(vertexBufferDescriptor);
				}

				ref<Buffer> indexStagingBuffer;

				// 6. Create index buffer.
				{
					static constexpr uint64 indexBufferSize = 3 * sizeof(uint16);

					indexStagingBuffer = createStagingBuffer(indexBufferSize);

					uint16* indexStagingBufferRange = indexStagingBuffer->map<uint16>(0, indexBufferSize);
					indexStagingBufferRange[0] = 0;
					indexStagingBufferRange[1] = 1;
					indexStagingBufferRange[2] = 2;

					indexStagingBuffer->unmap();

					BufferDescriptor indexBufferDescriptor{};
					indexBufferDescriptor.size = indexBufferSize;
					indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
					indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
					indexBufferDescriptor.persistent = false;

					indexBuffer = createBuffer(indexBufferDescriptor);
				}

				// 7. Write to vertex and index buffers.
				{
					const ref<Device> device = getDevice();

					sequence<BufferCopyRegion> vertexBufferCopyRegions(1);

					BufferCopyRegion& vertexBufferCopyRegion = vertexBufferCopyRegions[0];
					vertexBufferCopyRegion.sourceOffset = 0;
					vertexBufferCopyRegion.destinationOffset = 0;
					vertexBufferCopyRegion.size = vertexStagingBuffer->size;

					sequence<BufferCopyRegion> indexBufferCopyRegions(1);

					BufferCopyRegion& indexBufferCopyRegion = indexBufferCopyRegions[0];
					indexBufferCopyRegion.sourceOffset = 0;
					indexBufferCopyRegion.destinationOffset = 0;
					indexBufferCopyRegion.size = indexStagingBuffer->size;

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
					const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, vertexBufferCopyRegions);
					commandEncoder->copyBufferToBuffer(indexStagingBuffer, indexBuffer, indexBufferCopyRegions);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

					device->waitIdle();
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
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtent());
				commandEncoder->drawIndexed(3, 1, 0, 0, 0);
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
				vec3<float32> color;
			};

		private:
			ref<Subpass> subpass;
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
	};
}