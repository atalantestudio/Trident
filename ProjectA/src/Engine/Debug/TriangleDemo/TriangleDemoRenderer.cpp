#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Debug/TriangleDemo/TriangleDemoRenderer.hpp"

namespace ProjectA {
	TriangleDemoRenderer::TriangleDemoRenderer() {
		const ref<Device>& device = getDevice();

		// Create pipeline layout.
		{
			const PipelineLayoutDescriptor pipelineLayoutDescriptor;

			pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);
		}

		ref<ShaderModule> vertexShaderModule;

		// Create vertex shader module.
		{
			ShaderModuleDescriptor vertexShaderModuleDescriptor{};
			vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
			vertexShaderModuleDescriptor.entryPoint = "main";

			vertexShaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/TriangleDemo/Shader/TriangleDemo.vert.spv", vertexShaderModuleDescriptor);
		}

		ref<ShaderModule> fragmentShaderModule;

		// Create fragment shader module.
		{
			ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
			fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
			fragmentShaderModuleDescriptor.entryPoint = "main";

			fragmentShaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/TriangleDemo/Shader/TriangleDemo.frag.spv", fragmentShaderModuleDescriptor);
		}

		// Create render pipeline.
		{
			RenderPipelineDescriptor renderPipelineDescriptor;
			renderPipelineDescriptor.renderPass = Application::get().getRenderPass();
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
		}

		// Create vertex buffer.
		{
			chain<Vertex> vertices(3);
			vertices[0].position = {-0.5f, -0.5f};
			vertices[0].color = {1.0f, 0.0f, 0.0f};
			vertices[1].position = {0.0f,  0.5f};
			vertices[1].color = {0.0f, 1.0f, 0.0f};
			vertices[2].position = {0.5f, -0.5f};
			vertices[2].color = {0.0f, 0.0f, 1.0f};

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

			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, 0, 0, size);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			const ref<Device>& device = getDevice();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		// Create index buffer.
		{
			chain<uint16> indices(3);
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;

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

			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, 0, 0, size);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			const ref<Device>& device = getDevice();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}
	}

	void TriangleDemoRenderer::render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = Application::get().getRenderPass();
		beginRenderPassDescriptor.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		beginRenderPassDescriptor.nextImageIndex = nextImageIndex;

		const ref<Swapchain>& swapchain = Application::get().getSwapchain();

		const uint32 width = swapchain->getExtentWidth();
		const uint32 height = swapchain->getExtentHeight();

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->bindPipeline(renderPipeline);
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, sizeof(Vertex));
		commandEncoder->setViewport({0.0f, static_cast<float32>(height)}, {static_cast<float32>(width), -static_cast<float32>(height)}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, width, height);
		commandEncoder->drawIndexed(3, 1, 0);
		commandEncoder->endRenderPass();
	}
}