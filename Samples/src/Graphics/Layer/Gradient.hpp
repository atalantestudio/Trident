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
				renderPass = createRenderPass();
				pipelineLayout = createPipelineLayout();
				pipeline = createPipeline();

				createFramebuffers();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const ref<Framebuffer> framebuffer = framebuffers[getSwapchainImageIndex()];
				const Extent2D extent = framebuffer->getExtent();
				const float32 width = static_cast<float32>(extent.width);
				const float32 height = static_cast<float32>(extent.height);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffer,
					.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}},
				});
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, extent);
				commandEncoder->bindGraphicsPipeline(pipeline);
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				createFramebuffers();
			}

		private:
			ref<RenderPass> createRenderPass() const {
				return Renderer::createRenderPass({
					.attachments = {
						{
							.format = getSurface()->getFormat(),
							.sampleCount = 1,
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
				});
			}

			void createFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor = {
					.renderPass = renderPass,
					.imageViews = {nullptr},
					.extent = swapchain->getExtent(),
				};

				#if defined(DEBUG_OBJECT_NAME)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

				while (framebufferIndex < framebuffers.count()) {
					#if defined(DEBUG_OBJECT_NAME)
						name = Logger::format("In flight []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

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

				const sequence<ColorBlendAttachment> colorBlendAttachments = {
					{
						.blend = false,
					},
				};

				return createGraphicsPipeline({
					.pipelineLayout = pipelineLayout,
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
					.renderPass = renderPass,
					.subpassIndex = 0,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Gradient",
					#endif
				});
			}

		private:
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<PipelineLayout> pipelineLayout;
			ref<GraphicsPipeline> pipeline;
	};

	DEFINE_SAMPLE_LAYER(GradientSample);
}