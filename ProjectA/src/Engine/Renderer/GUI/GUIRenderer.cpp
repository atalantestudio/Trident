#include "pch.hpp"

#include "Engine/ECS/Component/Transform2DComponent.hpp"
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
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(0, InputRate::VERTEX);
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(1, InputRate::INSTANCE);
		renderPipelineDescriptor.vertexBufferLayout.add<vec3<float32>>(0, false);
		renderPipelineDescriptor.vertexBufferLayout.add<mat3x3<float32>>(1, false);
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);

		const std::vector<vec3<float32>> positionVertexBufferData = {
			{0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 1.0f},
		};

		BufferDescriptor positionVertexBufferDescriptor{};
		positionVertexBufferDescriptor.size = sizeof(vec3<float32>) * positionVertexBufferData.size();
		positionVertexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::VERTEX);

		positionVertexBuffer = createBuffer(positionVertexBufferDescriptor);
		positionVertexBuffer->write(positionVertexBufferData.data(), 0, sizeof(vec3<float32>) * positionVertexBufferData.size());

		BufferDescriptor transformVertexBufferDescriptor{};
		transformVertexBufferDescriptor.size = sizeof(Transform2DComponent) * MAX_QUADS;
		transformVertexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::VERTEX);

		transformVertexBuffer = createBuffer(transformVertexBufferDescriptor);

		const std::vector<uint16> indexBufferData = {
			0, 1, 2,
			0, 2, 3,
		};

		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = sizeof(uint16) * indexBufferData.size();
		indexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::INDEX);

		indexBuffer = createBuffer(indexBufferDescriptor);
		indexBuffer->write(indexBufferData.data(), 0, sizeof(uint16) * indexBufferData.size());

		commandEncoder = createCommandEncoder();
	}

	void GUIRenderer::registerEntities(const ECS& ecs) {
		const ComponentPool<Transform2DComponent>& transformComponentPool = ecs.getComponentPool<Transform2DComponent>();

		if (transformComponentPool.componentCount >= MAX_QUADS) {
			throw std::runtime_error("Too many alive components.");
		}

		const uint64 size = sizeof(Transform2DComponent);

		uint64 offset = 0;

		for (uint32 i = 0; i < transformComponentPool.componentCount; i++) {
			const Transform2DComponent& transformComponent = transformComponentPool.components[i];

			transformVertexBuffer->write(&transformComponent, offset, size);

			offset += size;
		}

		instanceCount = transformComponentPool.componentCount;
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
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, positionVertexBuffer, sizeof(vec3<float32>));
		commandEncoder->bindVertexBuffer(1, transformVertexBuffer, sizeof(Transform2DComponent));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
		commandEncoder->drawIndexed(6, instanceCount, 0);
		commandEncoder->endRenderPass();

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->submit(commandBuffer);
		device->present(swapchain, nextImageIndex);
		device->waitIdle();
	}
}