#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextRenderer : public Renderer {
		public:
			TextRenderer() :
				font("../ProjectA/src/Asset/Font/test.ttf")
			{
				const ref<Device> device = getDevice();
				const ref<Image>& glyphMapImage = font.getSdfImage();

				// 1. Create glyph map image view.
				{
					constexpr ImageViewDescriptor imageViewDescriptor;

					glyphMapImageView = glyphMapImage->createView(imageViewDescriptor);
				}

				// 2. Create glyph map image sampler.
				{
					SamplerDescriptor samplerDescriptor{};
					samplerDescriptor.magFilter = SamplerFilter::NEAREST;
					samplerDescriptor.minFilter = SamplerFilter::NEAREST;

					glyphMapImageSampler = createSampler(samplerDescriptor);
				}

				// 3. Create camera buffer.
				{
					constexpr uint64 cameraBufferSize = sizeof(Camera);

					BufferDescriptor cameraUniformBufferDescriptor;
					cameraUniformBufferDescriptor.size = cameraBufferSize;
					cameraUniformBufferDescriptor.usage = BufferUsage::UNIFORM;
					cameraUniformBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
					cameraUniformBufferDescriptor.persistent = true;

					cameraBuffer = createBuffer(cameraUniformBufferDescriptor);
					cameraMappedRange = cameraBuffer->map<Camera>(0, cameraBufferSize);
				}

				// 4. Create render pass.
				{
					ColorAttachment colorAttachment{};
					colorAttachment.format = getSurface()->getFormat();
					colorAttachment.sampleCount = 1;
					colorAttachment.loadOperation = LoadOperation::CLEAR;
					colorAttachment.storeOperation = StoreOperation::STORE;

					RenderPassDescriptor renderPassDescriptor;
					renderPassDescriptor.colorAttachments = {colorAttachment};
					renderPassDescriptor.resolve = false;
					renderPassDescriptor.depthStencil = false;
					renderPassDescriptor.presentAttachmentIndex = 0;

					renderPass = createRenderPass(renderPassDescriptor);
				}

				const ref<Swapchain>& swapchain = getSwapchain();

				swapchain->getImages(renderPass);

				// 5. Create pipeline layout.
				{
					DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
					cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
					cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
					cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

					DescriptorSetLayoutBinding glyphMapImageDescriptorSetLayoutBinding;
					glyphMapImageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
					glyphMapImageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
					glyphMapImageDescriptorSetLayoutBinding.imageViews = {glyphMapImageView};
					glyphMapImageDescriptorSetLayoutBinding.samplers = {glyphMapImageSampler};

					DescriptorSetLayout descriptorSetLayout;
					descriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, glyphMapImageDescriptorSetLayoutBinding};

					PipelineLayoutDescriptor pipelineLayoutDescriptor;
					pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

					pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
				}

				// 6. Create render pipeline.
				{
					VertexInputBinding vertexInputBinding(InputRate::VERTEX);
					vertexInputBinding.registerAttribute<svec2<float32>>();
					vertexInputBinding.registerAttribute<svec2<float32>>();

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
					renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
					renderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Glyph.vert", vertexShaderModuleDescriptor);
					renderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Glyph.frag", fragmentShaderModuleDescriptor);
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

				// 7. Create index buffer.
				{
					constexpr uint64 indexBufferSize = 6 * MAX_CHARACTER_COUNT * sizeof(uint16);

					const ref<Buffer> indexStagingBuffer = createStagingBuffer(indexBufferSize);

					uint16* indexMappedRange = indexStagingBuffer->map<uint16>(0, indexBufferSize);

					for (uint32 characterIndex = 0; characterIndex < MAX_CHARACTER_COUNT; characterIndex += 1) {
						indexMappedRange[0] = 4 * characterIndex + 0;
						indexMappedRange[1] = 4 * characterIndex + 1;
						indexMappedRange[2] = 4 * characterIndex + 2;
						indexMappedRange[3] = 4 * characterIndex + 0;
						indexMappedRange[4] = 4 * characterIndex + 2;
						indexMappedRange[5] = 4 * characterIndex + 3;

						indexMappedRange += 6;
					}

					indexStagingBuffer->unmap();

					BufferDescriptor indexBufferDescriptor;
					indexBufferDescriptor.size = indexBufferSize;
					indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
					indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

					indexBuffer = createBuffer(indexBufferDescriptor);

					CommandBufferDescriptor commandBufferDescriptor{};
					commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(indexStagingBuffer, indexBuffer, 0, 0, indexBufferSize);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				// 8. Create vertex buffer.
				{
					constexpr uint64 vertexBufferSize = 4 * MAX_CHARACTER_COUNT * sizeof(Vertex);

					BufferDescriptor vertexBufferDescriptor;
					vertexBufferDescriptor.size = vertexBufferSize;
					vertexBufferDescriptor.usage = BufferUsage::VERTEX;
					vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
					vertexBufferDescriptor.persistent = true;

					vertexBuffer = createBuffer(vertexBufferDescriptor);
					vertexMappedRange = vertexBuffer->map<Vertex>(0, vertexBufferSize);
				}

				updateCamera();

				setText("Text/\\Renderer", 64.0f);
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
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
				commandEncoder->drawIndexed(6 * characterCount, 1, 0);
				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				updateCamera();

				const ref<Swapchain>& swapchain = getSwapchain();

				swapchain->getImages(renderPass);
			}

			void setText(std::string_view text, float32 fontSize) {
				ENGINE_ASSERT(text.size() <= MAX_CHARACTER_COUNT, "The text must not exceed [] characters.", MAX_CHARACTER_COUNT);

				characterCount = static_cast<uint32>(text.size());

				const char* string = text.data();
				const uint8* codePoint = static_cast<const uint8*>(static_cast<const void*>(string));
				const float32 lineHeight = static_cast<float32>(font.getLineHeight());

				uint32 vertexIndex = 0;
				float32 positionX = 0;
				float32 positionY = 0;

				while (*codePoint != '\0') {
					const TrueTypeGlyph& glyph = font[*codePoint];

					const float32 advanceWidth = std::ceil(glyph.advanceWidth / 1210.0f * static_cast<float32>(fontSize));

					if (!glyph.invisible) {
						// For a line height (ascent - descent) of 1210 FUnits,
						// the 'A' glyph's width (not including bearing or advance) is 631 FUnits.
						// The width should be 62.13px.
						// TODO: float32 lyah::ceil(float32)
						const float32 width = glyph.width / lineHeight * fontSize;

						const float32 leftSideBearing = glyph.leftSideBearing / lineHeight * fontSize;

						const float32 x = positionX + leftSideBearing;
						const float32 y = positionY;

						// Top-left
						vertexMappedRange[vertexIndex + 0].vertex = {x, y};
						vertexMappedRange[vertexIndex + 0].imageCoordinate = {glyph.minCoordinateX, glyph.minCoordinateY};

						// Top-right
						vertexMappedRange[vertexIndex + 1].vertex = {x + width, y};
						vertexMappedRange[vertexIndex + 1].imageCoordinate = {glyph.maxCoordinateX, glyph.minCoordinateY};

						// Bottom-right
						vertexMappedRange[vertexIndex + 2].vertex = {x + width, y + fontSize};
						vertexMappedRange[vertexIndex + 2].imageCoordinate = {glyph.maxCoordinateX, glyph.maxCoordinateY};

						// Bottom-left
						vertexMappedRange[vertexIndex + 3].vertex = {x, y + fontSize};
						vertexMappedRange[vertexIndex + 3].imageCoordinate = {glyph.minCoordinateX, glyph.maxCoordinateY};

						vertexIndex += 4;
					}

					positionX += advanceWidth;
					codePoint += 1;
				}
			}

		private:
			void updateCamera() const {
				const ref<Swapchain>& swapchain = getSwapchain();
				const float32 width = static_cast<float32>(swapchain->getExtentWidth());
				const float32 height = static_cast<float32>(swapchain->getExtentHeight());

				cameraMappedRange->projection = {
					 2.0f / width,  0.0f,           0.0f,
					 0.0f,         -2.0f / height,  0.0f,
					-1.0f,          1.0f,           1.0f,
				};
			}

		private:
			struct Vertex {
				svec2<float32> vertex;
				svec2<float32> imageCoordinate;
			};

			struct Camera {
				mat3x3<float32> projection;
			};

		private:
			static constexpr uint64 MAX_CHARACTER_COUNT = 64;

		private:
			TrueTypeFont font;

			ref<Image> glyphMapImage;
			ref<ImageView> glyphMapImageView;

			ref<Sampler> glyphMapImageSampler;

			ref<Buffer> cameraBuffer;
			Camera* cameraMappedRange;

			ref<RenderPass> renderPass;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> indexBuffer;

			ref<Buffer> vertexBuffer;
			Vertex* vertexMappedRange;

			uint32 characterCount = 0;
	};
}