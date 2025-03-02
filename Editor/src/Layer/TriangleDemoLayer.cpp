#include "TriangleDemoLayer.hpp"

namespace ProjectA {
	TriangleDemoLayer::TriangleDemoLayer() : Layer("TriangleDemo") {}

	void TriangleDemoLayer::bind() {
		const RenderPassDescriptor renderPassDescriptor{};

		renderPass = Renderer::createRenderPass(renderPassDescriptor);

		SwapchainDescriptor swapchainDescriptor{};
		swapchainDescriptor.renderPass = renderPass;

		swapchain = Renderer::createSwapchain(swapchainDescriptor);

		const PipelineLayoutDescriptor pipelineLayoutDescriptor;

		pipelineLayout = Renderer::createPipelineLayout(pipelineLayoutDescriptor);

		const std::string vertexShaderModuleCode = FileLoader::load("assets/shaders/vert.spv");
		const std::string fragmentShaderModuleCode = FileLoader::load("assets/shaders/frag.spv");

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.swapchain = swapchain;
		renderPipelineDescriptor.renderPass = renderPass;
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexBufferLayout.add<vec3<float32>>(0, false);
		renderPipelineDescriptor.vertexBufferLayout.add<vec3<float32>>(0, false);
		renderPipelineDescriptor.vertexShaderModule = Renderer::createShaderModule(ShaderStage::VERTEX, vertexShaderModuleCode);
		renderPipelineDescriptor.fragmentShaderModule = Renderer::createShaderModule(ShaderStage::FRAGMENT, fragmentShaderModuleCode);
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;

		renderPipeline = Renderer::createRenderPipeline(renderPipelineDescriptor);

		const std::vector<TriangleDemoVertex> vertexBufferData = {
			{{-0.5f, -0.5f,  0.0f},    {1.0f, 0.0f, 0.0f}},
			{{ 0.0f,  0.5f,  0.0f},    {0.0f, 1.0f, 0.0f}},
			{{ 0.5f, -0.5f,  0.0f},    {0.0f, 0.0f, 1.0f}},
		};

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = sizeof(vertexBufferData) * sizeof(TriangleDemoVertex);
		vertexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::VERTEX);

		vertexBuffer = Renderer::createBuffer(vertexBufferDescriptor);
		vertexBuffer->write(vertexBufferData.data(), vertexBufferData.size() * sizeof(TriangleDemoVertex));

		commandBuffer = Renderer::createCommandBuffer();
	}

	void TriangleDemoLayer::unbind() {}

	void TriangleDemoLayer::update(float32 deltaTime) {
		const ref<Device>& device = Renderer::getDevice();

		device->waitForFences();

		const uint32 nextImageIndex = device->acquireNextImage(swapchain);

		device->resetFences();

		commandBuffer->reset();

		commandBuffer->begin();

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = renderPass;
		beginRenderPassDescriptor.swapchain = swapchain;
		beginRenderPassDescriptor.clearColor = {0.1f, 0.1f, 0.2f, 1.0f};
		beginRenderPassDescriptor.nextImageIndex = nextImageIndex;

		commandBuffer->beginRenderPass(beginRenderPassDescriptor);

		commandBuffer->bindRenderPipeline(renderPipeline);

		commandBuffer->bindVertexBuffer(vertexBuffer);

		commandBuffer->setViewport({0.0f, static_cast<float32>(swapchain->getExtentHeight())}, {static_cast<float32>(swapchain->getExtentWidth()), -static_cast<float32>(swapchain->getExtentHeight())}, 0.0f, 1.0f);

		commandBuffer->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());

		commandBuffer->draw(3, 1, 0);

		commandBuffer->endRenderPass();

		commandBuffer->end();

		device->submit(commandBuffer);

		device->present(swapchain, nextImageIndex);

		device->waitIdle();
	}

	void TriangleDemoLayer::renderImGui() {}

	void TriangleDemoLayer::onEvent(Event& event) {}
}