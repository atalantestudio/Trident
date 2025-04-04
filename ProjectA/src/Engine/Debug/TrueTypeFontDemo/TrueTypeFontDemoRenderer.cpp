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
		maxGlyphSize = computeMaxGlyphSize();

		BufferDescriptor glyphStagingBufferDescriptor{};
		glyphStagingBufferDescriptor.size = maxGlyphSize;
		glyphStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		glyphStagingBuffer = createBuffer(glyphStagingBufferDescriptor);

		glyphMappedRange = glyphStagingBuffer->map<sint32>(0, maxGlyphSize);

		BufferDescriptor glyphStorageBufferDescriptor{};
		glyphStorageBufferDescriptor.size = maxGlyphSize;
		glyphStorageBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		glyphStorageBuffer = createBuffer(glyphStorageBufferDescriptor);

		BufferDescriptor glyphIndexStagingBufferDescriptor{};
		glyphIndexStagingBufferDescriptor.size = sizeof(uint32);
		glyphIndexStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		glyphIndexStagingBuffer = createBuffer(glyphIndexStagingBufferDescriptor);

		glyphIndexMappedRange = glyphIndexStagingBuffer->map<uint32>(0, sizeof(uint32));

		BufferDescriptor glyphIndexStorageBufferDescriptor{};
		glyphIndexStorageBufferDescriptor.size = sizeof(uint32);
		glyphIndexStorageBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		glyphIndexStorageBuffer = createBuffer(glyphIndexStorageBufferDescriptor);
	}

	void TrueTypeFontDemoRenderer::createGlyphImage() {
		ImageDescriptor glyphImageDescriptor{};
		glyphImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::STORAGE;
		glyphImageDescriptor.width = 4096;
		glyphImageDescriptor.height = 4096;
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
		glyphMapSdfImageDescriptor.width = 512;
		glyphMapSdfImageDescriptor.height = 512;
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
		glyphSdfSamplerDescriptor.magFilter = SamplerFilter::LINEAR;
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

		DescriptorSetLayoutBinding glyphIndexDescriptorSetLayoutBinding;
		glyphIndexDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		glyphIndexDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphIndexDescriptorSetLayoutBinding.buffers = {glyphIndexStorageBuffer};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings = {
			glyphImageDescriptorSetLayoutBinding,
			glyphMapSdfImageDescriptorSetLayoutBinding,
			glyphIndexDescriptorSetLayoutBinding,
		};

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

		const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphImage, ImageLayout::UNDEFINED, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TrueTypeFontDemoRenderer::transitionGlyphImageRender() const {
		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphImage, ImageLayout::GENERAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TrueTypeFontDemoRenderer::transitionGlyphMapSdfImageCompute() const {
		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphMapSdfImage, ImageLayout::UNDEFINED, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TrueTypeFontDemoRenderer::transitionGlyphMapSdfImageRender() const {
		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphMapSdfImage, ImageLayout::GENERAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TrueTypeFontDemoRenderer::buildGlyphMapSdfImage() const {
		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		for (uint16 glyphIndex = 0, glyphOffset = 0; glyphIndex < 20; glyphIndex += 1) {
			ENGINE_TRACE("Computing SDF for glyph [].", glyphIndex);

			const Glyph& glyph = ttfFile.glyphs[glyphIndex];
			const longHorMetric& hMetric = ttfFile.hmetrics[glyphIndex];

			if (glyph.edges.empty()) {
				continue;
			}

			const uint64 glyphSize = sizeof(sint32) * 6 + sizeof(GlyphEdge) * glyph.edges.size();

			GlyphBuffer glyphBuffer{};
			glyphBuffer.minCoordinateX = glyph.min.x;
			glyphBuffer.minCoordinateY = glyph.min.y;
			glyphBuffer.maxCoordinateX = glyph.max.x;
			glyphBuffer.maxCoordinateY = glyph.max.y;
			glyphBuffer.advanceWidth = hMetric.advanceWidth;
			glyphBuffer.leftBearing = hMetric.leftSideBearing;
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

			sint32* glyphMappedRangeCopy = glyphMappedRange;

			std::copy_n(&glyphBuffer.minCoordinateX, 6, glyphMappedRangeCopy);

			glyphMappedRangeCopy += 6;

			std::copy(glyphBuffer.edges.begin(), glyphBuffer.edges.end(), (GlyphEdge*) glyphMappedRangeCopy);

			glyphMappedRangeCopy += 8 * glyphBuffer.edges.size();

			std::fill_n(glyphMappedRangeCopy, maxGlyphSize - (glyphMappedRangeCopy - glyphMappedRange), 0);

			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

			{
				const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(glyphStagingBuffer, glyphStorageBuffer, 0, 0, maxGlyphSize);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
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

			*glyphIndexMappedRange = glyphOffset;

			{
				const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(glyphIndexStagingBuffer, glyphIndexStorageBuffer, 0, 0, sizeof(uint32));

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
				device->waitIdle();
			}

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

			glyphOffset += 1;
		}
	}

	uint64 TrueTypeFontDemoRenderer::computeMaxGlyphSize() const {
		uint64 maxGlyphEdgeCount = 0;

		for (const Glyph& glyph : ttfFile.glyphs) {
			const uint64 glyphEdgeCount = glyph.edges.size();

			if (glyphEdgeCount > maxGlyphEdgeCount) {
				maxGlyphEdgeCount = glyphEdgeCount;
			}
		}

		return sizeof(sint32) * 6 + sizeof(GlyphEdge) * maxGlyphEdgeCount;
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