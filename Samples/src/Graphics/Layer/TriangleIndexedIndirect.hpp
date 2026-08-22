// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class TriangleIndexedIndirectSampleRenderer : public Renderer {
		private:
			struct Vertex {
				svec2<float32> position;
				vec3<float32> color;
			};

		public:
			explicit TriangleIndexedIndirectSampleRenderer() {}

			~TriangleIndexedIndirectSampleRenderer() {
				vertexBuffer.~ref();
				indexBuffer.~ref();
				indirectBuffer.~ref();
				stagingBuffer.~ref();
			}

			void build() override {
				pipelineLayout = createPipelineLayout();
				pipeline = createPipeline();
				vertexBuffer = createVertexBuffer();
				indexBuffer = createIndexBuffer();
				indirectBuffer = createIndirectBuffer();
				stagingBuffer = createStagingBuffer();
			}

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override {
				return {
					.buffers = staticBuffers,
					.images = {},
				};
			}

			void encodeOneTimeCommands() const override {
				const BufferView<Vertex> vertexView = stagingBuffer->createView<Vertex>(0, vertexBuffer->getSize());
				const BufferView<uint16> indexView = stagingBuffer->createView<uint16>(vertexView.getOffset() + vertexView.getSize(), indexBuffer->getSize());
				const BufferView<DrawIndexedIndirectCommand> indirectView = stagingBuffer->createView<DrawIndexedIndirectCommand>(indexView.getOffset() + indexView.getSize(), indirectBuffer->getSize());

				Vertex* vertexMappedRange = vertexView.map();
				vertexMappedRange[0] = {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}};
				vertexMappedRange[1] = {{ 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}};
				vertexMappedRange[2] = {{ 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}};

				uint16* indexMappedRange = indexView.map();
				indexMappedRange[0] = 0;
				indexMappedRange[1] = 1;
				indexMappedRange[2] = 2;

				DrawIndexedIndirectCommand* indirectMappedRange = indirectView.map();
				indirectMappedRange[0].indexCount = 3;
				indirectMappedRange[0].instanceCount = 1;
				indirectMappedRange[0].indexOffset = 0;
				indirectMappedRange[0].vertexBias = 0;
				indirectMappedRange[0].instanceOffset = 0;

				stagingBuffer->unmap();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "One-ti!e commands",
					#endif
				});

				commandEncoder->begin();

				const BufferCopyRegion bufferCopyRegions[] = {
					{
						.sourceOffset = vertexView.getOffset(),
						.destinationOffset = 0,
						.size = vertexView.getSize(),
					},
					{
						.sourceOffset = indexView.getOffset(),
						.destinationOffset = 0,
						.size = indexView.getSize(),
					},
					{
						.sourceOffset = indirectView.getOffset(),
						.destinationOffset = 0,
						.size = indirectView.getSize(),
					},
				};

				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegions[0]);
				commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, bufferCopyRegions[1]);
				commandEncoder->copyBufferToBuffer(stagingBuffer, indirectBuffer, bufferCopyRegions[2]);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D extent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(extent.width);
				const float32 height = static_cast<float32>(extent.height);

				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindGraphicsPipeline(pipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
				commandEncoder->drawIndexedIndirect(indirectBuffer, 0, 1, 0);
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
				VertexInputBinding vertexInputBindings[] = {
					VertexInputBinding(InputRate::VERTEX),
				};

				vertexInputBindings[0].registerAttribute<svec2<float32>>();
				vertexInputBindings[0].registerAttribute<vec3<float32>>();

				const ColorBlendAttachment colorBlendAttachments[] = {
					{
						.blend = false,
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
								.name = "Triangle vertex",
							#endif
						},
						"Sample/Triangle/Triangle.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Triangle fragment",
							#endif
						},
						"Sample/Triangle/Triangle.frag"
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
					.size = sizeof(Vertex) * 3,
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				});
			}

			ref<Buffer> createIndexBuffer() const {
				return createStaticBuffer({
					.size = sizeof(uint16) * 3,
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Index",
					#endif
				});
			}

			ref<Buffer> createIndirectBuffer() const {
				return createStaticBuffer({
					.size = sizeof(DrawIndexedIndirectCommand),
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDIRECT,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Indirect",
					#endif
				});
			}

			ref<Buffer> createStagingBuffer() const {
				return createStaticBuffer({
					.size = vertexBuffer->getSize() + indexBuffer->getSize() + indirectBuffer->getSize(),
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

			union {
				struct {
					ref<Buffer> vertexBuffer;
					ref<Buffer> indexBuffer;
					ref<Buffer> indirectBuffer;
					ref<Buffer> stagingBuffer;
				};

				ref<Buffer> staticBuffers[4];
			};
	};

	DEFINE_SAMPLE_LAYER(TriangleIndexedIndirectSample);
}