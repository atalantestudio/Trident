#include "pch.hpp"

#include "DebugPhysics2DRenderer.hpp"

namespace ProjectA {
	void DebugPhysics2DRenderer::registerScene(const Scene2D& scene) {
		constexpr uint64 vertexBufferSize = 3 * sizeof(vec2<float32>);

		const ref<Device> device = getDevice();

		const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

		vec2<float32>* vertices = vertexStagingBuffer->map<vec2<float32>>(0, vertexBufferSize);

		vertices[0] = {-0.5f, -0.5f};
		vertices[1] = { 0.0f,  0.5f};
		vertices[2] = { 0.5f, -0.5f};

		vertexStagingBuffer->unmap();

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = vertexBufferSize;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

		vertexBuffer = createBuffer(vertexBufferDescriptor);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, 0, 0, vertexBufferSize);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();

		const PipelineLayoutDescriptor pipelineLayoutDescriptor;

		renderPipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D.frag.spv", fragmentShaderModuleDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec2<float32>>(false);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.renderPass = getMainRenderPass();
		renderPipelineDescriptor.pipelineLayout = renderPipelineLayout;
		renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);
	}

	void DebugPhysics2DRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		commandEncoder->bindRenderPipeline(renderPipeline);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(vec2<float32>));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
		commandEncoder->draw(3, 1, 0);
	}
}