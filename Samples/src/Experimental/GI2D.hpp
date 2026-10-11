// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../Graphics/GraphicsLayer.hpp"

namespace Trident {
	class GI2DExperimentalRenderer : public Renderer {
		public:
			void build() override {
				pipelineLayout = createPipelineLayout();
				vertexGenerationPipeline = createVertexGenerationPipeline();
				globalIlluminationPipeline = createGlobalIlluminationPipeline();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D extent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(extent.x);
				const float32 height = static_cast<float32>(extent.y);

				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);

				commandEncoder->bindGraphicsPipeline(globalIlluminationPipeline);
				commandEncoder->draw(36, 1, 0, 0);
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

			ref<ComputePipeline> createVertexGenerationPipeline() const {
				return createComputePipeline({
					.pipelineLayout = pipelineLayout,
					.computeShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::COMPUTE,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Vertex generation compute",
							#endif
						},
						"Experimental/GI2D/VertexGeneration.comp"
					),
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex generation",
					#endif
				});
			}

			ref<GraphicsPipeline> createGlobalIlluminationPipeline() const {
				const ColorBlendAttachment colorBlendAttachments[] = {
					{
						.blend = false,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = pipelineLayout,
					.vertexInputBindings = {},
					.vertexShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::VERTEX,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Global illumination vertex",
							#endif
						},
						"Experimental/GI2D/GlobalIllumination.vert"
					),
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Global illumination fragment",
							#endif
						},
						"Experimental/GI2D/GlobalIllumination.frag"
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
						.name = "Global illumination",
					#endif
				});
			}

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<ComputePipeline> vertexGenerationPipeline;
			ref<GraphicsPipeline> globalIlluminationPipeline;
	};

	DEFINE_SAMPLE_LAYER(GI2DExperimental);
}