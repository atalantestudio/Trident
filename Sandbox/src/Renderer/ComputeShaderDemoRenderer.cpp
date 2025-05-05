#include "pch.hpp"

#include "Engine/Application.hpp"

#include "ComputeShaderDemoRenderer.hpp"

namespace ProjectA {
	ComputeShaderDemoRenderer::ComputeShaderDemoRenderer() {
		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		imageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::STORAGE;
		imageDescriptor.width = 64;
		imageDescriptor.height = 64;
		imageDescriptor.depth = 1;
		imageDescriptor.mipLevelCount = 1;
		imageDescriptor.arrayLayerCount = 1;
		imageDescriptor.sampleCount = 1;

		image = createImage(imageDescriptor);

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->setImageMemoryBarrier(image, ImageLayout::UNDEFINED, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		SamplerDescriptor samplerDescriptor{};
		samplerDescriptor.magFilter = SamplerFilter::NEAREST;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		sampler = createSampler(samplerDescriptor);

		const ImageViewDescriptor imageViewDescriptor;

		imageView = image->createView(imageViewDescriptor);

		DescriptorSetLayoutBinding computeDescriptorSetLayoutBinding;
		computeDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		computeDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		computeDescriptorSetLayoutBinding.samplers = {sampler};
		computeDescriptorSetLayoutBinding.imageViews = {imageView};

		DescriptorSetLayout computeDescriptorSetLayout;
		computeDescriptorSetLayout.bindings.emplace_back(computeDescriptorSetLayoutBinding);

		PipelineLayoutDescriptor computePipelineLayoutDescriptor;
		computePipelineLayoutDescriptor.layouts.emplace_back(computeDescriptorSetLayout);

		const ref<PipelineLayout> computePipelineLayout = createPipelineLayout(computePipelineLayoutDescriptor);

		ShaderModuleDescriptor computeShaderModuleDescriptor{};
		computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		computeShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> computeShaderModule = loadShaderModule("Asset/Shader/Compute.comp.spv", computeShaderModuleDescriptor);

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
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->dispatchGroups(2, 2, 1);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->setImageMemoryBarrier(image, ImageLayout::GENERAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		DescriptorSetLayoutBinding descriptorSetLayoutBinding;
		descriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		descriptorSetLayoutBinding.samplers = {sampler};
		descriptorSetLayoutBinding.imageViews = {imageView};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings.emplace_back(descriptorSetLayoutBinding);

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts.emplace_back(descriptorSetLayout);

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("Asset/Shader/Compute.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Asset/Shader/Compute.frag.spv", fragmentShaderModuleDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
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

		constexpr uint64 indexBufferSize = sizeof(uint16) * 6;

		const ref<Buffer> indexStagingBuffer = createStagingBuffer(indexBufferSize);

		uint16* indices = indexStagingBuffer->map<uint16>(0, indexBufferSize);

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;

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

	void ComputeShaderDemoRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		const ref<Swapchain>& swapchain = Application::get().getSwapchain();

		const uint32 width = swapchain->getExtentWidth();
		const uint32 height = swapchain->getExtentHeight();

		commandEncoder->bindRenderPipeline(renderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(vec2<float32>));
		commandEncoder->setViewport({0.0f, static_cast<float32>(height)}, {static_cast<float32>(width), -static_cast<float32>(height)}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, width, height);
		commandEncoder->drawIndexed(6, 1, 0);
	}
}