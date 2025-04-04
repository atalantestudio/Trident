#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Debug/TrueTypeFontDemo/TrueTypeFontDemoRenderer.hpp"

namespace ProjectA {
	TrueTypeFontDemoRenderer::TrueTypeFontDemoRenderer(const TTFFile& ttfFile) :
		ttfFile(ttfFile)
	{
		ENGINE_ASSERT(ttfFile.glyphs.size() < 4096, "Too much glyphs to compute.");

		createGlyphBuffers();
		createGlyphImage();
		createGlyphImageComputePipeline();
		createGlyphMapSdfImage();
		createGlyphMapSdfSampler();
		createGlyphSdfImageComputePipeline();

		transitionGlyphImageCompute();
		transitionGlyphMapSdfImageCompute();

		buildGlyphMapSdfImage();

		transitionGlyphImageRender();
		transitionGlyphMapSdfImageRender();

		createVertexBuffer();
		createIndexBuffer();
		createGlyphMapSdfImageRenderPipeline();
	}

	void TrueTypeFontDemoRenderer::createGlyphBuffers() {
		const uint64 maxGlyphSize = computeMaxGlyphSize();

		BufferDescriptor glyphStagingBufferDescriptor{};
		glyphStagingBufferDescriptor.size = maxGlyphSize;
		glyphStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		glyphStagingBuffer = createBuffer(glyphStagingBufferDescriptor);

		BufferDescriptor glyphStorageBufferDescriptor{};
		glyphStorageBufferDescriptor.size = maxGlyphSize;
		glyphStorageBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		glyphStorageBuffer = createBuffer(glyphStorageBufferDescriptor);
	}

	void TrueTypeFontDemoRenderer::createGlyphImage() {
		ImageDescriptor glyphImageDescriptor{};
		glyphImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::STORAGE;
		glyphImageDescriptor.width = 2048;
		glyphImageDescriptor.height = 2048;
		glyphImageDescriptor.depth = 1;
		glyphImageDescriptor.mipLevelCount = 1;
		glyphImageDescriptor.arrayLayerCount = 1;
		glyphImageDescriptor.sampleCount = 1;

		glyphImage = createImage(glyphImageDescriptor);

		const ImageViewDescriptor glyphImageViewDescriptor;

		glyphImageView = glyphImage->createView(glyphImageViewDescriptor);
	}

