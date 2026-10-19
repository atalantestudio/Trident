// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class ColorBlendLayerGroup : public RenderPassLayerGroup {
		public:
			explicit ColorBlendLayerGroup(range<uint32, uint32> layerRange) :
				RenderPassLayerGroup(layerRange)
			{}

			RenderPassDescriptor getRenderPassDescriptor() const override {
				return {
					.attachments = {
						{
							.format = Renderer::getSurface()->getFormat(),
							.sampleCount = ImageSampleCount::x1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::STORE,
							.finalLayout = ImageLayout::PRESENT_SRC,
						},
					},
					.subpasses = {
						{
							.colorAttachmentReferences = {
								{
									.index = 0,
									.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
								},
							},
						},
					},
				};
			}

			sequence<ClearValue> getClearValues() const override {
				return {
					{1.0f, 1.0f, 1.0f, 1.0f},
				};
			}
	};

	class ColorBlendSampleRenderer : public Renderer {
		private:
			struct Vertex {
				vec2<float32> position;
			};

			struct Instance {
				vec4<float32> color;
				vec2<float32> translation;
			};

		private:
			static constexpr uint64 VERTEX_SIZE = sizeof(Vertex) * 3;
			static constexpr uint64 INSTANCE_SIZE = sizeof(Instance) * 3;

		public:
			void build() override {
				pipelineLayout = createPipelineLayout();
				pipeline = createPipeline();
				staticBuffers[0] = createVertexBuffer();
				staticBuffers[1] = createStagingBuffer();
			}

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override {
				return {
					.buffers = staticBuffers,
					.images = {},
				};
			}

			void encodeOneTimeCommands() const override {
				const BufferView<Vertex> vertexView = staticBuffers[1]->createView<Vertex>(0, VERTEX_SIZE);
				const BufferView<Instance> instanceView = staticBuffers[1]->createView<Instance>(vertexView.getOffset() + vertexView.getSize(), INSTANCE_SIZE);

				Vertex* vertexMappedRange = vertexView.map();
				vertexMappedRange[0] = {{-0.5f, -0.5f}};
				vertexMappedRange[1] = {{ 0.0f,  0.5f}};
				vertexMappedRange[2] = {{ 0.5f, -0.5f}};

				Instance* instanceMappedRange = instanceView.map();
				instanceMappedRange[0] = {Color(0x8e1a237f).value, {-0.1f, -0.1f}};
				instanceMappedRange[1] = {Color(0x80b5e77f).value, { 0.0f,  0.0f}};
				instanceMappedRange[2] = {Color(0xeda0417f).value, { 0.1f,  0.1f}};

				staticBuffers[1]->unmap();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "One-time commands",
					#endif
				});

				commandEncoder->begin();

				const BufferCopyRegion bufferCopyRegion = {
					.sourceOffset = 0,
					.destinationOffset = 0,
					.size = staticBuffers[0]->getSize(),
				};

				commandEncoder->copyBufferToBuffer(staticBuffers[1], staticBuffers[0], bufferCopyRegion);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D extent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(extent.x);
				const float32 height = static_cast<float32>(extent.y);

				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindGraphicsPipeline(pipeline);
				commandEncoder->bindVertexBuffer(0, staticBuffers[0], 0);
				commandEncoder->bindVertexBuffer(1, staticBuffers[0], VERTEX_SIZE);
				commandEncoder->draw(3, 3, 0, 0);
			}

		private:
			ref<PipelineLayout> createPipelineLayout() const {
				return Renderer::createPipelineLayout({
					.descriptorSetLayouts = {},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Pipeline layout",
					#endif
				});
			}

			ref<GraphicsPipeline> createPipeline() const {
				VertexInputBinding vertexInputBindings[2] = {
					VertexInputBinding(InputRate::VERTEX),
					VertexInputBinding(InputRate::INSTANCE),
				};

				vertexInputBindings[0].registerAttribute<decltype(Vertex::position)>();
				vertexInputBindings[1].registerAttribute<decltype(Instance::color)>();
				vertexInputBindings[1].registerAttribute<decltype(Instance::translation)>();

				const ColorBlendAttachment colorBlendAttachments[] = {
					{
						.blend = true,
						.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
						.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = pipelineLayout,
					.vertexInputBindings = vertexInputBindings,
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Vertex",
							#endif
						},
						"Sample/ColorBlend/ColorBlend.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Fragment",
							#endif
						},
						"Sample/ColorBlend/ColorBlend.frag"
					),
					.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
					.polygonMode = PolygonMode::FILL,
					.cullMode = CullMode::BACK,
					.frontFace = FrontFace::CLOCKWISE,
					.lineWidth = 1.0f,
					.minSampleShading = 0.0f,
					.colorBlendState = {
						.attachments = colorBlendAttachments,
					},
					.depthStencilState = {
						.depthTest = false,
					},
					.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
					.renderPass = pointer_cast<const RenderPassLayer*>(getLayer())->renderPass,
					.subpassIndex = 0,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Pipeline",
					#endif
				});
			}

			ref<Buffer> createVertexBuffer() const {
				return createStaticBuffer({
					.size = VERTEX_SIZE + INSTANCE_SIZE,
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

			ref<Buffer> createStagingBuffer() const {
				return createStaticBuffer({
					.size = staticBuffers[0]->getSize(),
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
			}

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> pipeline;
			ref<Buffer> staticBuffers[2];
	};

	DEFINE_SAMPLE_LAYER(ColorBlendSample);
}