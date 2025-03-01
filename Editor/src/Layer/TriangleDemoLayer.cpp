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

		const std::string vertexShaderModuleCode = FileLoader::load("assets/shaders/TriangleDemo.vert.spv");
		const std::string fragmentShaderModuleCode = FileLoader::load("assets/shaders/TriangleDemo.frag.spv");

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.swapchain = swapchain;
		renderPipelineDescriptor.renderPass = renderPass;
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexBufferLayout.add<vec2<float32>>(0, false);
		renderPipelineDescriptor.vertexShaderModule = Renderer::createShaderModule(ShaderStage::VERTEX, vertexShaderModuleCode);
		renderPipelineDescriptor.fragmentShaderModule = Renderer::createShaderModule(ShaderStage::FRAGMENT, fragmentShaderModuleCode);
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_STRIP;

		renderPipeline = Renderer::createRenderPipeline(renderPipelineDescriptor);

		constexpr float32 vertexBufferData[] = {
			-0.5, -0.5,
			 0.5, -0.5,
			 0.0,  0.5,
		};

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = sizeof(float32) * 6;
		vertexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::VERTEX);

		vertexBuffer = Renderer::createBuffer(vertexBufferDescriptor);
		vertexBuffer->write(vertexBufferData);

		commandBuffer = Renderer::createCommandBuffer();
	}

	void TriangleDemoLayer::unbind() {}

	void TriangleDemoLayer::update(float32 deltaTime) {
		const vec2<float32> size = {1280.0f, 720.0f};

		commandBuffer->begin();

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = renderPass;
		beginRenderPassDescriptor.swapchain = swapchain;
		beginRenderPassDescriptor.clearColor = {1.0f, 0.2f, 0.0f, 1.0f};
		beginRenderPassDescriptor.depth = 0.0f;
		beginRenderPassDescriptor.stencil = 0;

		commandBuffer->beginRenderPass(beginRenderPassDescriptor);

		commandBuffer->bindRenderPipeline(renderPipeline);

		commandBuffer->bindVertexBuffer(vertexBuffer);

		commandBuffer->setViewport({0.0f, 0.0f}, size, 0.0f, 1.0f);

		commandBuffer->setScissor(0, 0, 1280, 720);

		commandBuffer->draw(3, 1, 0);

		commandBuffer->endRenderPass();

		commandBuffer->end();
	}

	void TriangleDemoLayer::renderImGui() {}

	void TriangleDemoLayer::onEvent(Event& event) {}
}