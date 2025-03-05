#include "pch.hpp"

#include "Engine/Loader/FileLoader.hpp"
#include "Engine/Renderer/Buffer/BufferUsage.hpp"
#include "Engine/Renderer/GUI/GUIRenderer.hpp"

namespace ProjectA {
	GUIRenderer::GUIRenderer() {
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
				vertexShaderModulePath = "../ProjectA/src/Engine/Shader/GUI.vert";
				fragmentShaderModulePath = "../ProjectA/src/Engine/Shader/GUI.frag";

				break;
			case GraphicsAPI::VULKAN:
				vertexShaderModulePath = "../ProjectA/src/Engine/Shader/GUI.vert.spv";
				fragmentShaderModulePath = "../ProjectA/src/Engine/Shader/GUI.frag.spv";

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
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(0, InputRate::INSTANCE);
		renderPipelineDescriptor.vertexBufferLayout.add<mat3x3<float32>>(0, false);
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);

		const mat3x3<float32> world0 =
			mat3x3<float32>::scaling({64.0f, 64.0f}) *
			mat3x3<float32>::translation({16.0f, 0.0f});
		const mat3x3<float32> world1 =
			mat3x3<float32>::scaling({128.0f, 48.0f}) *
			mat3x3<float32>::translation({16.0f, 80.0f});
		const mat3x3<float32> world2 =
			mat3x3<float32>::scaling({256.0f, 512.0f}) *
			mat3x3<float32>::translation({80.0f, 128.0f});

		const std::vector<mat3x3<float32>> vertexBufferData = {
			world0,
			world1,
			world2,
		};

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = sizeof(mat3x3<float32>) * vertexBufferData.size();
		vertexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::VERTEX);

		vertexBuffer = createBuffer(vertexBufferDescriptor);
		vertexBuffer->write(vertexBufferData.data(), sizeof(mat3x3<float32>) * vertexBufferData.size());

		/*const std::vector<uint16> indexBufferData = {
			0, 1, 2,
			0, 2, 3,
		};

		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = sizeof(uint16) * indexBufferData.size();
		indexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::INDEX);

		indexBuffer = createBuffer(indexBufferDescriptor);
		indexBuffer->write(indexBufferData.data(), sizeof(uint16) * indexBufferData.size());*/

		commandEncoder = createCommandEncoder();
	}

	void GUIRenderer::render() const {
		const ref<Device>& device = getDevice();

		device->waitForFences();

		const uint32 nextImageIndex = device->acquireNextImage(swapchain);

		device->resetFences();

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = renderPass;
		beginRenderPassDescriptor.swapchain = swapchain;
		beginRenderPassDescriptor.clearColor = {0.1f, 0.1f, 0.2f, 1.0f};
		beginRenderPassDescriptor.nextImageIndex = nextImageIndex;

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		commandEncoder->reset();
		commandEncoder->begin();
		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->bindRenderPipeline(renderPipeline);
		//commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(vertexBuffer, sizeof(mat3x3<float32>));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
		commandEncoder->draw(3, 3, 0);
		commandEncoder->endRenderPass();

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->submit(commandBuffer);
		device->present(swapchain, nextImageIndex);
		device->waitIdle();
	}
}