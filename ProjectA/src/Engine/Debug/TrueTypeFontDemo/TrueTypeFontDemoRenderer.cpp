#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Debug/TrueTypeFontDemo/TrueTypeFontDemoRenderer.hpp"

namespace ProjectA {
	TrueTypeFontDemoRenderer::TrueTypeFontDemoRenderer(const TTFFile& ttfFile) {
		const ref<Device>& device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const Glyph& glyph = ttfFile.glyphs[16];

		ENGINE_ASSERT(!glyph.edges.empty(), "Glyph has no edges.");

		const uint64 size = sizeof(sint32) * 2 + sizeof(GlyphEdge) * glyph.edges.size();

		BufferDescriptor glyphEdgeStagingBufferDescriptor{};
		glyphEdgeStagingBufferDescriptor.size = size;
		glyphEdgeStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> glyphEdgeStagingBuffer = createBuffer(glyphEdgeStagingBufferDescriptor);

		sint32* glyphEdgeMappedRange = glyphEdgeStagingBuffer->map<sint32>(0, size);

		GlyphBuffer glyphBuffer{};
		glyphBuffer.minCoordinate = static_cast<sint32>(glyph.min.x << 16) | glyph.min.y;
		glyphBuffer.maxCoordinate = static_cast<sint32>(glyph.max.x << 16) | glyph.max.y;
		glyphBuffer.edges.resize(glyph.edges.size());

		for (uint16 edgeIndex = 0; edgeIndex < glyph.edges.size(); edgeIndex += 1) {
			const Edge& edge = glyph.edges[edgeIndex];

			glyphBuffer.edges[edgeIndex].start = (edge.start.x << 16) | edge.start.y;
			glyphBuffer.edges[edgeIndex].end = (edge.end.x << 16) | edge.end.y;
			glyphBuffer.edges[edgeIndex].control = (edge.control.x << 16) | edge.control.y;
			glyphBuffer.edges[edgeIndex].curved = edge.isQuadraticCurve;
		}

		std::copy(&glyphBuffer.minCoordinate, &glyphBuffer.maxCoordinate + 1, glyphEdgeMappedRange);

		glyphEdgeMappedRange += 2;

		std::copy(glyphBuffer.edges.begin(), glyphBuffer.edges.end(), (GlyphEdge*) glyphEdgeMappedRange);

		glyphEdgeStagingBuffer->unmap();

		BufferDescriptor glyphEdgeStorageBufferDescriptor{};
		glyphEdgeStorageBufferDescriptor.size = size;
		glyphEdgeStorageBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		glyphEdgeStorageBuffer = createBuffer(glyphEdgeStorageBufferDescriptor);

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(glyphEdgeStagingBuffer, glyphEdgeStorageBuffer, 0, 0, size);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

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

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->setImageMemoryBarrier(glyphImage, ImageLayout::UNDEFINED, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		SamplerDescriptor samplerDescriptor{};
		samplerDescriptor.magFilter = SamplerFilter::NEAREST;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		sampler = createSampler(samplerDescriptor);

		const ImageViewDescriptor imageViewDescriptor;

		glyphImageView = glyphImage->createView(imageViewDescriptor);

		DescriptorSetLayoutBinding glyphEdgeDescriptorSetLayoutBinding;
		glyphEdgeDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		glyphEdgeDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphEdgeDescriptorSetLayoutBinding.buffers = {glyphEdgeStorageBuffer};

		DescriptorSetLayoutBinding imageDescriptorSetLayoutBinding;
		imageDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		imageDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		imageDescriptorSetLayoutBinding.samplers = {sampler};
		imageDescriptorSetLayoutBinding.imageViews = {glyphImageView};

		DescriptorSetLayout computeDescriptorSetLayout;
		computeDescriptorSetLayout.bindings.emplace_back(glyphEdgeDescriptorSetLayoutBinding);
		computeDescriptorSetLayout.bindings.emplace_back(imageDescriptorSetLayoutBinding);

		PipelineLayoutDescriptor computePipelineLayoutDescriptor;
		computePipelineLayoutDescriptor.layouts.emplace_back(computeDescriptorSetLayout);

		const ref<PipelineLayout> computePipelineLayout = createPipelineLayout(computePipelineLayoutDescriptor);

		ShaderModuleDescriptor computeShaderModuleDescriptor{};
		computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		computeShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> computeShaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/TrueTypeFontDemo/Shader/main.comp.spv", computeShaderModuleDescriptor);

		ComputePipelineDescriptor computePipelineDescriptor;
		computePipelineDescriptor.pipelineLayout = computePipelineLayout;
		computePipelineDescriptor.computeShaderModule = computeShaderModule;

		const ref<ComputePipeline> computePipeline = createComputePipeline(computePipelineDescriptor);

		CommandBufferDescriptor computeCommandBufferDescriptor{};
		computeCommandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		{
			const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(computeCommandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->bindComputePipeline(computePipeline);
			commandEncoder->bindDescriptorSets(0);
			commandEncoder->dispatchGroups(128, 128, 1);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->setImageMemoryBarrier(glyphImage, ImageLayout::GENERAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		DescriptorSetLayoutBinding descriptorSetLayoutBinding;
		descriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		descriptorSetLayoutBinding.samplers = {sampler};
		descriptorSetLayoutBinding.imageViews = {glyphImageView};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings.emplace_back(descriptorSetLayoutBinding);

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts.emplace_back(descriptorSetLayout);

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

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
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(0, InputRate::VERTEX);
		renderPipelineDescriptor.vertexBufferLayout.add<vec2<float32>>(0, false);
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);

		const chain<vec2<float32>> vertices = {
			{-1.0f,  1.0f},
			{ 1.0f,  1.0f},
			{ 1.0f, -1.0f},
			{-1.0f, -1.0f},
		};

		const uint64 vertexBufferSize = sizeof(vec2<float32>) * vertices.size();

		BufferDescriptor vertexStagingBufferDescriptor{};
		vertexStagingBufferDescriptor.size = vertexBufferSize;
		vertexStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> vertexStagingBuffer = createBuffer(vertexStagingBufferDescriptor);

		vec2<float32>* vertexMappedRange = vertexStagingBuffer->map<vec2<float32>>(0, vertexBufferSize);

		std::copy(vertices.begin(), vertices.end(), vertexMappedRange);

		vertexStagingBuffer->unmap();

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = vertexBufferSize;
		vertexBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX;

		vertexBuffer = createBuffer(vertexBufferDescriptor);

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, 0, 0, vertexBufferSize);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		const std::array<uint16, 6> indices = {
			0, 1, 2,
			0, 2, 3,
		};

		const uint64 indexBufferSize = sizeof(uint16) * indices.size();

		BufferDescriptor indexStagingBufferDescriptor{};
		indexStagingBufferDescriptor.size = indexBufferSize;
		indexStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> indexStagingBuffer = createBuffer(indexStagingBufferDescriptor);

		uint16* indexMappedRange = indexStagingBuffer->map<uint16>(0, indexBufferSize);

		std::copy(indices.begin(), indices.end(), indexMappedRange);

		indexStagingBuffer->unmap();

		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = indexBufferSize;
		indexBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX;

		indexBuffer = createBuffer(indexBufferDescriptor);

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(indexStagingBuffer, indexBuffer, 0, 0, indexBufferSize);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}
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