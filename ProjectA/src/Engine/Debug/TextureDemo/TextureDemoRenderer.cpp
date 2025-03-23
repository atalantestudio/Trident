#include "pch.hpp"

#include "Engine/Debug/TextureDemo/TextureDemoRenderer.hpp"
#include "Engine/Filesystem/FileLoader.hpp"
#include "Engine/Filesystem/PNGFileLoader.hpp"

namespace ProjectA {
	TextureDemoRenderer::TextureDemoRenderer() :
		availableImageSemaphores(MAX_FRAMES_IN_FLIGHT),
		finishedRenderSemaphores(MAX_FRAMES_IN_FLIGHT),
		flightFences(MAX_FRAMES_IN_FLIGHT)
	{
		const RenderPassDescriptor renderPassDescriptor;

		renderPass = createRenderPass(renderPassDescriptor);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = renderPass;

		swapchain = createSwapchain(swapchainDescriptor);

		// Create texture.
		{
			// TODO:
			// - checkerboard.png (colorType = 2, no alpha)

			const PNGFile file = PNGFileLoader::load("../ProjectA/src/Engine/Debug/TextureDemo/Asset/Zelda.png");
			const uint64 size = sizeof(uint8) * file.data.size();

			BufferDescriptor stagingBufferDescriptor{};
			stagingBufferDescriptor.size = size;
			stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

			const ref<Buffer> stagingBuffer = createBuffer(stagingBufferDescriptor);

			const BufferMappedRange mappedRange = stagingBuffer->map(0, size);

			std::copy(file.data.begin(), file.data.end(), static_cast<uint8*>(mappedRange));

			stagingBuffer->unmap();

			TextureDescriptor textureDescriptor{};
			textureDescriptor.type = TextureType::TEXTURE_2D;
			textureDescriptor.format = TextureFormat::R8G8B8A8_SRGB;
			textureDescriptor.usage = TextureUsage::TRANSFER_DST | TextureUsage::SAMPLED;
			textureDescriptor.width = file.width;
			textureDescriptor.height = file.height;
			textureDescriptor.depth = 1;
			textureDescriptor.mipLevelCount = 1;
			textureDescriptor.arrayLayerCount = 1;
			textureDescriptor.sampleCount = 1;

			texture = createTexture(textureDescriptor);

			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

			const ref<CommandEncoder> commandEncoder = createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->setTextureMemoryBarrier(texture, TextureLayout::UNDEFINED, TextureLayout::TRANSFER_DST_OPTIMAL);
			commandEncoder->copyBufferToTexture(stagingBuffer, texture, file.width, file.height, 0);
			commandEncoder->setTextureMemoryBarrier(texture, TextureLayout::TRANSFER_DST_OPTIMAL, TextureLayout::SHADER_READ_ONLY_OPTIMAL);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			const ref<Device>& device = getDevice();

			device->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		// Create texture view.
		{
			const TextureViewDescriptor textureViewDescriptor;

			textureView = texture->createView(textureViewDescriptor);
		}

		// Create sampler.
		{
			SamplerDescriptor samplerDescriptor{};
			samplerDescriptor.magFilter = SamplerFilter::NEAREST;
			samplerDescriptor.minFilter = SamplerFilter::NEAREST;

			sampler = createSampler(samplerDescriptor);
		}

		DescriptorSetLayoutBinding textureDescriptorSetLayoutBinding{};
		textureDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		textureDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		textureDescriptorSetLayoutBinding.samplers = {sampler};
		textureDescriptorSetLayoutBinding.textureViews = {textureView};

		DescriptorSetLayout textureDescriptorSetLayout{};
		textureDescriptorSetLayout.bindings.emplace_back(textureDescriptorSetLayoutBinding);

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts.emplace_back(textureDescriptorSetLayout);

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		std::string vertexShaderModulePath;
		std::string fragmentShaderModulePath;

		switch (getGraphicsApi()) {
			case GraphicsAPI::NONE:
				throw std::runtime_error("TextureDemo must use either OpenGL or Vulkan.");
			case GraphicsAPI::OPENGL:
				vertexShaderModulePath = "../ProjectA/src/Engine/Debug/TextureDemo/Shader/TextureDemo.vert";
				fragmentShaderModulePath = "../ProjectA/src/Engine/Debug/TextureDemo/Shader/TextureDemo.frag";

				break;
			case GraphicsAPI::VULKAN:
				vertexShaderModulePath = "../ProjectA/src/Engine/Debug/TextureDemo/Shader/TextureDemo.vert.spv";
				fragmentShaderModulePath = "../ProjectA/src/Engine/Debug/TextureDemo/Shader/TextureDemo.frag.spv";

				break;
		}

		const chain<uint8> vertexShaderModuleCode = FileLoader::loadByteBuffer(vertexShaderModulePath);
		const chain<uint8> fragmentShaderModuleCode = FileLoader::loadByteBuffer(fragmentShaderModulePath);

		const ref<ShaderModule> vertexShaderModule = createShaderModule(ShaderStage::VERTEX, vertexShaderModuleCode);
		const ref<ShaderModule> fragmentShaderModule = createShaderModule(ShaderStage::FRAGMENT, fragmentShaderModuleCode);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.swapchain = swapchain;
		renderPipelineDescriptor.renderPass = renderPass;
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

		// Create vertex buffer.
		{
			// Texture coordinates are reversed manually.
			chain<Vertex> vertices(4);
			vertices[0].position = {-1.0f, -1.0f};
			vertices[0].uv = {0.0f, 1.0f};
			vertices[1].position = {-1.0f, 1.0f};
			vertices[1].uv = {0.0f, 0.0f};
			vertices[2].position = {1.0f, 1.0f};
			vertices[2].uv = {1.0f, 0.0f};
			vertices[3].position = {1.0f, -1.0f};
			vertices[3].uv = {1.0f, 1.0f};

			const uint64 size = sizeof(Vertex) * vertices.size();

			BufferDescriptor stagingBufferDescriptor{};
			stagingBufferDescriptor.size = size;
			stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

			const ref<Buffer> stagingBuffer = createBuffer(stagingBufferDescriptor);

			const BufferMappedRange mappedRange = stagingBuffer->map(0, size);

			std::copy(vertices.begin(), vertices.end(), static_cast<Vertex*>(mappedRange));

			stagingBuffer->unmap();

			BufferDescriptor vertexBufferDescriptor{};
			vertexBufferDescriptor.size = size;
			vertexBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX;

			vertexBuffer = createBuffer(vertexBufferDescriptor);

			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

			const ref<CommandEncoder> commandEncoder = createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, 0, 0, size);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			const ref<Device>& device = getDevice();

			device->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		// Create index buffer.
		{
			chain<uint16> indices(6);
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 0;
			indices[4] = 2;
			indices[5] = 3;

			const uint64 size = sizeof(uint16) * indices.size();

			BufferDescriptor stagingBufferDescriptor{};
			stagingBufferDescriptor.size = size;
			stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

			const ref<Buffer> stagingBuffer = createBuffer(stagingBufferDescriptor);

			const BufferMappedRange mappedRange = stagingBuffer->map(0, size);

			std::copy(indices.begin(), indices.end(), static_cast<uint16*>(mappedRange));

			stagingBuffer->unmap();

			BufferDescriptor indexBufferDescriptor{};
			indexBufferDescriptor.size = size;
			indexBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX;

			indexBuffer = createBuffer(indexBufferDescriptor);

			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

			const ref<CommandEncoder> commandEncoder = createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, 0, 0, size);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			const ref<Device>& device = getDevice();

			device->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		// Create command encoder.
		{
			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

			commandEncoder = createCommandEncoder(commandBufferDescriptor);
		}

		for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			availableImageSemaphores[i] = createSemaphore();
			finishedRenderSemaphores[i] = createSemaphore();
			flightFences[i] = createFence();
		}
	}

