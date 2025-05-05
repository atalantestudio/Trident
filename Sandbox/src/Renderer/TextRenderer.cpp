#include "pch.hpp"

#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

#include "TextRenderer.hpp"

namespace ProjectA {
	TextRenderer::TextRenderer(const TrueTypeFont& font) :
		font(font)
	{
		glyphMapImage = this->font.getSdfImage();

		createGlyphMapImageView();
		createGlyphMapImageSampler();
		createVertexBuffer();
		createIndexBuffer();
		createCameraBuffer();
		createPipeline();
	}

	void TextRenderer::createGlyphMapImageView() {
		const ImageViewDescriptor imageViewDescriptor;

		glyphMapImageView = glyphMapImage->createView(imageViewDescriptor);
	}

	void TextRenderer::createGlyphMapImageSampler() {
		SamplerDescriptor samplerDescriptor{};
		samplerDescriptor.magFilter = SamplerFilter::NEAREST;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		glyphMapImageSampler = createSampler(samplerDescriptor);
	}

	void TextRenderer::createVertexBuffer() {
		constexpr uint64 vertexBufferSize = MAX_QUAD_COUNT * 4 * sizeof(Vertex);

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = vertexBufferSize;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX;
		vertexBufferDescriptor.persistent = true;

		vertexBuffer = createBuffer(vertexBufferDescriptor);

		vertices = vertexBuffer->map<Vertex>(0, vertexBufferSize);
	}

	void TextRenderer::createIndexBuffer() {
		const uint64 indexBufferSize = sizeof(uint16) * 6 * MAX_QUAD_COUNT;

		const ref<Buffer> indexStagingBuffer = createStagingBuffer(indexBufferSize);

		uint16* indexMappedRange = indexStagingBuffer->map<uint16>(0, indexBufferSize);

		for (uint32 quadIndex = 0; quadIndex < MAX_QUAD_COUNT; quadIndex += 1) {
			*indexMappedRange++ = quadIndex * 4 + 0;
			*indexMappedRange++ = quadIndex * 4 + 1;
			*indexMappedRange++ = quadIndex * 4 + 2;
			*indexMappedRange++ = quadIndex * 4 + 0;
			*indexMappedRange++ = quadIndex * 4 + 2;
			*indexMappedRange++ = quadIndex * 4 + 3;
		}

		indexStagingBuffer->unmap();

		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = indexBufferSize;
		indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;

		indexBuffer = createBuffer(indexBufferDescriptor);

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(indexStagingBuffer, indexBuffer, 0, 0, indexBufferSize);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TextRenderer::createCameraBuffer() {
		constexpr uint64 cameraBufferSize = sizeof(Camera);

		BufferDescriptor cameraUniformBufferDescriptor{};
		cameraUniformBufferDescriptor.size = cameraBufferSize;
		cameraUniformBufferDescriptor.usage = BufferUsage::UNIFORM;

		cameraBuffer = createBuffer(cameraUniformBufferDescriptor);

		camera = cameraBuffer->map<Camera>(0, cameraBufferSize);
	}

	void TextRenderer::createPipeline() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

		DescriptorSetLayoutBinding imageDescriptorSetLayoutBinding;
		imageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		imageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		imageDescriptorSetLayoutBinding.imageViews = {glyphMapImageView};
		imageDescriptorSetLayoutBinding.samplers = {glyphMapImageSampler};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings = {
			cameraDescriptorSetLayoutBinding,
			imageDescriptorSetLayoutBinding,
		};

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("Asset/Shader/Glyph.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Asset/Shader/Glyph.frag.spv", fragmentShaderModuleDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec2<float32>>(false);
		vertexInputBinding.registerAttribute<vec2<float32>>(false);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.renderPass = getMainRenderPass();
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);
	}

	void TextRenderer::setText(const std::string& text, float32 fontSize) {
		ENGINE_ASSERT(text.size() <= MAX_QUAD_COUNT, "The text must not exceed [] characters.", MAX_QUAD_COUNT);

		quadCount = static_cast<uint32>(text.size());

		const char* string = text.c_str();
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
				vertices[vertexIndex + 0].position = {x, y};
				vertices[vertexIndex + 0].uv = {glyph.minCoordinateX, glyph.minCoordinateY};

				// Top-right
				vertices[vertexIndex + 1].position = {x + width, y};
				vertices[vertexIndex + 1].uv = {glyph.maxCoordinateX, glyph.minCoordinateY};

				// Bottom-right
				vertices[vertexIndex + 2].position = {x + width, y + fontSize};
				vertices[vertexIndex + 2].uv = {glyph.maxCoordinateX, glyph.maxCoordinateY};

				// Bottom-left
				vertices[vertexIndex + 3].position = {x, y + fontSize};
				vertices[vertexIndex + 3].uv = {glyph.minCoordinateX, glyph.maxCoordinateY};

				vertexIndex += 4;
			}

			positionX += advanceWidth;
			codePoint += 1;
		}
	}

	void TextRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		updateCamera();

		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		commandEncoder->bindRenderPipeline(renderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
		commandEncoder->drawIndexed(6 * quadCount, 1, 0);
	}

	void TextRenderer::updateCamera() const {
		const float32 width = static_cast<float32>(getSwapchain()->getExtentWidth());
		const float32 height = static_cast<float32>(getSwapchain()->getExtentHeight());

		camera->projection = {
			2.0f / width, 0.0f, 0.0f,
			0.0f, -2.0f / height, 0.0f,
			-1.0f, 1.0f, 1.0f,
		};
	}
}