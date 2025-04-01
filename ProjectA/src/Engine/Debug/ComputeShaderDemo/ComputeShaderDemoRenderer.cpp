#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Debug/ComputeShaderDemo/ComputeShaderDemoRenderer.hpp"

namespace ProjectA {
	ComputeShaderDemoRenderer::ComputeShaderDemoRenderer() {
		const ref<Device>& device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const uint32 textureWidth = 64;
		const uint32 textureHeight = 64;
		const uint32 textureDepth = 1;

		TextureDescriptor textureDescriptor{};
		textureDescriptor.type = TextureType::TEXTURE_2D;
		textureDescriptor.format = TextureFormat::R8G8B8A8_UNORM;
		textureDescriptor.usage = TextureUsage::SAMPLED | TextureUsage::STORAGE;
		textureDescriptor.width = textureWidth;
		textureDescriptor.height = textureHeight;
		textureDescriptor.depth = textureDepth;
		textureDescriptor.mipLevelCount = 1;
		textureDescriptor.arrayLayerCount = 1;
		textureDescriptor.sampleCount = 1;

		texture = createTexture(textureDescriptor);

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->setTextureMemoryBarrier(texture, TextureLayout::UNDEFINED, TextureLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		SamplerDescriptor samplerDescriptor{};
		samplerDescriptor.magFilter = SamplerFilter::NEAREST;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		sampler = createSampler(samplerDescriptor);

		const TextureViewDescriptor textureViewDescriptor;

		textureView = texture->createView(textureViewDescriptor);

		DescriptorSetLayoutBinding computeDescriptorSetLayoutBinding;
		computeDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_TEXTURE;
		computeDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		computeDescriptorSetLayoutBinding.samplers = {sampler};
		computeDescriptorSetLayoutBinding.textureViews = {textureView};

		DescriptorSetLayout computeDescriptorSetLayout;
		computeDescriptorSetLayout.bindings.emplace_back(computeDescriptorSetLayoutBinding);

		PipelineLayoutDescriptor computePipelineLayoutDescriptor;
		computePipelineLayoutDescriptor.layouts.emplace_back(computeDescriptorSetLayout);

		const ref<PipelineLayout> computePipelineLayout = createPipelineLayout(computePipelineLayoutDescriptor);

		ShaderModuleDescriptor computeShaderModuleDescriptor{};
		computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		computeShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> computeShaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/ComputeShaderDemo/Shader/ComputeShaderDemo.comp.spv", computeShaderModuleDescriptor);

		ComputePipelineDescriptor computePipelineDescriptor;
		computePipelineDescriptor.pipelineLayout = computePipelineLayout;
		computePipelineDescriptor.computeShaderModule = computeShaderModule;

		const ref<ComputePipeline> computePipeline = createComputePipeline(computePipelineDescriptor);

		CommandBufferDescriptor computeCommandBufferDescriptor{};
		computeCommandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		{
			const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(computeCommandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->bindPipeline(computePipeline);
			commandEncoder->bindDescriptorSets(0);
			commandEncoder->dispatchGroups(64, 64, 1);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->setTextureMemoryBarrier(texture, TextureLayout::GENERAL, TextureLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		DescriptorSetLayoutBinding descriptorSetLayoutBinding;
		descriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		descriptorSetLayoutBinding.samplers = {sampler};
		descriptorSetLayoutBinding.textureViews = {textureView};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings.emplace_back(descriptorSetLayoutBinding);

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts.emplace_back(descriptorSetLayout);

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/ComputeShaderDemo/Shader/ComputeShaderDemo.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/ComputeShaderDemo/Shader/ComputeShaderDemo.frag.spv", fragmentShaderModuleDescriptor);

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

		const BufferMappedRange vertexMappedRange = vertexStagingBuffer->map(0, vertexBufferSize);

		std::copy(vertices.begin(), vertices.end(), static_cast<vec2<float32>*>(vertexMappedRange));

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

		const BufferMappedRange indexMappedRange = indexStagingBuffer->map(0, indexBufferSize);

		std::copy(indices.begin(), indices.end(), static_cast<uint16*>(indexMappedRange));

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

	void ComputeShaderDemoRenderer::render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = Application::get().getRenderPass();
		beginRenderPassDescriptor.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		beginRenderPassDescriptor.nextImageIndex = nextImageIndex;

		const ref<Swapchain>& swapchain = Application::get().getSwapchain();

		const uint32 width = swapchain->getExtentWidth();
		const uint32 height = swapchain->getExtentHeight();

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->bindPipeline(renderPipeline);
		commandEncoder->bindDescriptorSets(0);
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, sizeof(vec2<float32>));
		commandEncoder->setViewport({0.0f, static_cast<float32>(height)}, {static_cast<float32>(width), -static_cast<float32>(height)}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, width, height);
		commandEncoder->drawIndexed(6, 1, 0);
		commandEncoder->endRenderPass();
	}
}