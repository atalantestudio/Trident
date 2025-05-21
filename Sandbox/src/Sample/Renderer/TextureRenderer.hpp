#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextureRenderer : public Renderer {
		public:
			TextureRenderer() :
				font("../ProjectA/src/Asset/Font/test.ttf")
			{
				const ref<Device> device = getDevice();
				const ref<Image>& image = font.getSdfImage();

				// 1. Create image view.
				{
					constexpr ImageViewDescriptor imageViewDescriptor;

					imageView = image->createView(imageViewDescriptor);
				}

				// 2. Create image sampler.
				{
					SamplerDescriptor samplerDescriptor{};
					samplerDescriptor.magFilter = SamplerFilter::NEAREST;
					samplerDescriptor.minFilter = SamplerFilter::NEAREST;

					sampler = createSampler(samplerDescriptor);
				}

				// 3. Create render pass.
				{
					Attachment colorAttachment{};
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.colorAttachments = {colorAttachment};

					renderPass = createRenderPass(renderPassDescriptor);
				}

				// 4. Create swapchain.
				{
					SwapchainDescriptor swapchainDescriptor;
					swapchainDescriptor.renderPass = renderPass;
					swapchainDescriptor.extentWidth = Application::get().getWindow().getSize()[0];
					swapchainDescriptor.extentHeight = Application::get().getWindow().getSize()[1];

					swapchain = createSwapchain(swapchainDescriptor);
				}

				// 5. Create pipeline layout.
				{
					DescriptorSetLayoutBinding imageDescriptorSetLayoutBinding;
					imageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
					imageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
					imageDescriptorSetLayoutBinding.imageViews = {imageView};
					imageDescriptorSetLayoutBinding.samplers = {sampler};

					DescriptorSetLayout descriptorSetLayout;
					descriptorSetLayout.bindings = {imageDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor;
					pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 6. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>(false);

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
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Texture.vert.spv", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Texture.frag.spv", fragmentShaderModuleDescriptor);
					renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
					renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

					renderPipeline = createRenderPipeline(renderPipelineDescriptor);
				}

				// 7. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 3 * sizeof(Vertex);

					const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

					Vertex* vertices = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

					vertices[0].position = {-1.0f,  1.0f};
					vertices[1].position = { 3.0f,  1.0f};
					vertices[2].position = {-1.0f, -3.0f};

					vertexStagingBuffer->unmap();

					BufferDescriptor vertexBufferDescriptor{};
					vertexBufferDescriptor.size = vertexBufferSize;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

					vertexBuffer = createBuffer(vertexBufferDescriptor);

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

			void render(const ref<CommandEncoder>& commandEncoder) const final override {
				const float32 width = static_cast<float32>(swapchain->getExtentWidth());
				const float32 height = static_cast<float32>(swapchain->getExtentHeight());

				BeginRenderPassDescriptor beginRenderPassDescriptor;
				beginRenderPassDescriptor.renderPass = renderPass;
				beginRenderPassDescriptor.swapchain = swapchain;
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};
				beginRenderPassDescriptor.imageIndex = acquireNextImage(swapchain);

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(renderPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
				commandEncoder->draw(6, 1, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) final override {
				const ref<Device> device = getDevice();

				device->waitIdle();

				SwapchainDescriptor swapchainDescriptor;
				swapchainDescriptor.renderPass = renderPass;
				swapchainDescriptor.extentWidth = event.getWidth();
				swapchainDescriptor.extentHeight = event.getHeight();

				swapchain->recreate(swapchainDescriptor);
			}

		private:
			struct Vertex {
				svec2<float32> position;
			};

		private:
			// Use the glyph map image from a TTF font as example.
			TrueTypeFont font;

			ref<ImageView> imageView;

			ref<Sampler> sampler;

			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
	};
}