	void TrueTypeFontDemoRenderer::createGlyphImageComputePipeline() {
		DescriptorSetLayoutBinding glyphBufferDescriptorSetLayoutBinding;
		glyphBufferDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		glyphBufferDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphBufferDescriptorSetLayoutBinding.buffers = {glyphStorageBuffer};

		DescriptorSetLayoutBinding glyphImageDescriptorSetLayoutBinding;
		glyphImageDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		glyphImageDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphImageDescriptorSetLayoutBinding.imageViews = {glyphImageView};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings = {glyphBufferDescriptorSetLayoutBinding, glyphImageDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

		glyphImageComputePipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor shaderModuleDescriptor{};
		shaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		shaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> shaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/TrueTypeFontDemo/Shader/GlyphImage.comp.spv", shaderModuleDescriptor);

		ComputePipelineDescriptor computePipelineDescriptor;
		computePipelineDescriptor.pipelineLayout = glyphImageComputePipelineLayout;
		computePipelineDescriptor.computeShaderModule = shaderModule;

		glyphImageComputePipeline = createComputePipeline(computePipelineDescriptor);
	}

	void TrueTypeFontDemoRenderer::createGlyphMapSdfImage() {
		ImageDescriptor glyphMapSdfImageDescriptor{};
		glyphMapSdfImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphMapSdfImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphMapSdfImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::STORAGE;
		glyphMapSdfImageDescriptor.width = 64; // 4096
		glyphMapSdfImageDescriptor.height = 64; // 4096
		glyphMapSdfImageDescriptor.depth = 1;
		glyphMapSdfImageDescriptor.mipLevelCount = 1;
		glyphMapSdfImageDescriptor.arrayLayerCount = 1;
		glyphMapSdfImageDescriptor.sampleCount = 1;

		glyphMapSdfImage = createImage(glyphMapSdfImageDescriptor);

		const ImageViewDescriptor glyphMapSdfImageViewDescriptor;

		glyphMapSdfImageView = glyphMapSdfImage->createView(glyphMapSdfImageViewDescriptor);
	}

	void TrueTypeFontDemoRenderer::createGlyphMapSdfSampler() {
		SamplerDescriptor glyphSdfSamplerDescriptor{};
		glyphSdfSamplerDescriptor.magFilter = SamplerFilter::NEAREST;
		glyphSdfSamplerDescriptor.minFilter = SamplerFilter::NEAREST;

		glyphMapSdfSampler = createSampler(glyphSdfSamplerDescriptor);
	}

	void TrueTypeFontDemoRenderer::createGlyphSdfImageComputePipeline() {
		DescriptorSetLayoutBinding glyphImageDescriptorSetLayoutBinding;
		glyphImageDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		glyphImageDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphImageDescriptorSetLayoutBinding.imageViews = {glyphImageView};

		DescriptorSetLayoutBinding glyphMapSdfImageDescriptorSetLayoutBinding;
		glyphMapSdfImageDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		glyphMapSdfImageDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphMapSdfImageDescriptorSetLayoutBinding.imageViews = {glyphMapSdfImageView};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings = {glyphImageDescriptorSetLayoutBinding, glyphMapSdfImageDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

		glyphSdfImageComputePipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor shaderModuleDescriptor{};
		shaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		shaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> shaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/TrueTypeFontDemo/Shader/GlyphDistanceField.comp.spv", shaderModuleDescriptor);

		ComputePipelineDescriptor computePipelineDescriptor;
		computePipelineDescriptor.pipelineLayout = glyphSdfImageComputePipelineLayout;
		computePipelineDescriptor.computeShaderModule = shaderModule;

		glyphSdfImageComputePipeline = createComputePipeline(computePipelineDescriptor);
	}

	void TrueTypeFontDemoRenderer::createVertexBuffer() {
		const vec2<float32> vertices[4] = {
			{-1.0f,  1.0f},
			{ 1.0f,  1.0f},
			{ 1.0f, -1.0f},
			{-1.0f, -1.0f},
		};

		const uint64 vertexBufferSize = sizeof(vec2<float32>) * 4;

		BufferDescriptor vertexStagingBufferDescriptor{};
		vertexStagingBufferDescriptor.size = vertexBufferSize;
		vertexStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> vertexStagingBuffer = createBuffer(vertexStagingBufferDescriptor);

		vec2<float32>* vertexMappedRange = vertexStagingBuffer->map<vec2<float32>>(0, vertexBufferSize);

		std::copy_n(&vertices[0], 4, vertexMappedRange);

		vertexStagingBuffer->unmap();

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = vertexBufferSize;
		vertexBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX;

		vertexBuffer = createBuffer(vertexBufferDescriptor);

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

	void TrueTypeFontDemoRenderer::createIndexBuffer() {
		const uint16 indices[6] = {
			0, 1, 2,
			0, 2, 3,
		};

		const uint64 indexBufferSize = sizeof(uint16) * 6;

		BufferDescriptor indexStagingBufferDescriptor{};
		indexStagingBufferDescriptor.size = indexBufferSize;
		indexStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> indexStagingBuffer = createBuffer(indexStagingBufferDescriptor);

		uint16* indexMappedRange = indexStagingBuffer->map<uint16>(0, indexBufferSize);

		std::copy_n(&indices[0], 6, indexMappedRange);

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

	void TrueTypeFontDemoRenderer::createGlyphMapSdfImageRenderPipeline() {
		DescriptorSetLayoutBinding glyphImageDescriptorSetLayoutBinding;
		glyphImageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		glyphImageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		glyphImageDescriptorSetLayoutBinding.samplers = {glyphMapSdfSampler};
		glyphImageDescriptorSetLayoutBinding.imageViews = {glyphMapSdfImageView};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings = {glyphImageDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

		renderPipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/TrueTypeFontDemo/Shader/main.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/TrueTypeFontDemo/Shader/main.frag.spv", fragmentShaderModuleDescriptor);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.renderPass = Application::get().getRenderPass();
		renderPipelineDescriptor.pipelineLayout = renderPipelineLayout;
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(0, InputRate::VERTEX);
		renderPipelineDescriptor.vertexBufferLayout.add<vec2<float32>>(0, false);
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);
	}

	void TrueTypeFontDemoRenderer::transitionGlyphImageCompute() const {
		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphImage, ImageLayout::UNDEFINED, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TrueTypeFontDemoRenderer::transitionGlyphImageRender() const {
		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphImage, ImageLayout::GENERAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TrueTypeFontDemoRenderer::transitionGlyphMapSdfImageCompute() const {
		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphMapSdfImage, ImageLayout::UNDEFINED, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TrueTypeFontDemoRenderer::transitionGlyphMapSdfImageRender() const {
		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphMapSdfImage, ImageLayout::GENERAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TrueTypeFontDemoRenderer::buildGlyphMapSdfImage() const {
		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		uint16 xOffset = 0;
		uint16 yOffset = 0;

		for (uint16 glyphIndex = 0; glyphIndex < ttfFile.glyphs.size(); glyphIndex += 1) {
			const Glyph& glyph = ttfFile.glyphs[glyphIndex];

			if (glyph.edges.empty()) {
				continue;
			}

			const uint64 glyphSize = sizeof(sint32) * 4 + sizeof(GlyphEdge) * glyph.edges.size();

			sint32* glyphMappedRange = glyphStagingBuffer->map<sint32>(0, glyphSize);

			GlyphBuffer glyphBuffer{};
			glyphBuffer.minCoordinateX = glyph.min.x;
			glyphBuffer.minCoordinateY = glyph.min.y;
			glyphBuffer.maxCoordinateX = glyph.max.x;
			glyphBuffer.maxCoordinateY = glyph.max.y;
			glyphBuffer.edges.resize(glyph.edges.size());

			for (uint16 edgeIndex = 0; edgeIndex < glyph.edges.size(); edgeIndex += 1) {
				const Edge& edge = glyph.edges[edgeIndex];
				GlyphEdge& glyphBufferEdge = glyphBuffer.edges[edgeIndex];

				glyphBufferEdge.startX = edge.start.x;
				glyphBufferEdge.startY = edge.start.y;
				glyphBufferEdge.endX = edge.end.x;
				glyphBufferEdge.endY = edge.end.y;
				glyphBufferEdge.controlX = edge.control.x;
				glyphBufferEdge.controlY = edge.control.y;
				glyphBufferEdge.curved = edge.curved;
			}

			std::copy_n(&glyphBuffer.minCoordinateX, 4, glyphMappedRange);

			glyphMappedRange += 4;

			std::copy(glyphBuffer.edges.begin(), glyphBuffer.edges.end(), (GlyphEdge*) glyphMappedRange);

			glyphStagingBuffer->unmap();

			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

			{
				const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(glyphStagingBuffer, glyphStorageBuffer, 0, 0, glyphSize);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
				device->waitIdle();
			}

			{
				const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->bindComputePipeline(glyphImageComputePipeline);
				commandEncoder->bindDescriptorSets(0);
				commandEncoder->dispatchGroups(128, 128, 1);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
				device->waitIdle();
			}

			xOffset += 64;
			yOffset += 64;

			// TODO: Generate glyph distance field.

			{
				const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->bindComputePipeline(glyphSdfImageComputePipeline);
				commandEncoder->bindDescriptorSets(0);
				commandEncoder->dispatchGroups(8, 8, 1);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
				device->waitIdle();
			}

			break;
		}
	}

	uint64 TrueTypeFontDemoRenderer::computeMaxGlyphSize() const {
		uint64 maxglyphCount = 0;

		for (const Glyph& glyph : ttfFile.glyphs) {
			const uint64 glyphCount = glyph.edges.size();

			if (glyphCount > maxglyphCount) {
				maxglyphCount = glyphCount;
			}
		}

		return sizeof(sint32) * 4 + sizeof(GlyphEdge) * maxglyphCount;
	}

	void TrueTypeFontDemoRenderer::render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
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
		commandEncoder->drawIndexed(6, 1, 0);
		commandEncoder->endRenderPass();
	}
}