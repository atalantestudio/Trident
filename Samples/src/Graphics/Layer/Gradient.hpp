// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class GradientSampleRenderer : public Renderer {
		private:
			// TODO(tsl)
			static constexpr bool LOAD_TSL_SHADER_MODULE = false;

		public:
			void build() override {
				pipelineLayout = createPipelineLayout();
				pipeline = createPipeline();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D extent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(extent.x);
				const float32 height = static_cast<float32>(extent.y);

				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindGraphicsPipeline(pipeline);
				commandEncoder->draw(3, 1, 0, 0);
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
				const ShaderModuleDescriptor vertexShaderModuleDescriptor = {
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex",
					#endif
				};

				ref<ShaderModule> vertexShaderModule;

				if (getGraphicsApi() == GraphicsAPI::GL && LOAD_TSL_SHADER_MODULE) {
					const ref<ShaderModuleAsset> vertexShaderModuleAsset = getLayer()->getApplication()->getAssetLibrary()->getAsset<ShaderModuleAsset>("Screen");

					vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, vertexShaderModuleAsset);
				}
				else {
					vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Sample/Gradient/Gradient.vert");
				}

				const ColorBlendAttachment colorBlendAttachments[] = {
					{
						.blend = false,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = pipelineLayout,
					.vertexInputBindings = {},
					.vertexShaderModule = vertexShaderModule,
					.fragmentShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Fragment",
							#endif
						},
						"Sample/Gradient/Gradient.frag"
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
						.name = "Gradient",
					#endif
				});
			}

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> pipeline;
	};

	DEFINE_SAMPLE_LAYER(GradientSample);
}