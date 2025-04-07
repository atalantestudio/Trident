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
		createCameraUniformBuffer();
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
		const uint64 vertexBufferSize = sizeof(Vertex) * 3 * MAX_CHARACTERS;

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = vertexBufferSize;
		vertexBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX;

		vertexBuffer = createBuffer(vertexBufferDescriptor);
	}

	void TextRenderer::createIndexBuffer() {
		constexpr uint64 indexCount = 6;
		constexpr uint16 indices[indexCount] = {
			0, 1, 2,
			0, 2, 3,
		};

		const uint64 indexBufferSize = sizeof(uint16) * indexCount * MAX_CHARACTERS;

		BufferDescriptor indexStagingBufferDescriptor{};
		indexStagingBufferDescriptor.size = indexBufferSize;
		indexStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> indexStagingBuffer = createBuffer(indexStagingBufferDescriptor);

		uint16* indexMappedRange = indexStagingBuffer->map<uint16>(0, indexBufferSize);

		for (uint32 characterIndex = 0; characterIndex < MAX_CHARACTERS; characterIndex += 1) {
			*indexMappedRange++ = characterIndex * 4 + 0;
			*indexMappedRange++ = characterIndex * 4 + 1;
			*indexMappedRange++ = characterIndex * 4 + 2;
			*indexMappedRange++ = characterIndex * 4 + 0;
			*indexMappedRange++ = characterIndex * 4 + 2;
			*indexMappedRange++ = characterIndex * 4 + 3;
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

	void TextRenderer::createCameraUniformBuffer() {
		BufferDescriptor cameraUniformBufferDescriptor{};
		cameraUniformBufferDescriptor.size = sizeof(mat3x3<float32>);
		cameraUniformBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::UNIFORM;

		cameraUniformBuffer = createBuffer(cameraUniformBufferDescriptor);

		cameraMappedRange = cameraUniformBuffer->map<mat3x3<float32>>(0, cameraUniformBufferDescriptor.size);
	}

	void TextRenderer::createPipeline() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

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

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.renderPass = Application::get().getRenderPass();
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(0, InputRate::VERTEX);
		renderPipelineDescriptor.vertexBufferLayout.add<vec2<float32>>(0, false);
		renderPipelineDescriptor.vertexBufferLayout.add<vec2<float32>>(0, false);
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);
	}

	void TextRenderer::setText(const std::string& text) {
		ENGINE_ASSERT(text.size() <= MAX_CHARACTERS, "The text must not exceed [] characters.", MAX_CHARACTERS);

		characterCount = static_cast<uint32>(text.size());

		const uint64 vertexBufferSize = sizeof(Vertex) * 4 * characterCount;

		BufferDescriptor vertexStagingBufferDescriptor{};
		vertexStagingBufferDescriptor.size = vertexBufferSize;
		vertexStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> vertexStagingBuffer = createBuffer(vertexStagingBufferDescriptor);

		Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

		const char* string = text.c_str();
		const uint8* codePoint = static_cast<const uint8*>(static_cast<const void*>(string));

		constexpr uint32 fontSize = 64;

		float32 positionX = 0;
		float32 positionY = 0;

		uint32 vertexIndex = 0;

		while (*codePoint != '\0') {
			const TrueTypeGlyph& glyph = font[*codePoint];

			if (!glyph.invisible) {
				const float32 width = glyph.width;

				vertexMappedRange[vertexIndex + 0].position = {glyph.leftSideBearing + positionX, positionY};
				vertexMappedRange[vertexIndex + 0].uv = {glyph.minCoordinateX, glyph.minCoordinateY};

				vertexMappedRange[vertexIndex + 1].position = {glyph.leftSideBearing + positionX + width, positionY};
				vertexMappedRange[vertexIndex + 1].uv = {glyph.maxCoordinateX, glyph.minCoordinateY};

				vertexMappedRange[vertexIndex + 2].position = {glyph.leftSideBearing + positionX + width, positionY + fontSize};
				vertexMappedRange[vertexIndex + 2].uv = {glyph.maxCoordinateX, glyph.maxCoordinateY};

				vertexMappedRange[vertexIndex + 3].position = {glyph.leftSideBearing + positionX, positionY + fontSize};
				vertexMappedRange[vertexIndex + 3].uv = {glyph.minCoordinateX, glyph.maxCoordinateY};

				vertexIndex += 4;
			}

			positionX += glyph.advanceWidth;
			codePoint += 1;
		}

		vertexStagingBuffer->unmap();

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, 0, 0, vertexBufferSize);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
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
		commandEncoder->bindVertexBuffer(0, vertexBuffer, sizeof(vec2<float32>));
		commandEncoder->setViewport({0.0f, static_cast<float32>(height)}, {static_cast<float32>(width), -static_cast<float32>(height)}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, width, height);
		commandEncoder->drawIndexed(6 * characterCount, 1, 0);
		commandEncoder->endRenderPass();
	}

	void TextRenderer::updateCamera() const {
		const float32 width = static_cast<float32>(Application::get().getSwapchain()->getExtentWidth());
		const float32 height = static_cast<float32>(Application::get().getSwapchain()->getExtentHeight());

		const mat3x3<float32> cameraProjection = {
			2.0f / width, 0.0f, 0.0f,
			0.0f, -2.0f / height, 0.0f,
			-1.0f, 1.0f, 1.0f,
		};

		*cameraMappedRange = cameraProjection;
	}
}