	void TextureDemoRenderer::render() {
		const ref<Device>& device = getDevice();

		device->waitForFence(flightFences[frameIndex]);

		const uint32 nextImageIndex = device->acquireNextImage(swapchain, availableImageSemaphores[frameIndex]);

		device->resetFence(flightFences[frameIndex]);

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = renderPass;
		beginRenderPassDescriptor.swapchain = swapchain;
		beginRenderPassDescriptor.clearColor = {1.0f, 1.0f, 1.0f, 1.0f};
		beginRenderPassDescriptor.nextImageIndex = nextImageIndex;

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		commandEncoder->reset();
		commandEncoder->begin();
		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->bindRenderPipeline(renderPipeline);
		commandEncoder->bindDescriptorSets(0);
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, sizeof(Vertex));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
		commandEncoder->drawIndexed(6, 1, 0);
		commandEncoder->endRenderPass();

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->submit(commandBuffer, availableImageSemaphores[frameIndex], finishedRenderSemaphores[frameIndex], flightFences[frameIndex]);
		device->present(swapchain, finishedRenderSemaphores[frameIndex], nextImageIndex);
		device->waitIdle();

		frameIndex++;
		frameIndex %= MAX_FRAMES_IN_FLIGHT;
	}

	void TextureDemoRenderer::onEvent(Event& event) {}
}