#include "pch.hpp"

#include "Engine/Filesystem/FileLoader.hpp"
#include "Engine/Renderer/TriangleDemo/TriangleDemoRenderer.hpp"

namespace ProjectA {
	TriangleDemoRenderer::TriangleDemoRenderer() :
		availableImageSemaphores(MAX_FRAMES_IN_FLIGHT),
		finishedRenderSemaphores(MAX_FRAMES_IN_FLIGHT),
		flightFences(MAX_FRAMES_IN_FLIGHT)
	{
		const RenderPassDescriptor renderPassDescriptor;

		renderPass = createRenderPass(renderPassDescriptor);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = renderPass;

		swapchain = createSwapchain(swapchainDescriptor);

		const PipelineLayoutDescriptor pipelineLayoutDescriptor;

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		std::string vertexShaderModulePath;
		std::string fragmentShaderModulePath;

		switch (getGraphicsApi()) {
			case GraphicsAPI::NONE:
				throw std::runtime_error("TriangleDemo must use either OpenGL or Vulkan.");
			case GraphicsAPI::OPENGL:
				vertexShaderModulePath = "../ProjectA/src/Engine/Shader/TriangleDemo.vert";
				fragmentShaderModulePath = "../ProjectA/src/Engine/Shader/TriangleDemo.frag";

				break;
			case GraphicsAPI::VULKAN:
				vertexShaderModulePath = "../ProjectA/src/Engine/Shader/TriangleDemo.vert.spv";
				fragmentShaderModulePath = "../ProjectA/src/Engine/Shader/TriangleDemo.frag.spv";

				break;
		}

		const std::string vertexShaderModuleCode = FileLoader::load(vertexShaderModulePath);
		const std::string fragmentShaderModuleCode = FileLoader::load(fragmentShaderModulePath);

		const ref<ShaderModule> vertexShaderModule = createShaderModule(ShaderStage::VERTEX, vertexShaderModuleCode);
		const ref<ShaderModule> fragmentShaderModule = createShaderModule(ShaderStage::FRAGMENT, fragmentShaderModuleCode);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.swapchain = swapchain;
		renderPipelineDescriptor.renderPass = renderPass;
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(0, InputRate::VERTEX);
		renderPipelineDescriptor.vertexBufferLayout.add<vec2<float32>>(0, false);
		renderPipelineDescriptor.vertexBufferLayout.add<vec3<float32>>(0, false);
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);

		// Create vertex buffer.
		{
			chain<Vertex> vertices(3);
			vertices[0].position = {-0.5f, -0.5f};
			vertices[0].color = {1.0f, 0.0f, 0.0f};
			vertices[1].position = {0.0f,  0.5f};
			vertices[1].color = {0.0f, 1.0f, 0.0f};
			vertices[2].position = {0.5f, -0.5f};
			vertices[2].color = {0.0f, 0.0f, 1.0f};

			BufferDescriptor vertexBufferDescriptor{};
			vertexBufferDescriptor.size = sizeof(Vertex) * vertices.size();
			vertexBufferDescriptor.usage = BufferUsage::VERTEX;

			vertexBuffer = createBuffer(vertexBufferDescriptor);

			BufferMappedRange mappedRange = vertexBuffer->map(0, vertexBufferDescriptor.size);

			std::copy(&vertices[0], &vertices[0] + vertexBufferDescriptor.size, (Vertex*)mappedRange);

			vertexBuffer->unmap();
		}

		// Create index buffer.
		{
			chain<uint16> indices(3);
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;

			BufferDescriptor indexBufferDescriptor{};
			indexBufferDescriptor.size = sizeof(uint16) * indices.size();
			indexBufferDescriptor.usage = BufferUsage::INDEX;

			indexBuffer = createBuffer(indexBufferDescriptor);

			BufferMappedRange mappedRange = indexBuffer->map(0, indexBufferDescriptor.size);

			std::memcpy(mappedRange, indices.data(), indexBufferDescriptor.size);

			indexBuffer->unmap();
		}

		commandEncoder = createCommandEncoder();

		for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			availableImageSemaphores[i] = createSemaphore();
			finishedRenderSemaphores[i] = createSemaphore();
			flightFences[i] = createFence();
		}
	}

	void TriangleDemoRenderer::render() {
		const ref<Device>& device = getDevice();

		device->waitForFence(flightFences[frameIndex]);

		const uint32 nextImageIndex = device->acquireNextImage(swapchain, availableImageSemaphores[frameIndex]);

		device->resetFence(flightFences[frameIndex]);

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = renderPass;
		beginRenderPassDescriptor.swapchain = swapchain;
		beginRenderPassDescriptor.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		beginRenderPassDescriptor.nextImageIndex = nextImageIndex;

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		commandEncoder->reset();
		commandEncoder->begin();
		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->bindRenderPipeline(renderPipeline);
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, sizeof(Vertex));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
		commandEncoder->drawIndexed(3, 1, 0);
		commandEncoder->endRenderPass();

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->submit(commandBuffer, availableImageSemaphores[frameIndex], finishedRenderSemaphores[frameIndex], flightFences[frameIndex]);
		device->present(swapchain, finishedRenderSemaphores[frameIndex], nextImageIndex);
		device->waitIdle();

		frameIndex++;
		frameIndex %= MAX_FRAMES_IN_FLIGHT;
	}

	void TriangleDemoRenderer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<WindowResizeEvent>(BIND(TriangleDemoRenderer::onResize));
	}

	bool TriangleDemoRenderer::onResize(const WindowResizeEvent& event) {
		return true;
	}
}