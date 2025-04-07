#include "pch.hpp"

#include "Engine/Application.hpp"
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
		samplerDescriptor.magFilter = SamplerFilter::LINEAR;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		glyphMapImageSampler = createSampler(samplerDescriptor);
	}

	void TextRenderer::createVertexBuffer() {
		constexpr uint64 vertexBufferSize = sizeof(Vertex) * 4 * MAX_QUAD_COUNT;

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = vertexBufferSize;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX;

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
		indexBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX;

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

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("assets/shaders/Glyph.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("assets/shaders/Glyph.frag.spv", fragmentShaderModuleDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec2<float32>>(false);
		vertexInputBinding.registerAttribute<vec2<float32>>(false);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.renderPass = Application::get().getRenderPass();
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);
	}

	void TextRenderer::setText(const std::string& text) {
		ENGINE_ASSERT(text.size() <= MAX_QUAD_COUNT, "The text must not exceed [] characters.", MAX_QUAD_COUNT);

		quadCount = static_cast<uint32>(text.size());

		const char* string = text.c_str();
		const uint8* codePoint = static_cast<const uint8*>(static_cast<const void*>(string));

		constexpr uint32 fontSize = 64;

		uint32 vertexIndex = 0;
		float32 positionX = 0;
		float32 positionY = 0;

		while (*codePoint != '\0') {
			const TrueTypeGlyph& glyph = font[*codePoint];

			if (!glyph.invisible) {
				vertices[vertexIndex + 0].position = {glyph.leftSideBearing + positionX, positionY};
				vertices[vertexIndex + 0].uv = {glyph.minCoordinateX, glyph.minCoordinateY};
				vertices[vertexIndex + 1].position = {glyph.leftSideBearing + positionX + glyph.width, positionY};
				vertices[vertexIndex + 1].uv = {glyph.maxCoordinateX, glyph.minCoordinateY};
				vertices[vertexIndex + 2].position = {glyph.leftSideBearing + positionX + glyph.width, positionY + fontSize};
				vertices[vertexIndex + 2].uv = {glyph.maxCoordinateX, glyph.maxCoordinateY};
				vertices[vertexIndex + 3].position = {glyph.leftSideBearing + positionX, positionY + fontSize};
				vertices[vertexIndex + 3].uv = {glyph.minCoordinateX, glyph.maxCoordinateY};

				vertexIndex += 4;
			}

			positionX += glyph.advanceWidth;
			codePoint += 1;
		}
	}

	void TextRenderer::render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		updateCamera();

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = Application::get().getRenderPass();
		beginRenderPassDescriptor.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		beginRenderPassDescriptor.nextImageIndex = nextImageIndex;

		const ref<Swapchain>& swapchain = Application::get().getSwapchain();

		const uint32 width = swapchain->getExtentWidth();
		const uint32 height = swapchain->getExtentHeight();

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->bindRenderPipeline(renderPipeline);
		commandEncoder->bindDescriptorSets(0);
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, sizeof(Vertex));
		commandEncoder->setViewport({0.0f, static_cast<float32>(height)}, {static_cast<float32>(width), -static_cast<float32>(height)}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, width, height);
		commandEncoder->drawIndexed(6 * quadCount, 1, 0);
		commandEncoder->endRenderPass();
	}

	void TextRenderer::updateCamera() const {
		const float32 width = static_cast<float32>(Application::get().getSwapchain()->getExtentWidth());
		const float32 height = static_cast<float32>(Application::get().getSwapchain()->getExtentHeight());

		camera->projection = {
			2.0f / width, 0.0f, 0.0f,
			0.0f, -2.0f / height, 0.0f,
			-1.0f, 1.0f, 1.0f,
		};
	}
}