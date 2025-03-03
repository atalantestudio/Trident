#include "pch.hpp"

#include "Engine/Loader/FileLoader.hpp"
#include "Engine/Renderer/Buffer/BufferUsage.hpp"
#include "Engine/Renderer/TriangleDemo/TriangleDemoRenderer.hpp"

namespace ProjectA {
	TriangleDemoRenderer::TriangleDemoRenderer() {
		const RenderPassDescriptor renderPassDescriptor;

		renderPass = Renderer::createRenderPass(renderPassDescriptor);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = renderPass;

		swapchain = Renderer::createSwapchain(swapchainDescriptor);

		const PipelineLayoutDescriptor pipelineLayoutDescriptor;

		pipelineLayout = Renderer::createPipelineLayout(pipelineLayoutDescriptor);

		const std::string vertexShaderModuleCode = FileLoader::load("../ProjectA/src/Engine/Shader/TriangleDemo.vert.spv");
		const std::string fragmentShaderModuleCode = FileLoader::load("../ProjectA/src/Engine/Shader/TriangleDemo.frag.spv");

		const ref<ShaderModule> vertexShaderModule = Renderer::createShaderModule(ShaderStage::VERTEX, vertexShaderModuleCode);
		const ref<ShaderModule> fragmentShaderModule = Renderer::createShaderModule(ShaderStage::FRAGMENT, fragmentShaderModuleCode);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.swapchain = swapchain;
		renderPipelineDescriptor.renderPass = renderPass;
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexBufferLayout.add<vec3<float32>>(0, false); // Position
		renderPipelineDescriptor.vertexBufferLayout.add<vec3<float32>>(0, false); // Color
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;

		renderPipeline = Renderer::createRenderPipeline(renderPipelineDescriptor);

		const std::vector<Vertex> vertexBufferData = {
			{{-0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}},
			{{ 0.0f,  0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}},
			{{ 0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}},
		};

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = sizeof(Vertex) * vertexBufferData.size();
		vertexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::VERTEX);

		vertexBuffer = Renderer::createBuffer(vertexBufferDescriptor);
		vertexBuffer->write(vertexBufferData.data(), sizeof(Vertex) * vertexBufferData.size());

		commandBuffer = Renderer::createCommandBuffer();
	}

	void TriangleDemoRenderer::render() const {
		const ref<Device>& device = Renderer::getDevice();

		device->waitForFences();

		const uint32 nextImageIndex = device->acquireNextImage(swapchain);

		device->resetFences();

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = renderPass;
		beginRenderPassDescriptor.swapchain = swapchain;
		beginRenderPassDescriptor.clearColor = {0.1f, 0.1f, 0.2f, 1.0f};
		beginRenderPassDescriptor.nextImageIndex = nextImageIndex;

		const vec2<float32> origin = {
			0.0f,
			static_cast<float32>(swapchain->getExtentHeight()),
		};

		const vec2<float32> size = {
			static_cast<float32>(swapchain->getExtentWidth()),
			-static_cast<float32>(swapchain->getExtentHeight()),
		};

		commandBuffer->reset();
		commandBuffer->begin();
		commandBuffer->beginRenderPass(beginRenderPassDescriptor);
		commandBuffer->bindRenderPipeline(renderPipeline);
		commandBuffer->bindVertexBuffer(vertexBuffer);
		commandBuffer->setViewport(origin, size, 0.0f, 1.0f);
		commandBuffer->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
		commandBuffer->draw(3, 1, 0);
		commandBuffer->endRenderPass();
		commandBuffer->end();

		device->submit(commandBuffer);
		device->present(swapchain, nextImageIndex);
		device->waitIdle();
	}
}