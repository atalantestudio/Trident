#include "pch.hpp"

#include "Engine/ECS/Component/Transform2DComponent.hpp"
#include "Engine/Loader/FileLoader.hpp"
#include "Engine/Renderer/Buffer/BufferUsage.hpp"
#include "Engine/Renderer/GUI/GUIRenderer.hpp"

namespace ProjectA {
	GUIRenderer::GUIRenderer() :
		cameraUniformBuffers(MAX_FRAMES_IN_FLIGHT),
		availableImageSemaphores(MAX_FRAMES_IN_FLIGHT),
		finishedRenderSemaphores(MAX_FRAMES_IN_FLIGHT),
		flightFences(MAX_FRAMES_IN_FLIGHT)
	{
		const RenderPassDescriptor renderPassDescriptor;

		renderPass = createRenderPass(renderPassDescriptor);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = renderPass;

		swapchain = createSwapchain(swapchainDescriptor);

		BufferDescriptor uniformBufferDescriptor{};
		uniformBufferDescriptor.size = sizeof(UniformBufferObject);
		uniformBufferDescriptor.usage = static_cast<uint32>(BufferUsage::UNIFORM);
		uniformBufferDescriptor.mapped = true;

		for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			cameraUniformBuffers[i] = createBuffer(uniformBufferDescriptor);
			availableImageSemaphores[i] = createSemaphore();
			finishedRenderSemaphores[i] = createSemaphore();
			flightFences[i] = createFence();
		}

		DescriptorSetLayoutBinding descriptorSetLayoutBinding{};
		descriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		descriptorSetLayoutBinding.stageFlags = static_cast<uint32>(ShaderStage::VERTEX);
		descriptorSetLayoutBinding.buffers = cameraUniformBuffers;

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings.emplace_back(descriptorSetLayoutBinding);

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts.emplace_back(descriptorSetLayout);

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
		renderPipelineDescriptor.vertexBufferLayout.add<vec3<float32>>(0, false);
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(1, InputRate::INSTANCE);
		renderPipelineDescriptor.vertexBufferLayout.add<mat3x3<float32>>(1, false);
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);

		const std::vector<vec3<float32>> positionVertexBufferData = {
			{0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{0.0f, 1.0f, 1.0f},
		};

		BufferDescriptor positionVertexBufferDescriptor{};
		positionVertexBufferDescriptor.size = sizeof(vec3<float32>) * positionVertexBufferData.size();
		positionVertexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::VERTEX);
		positionVertexBufferDescriptor.mapped = true;

		positionVertexBuffer = createBuffer(positionVertexBufferDescriptor);
		positionVertexBuffer->write(positionVertexBufferData.data(), 0, sizeof(vec3<float32>) * positionVertexBufferData.size());
		positionVertexBuffer->unmap();

		BufferDescriptor transformVertexBufferDescriptor{};
		transformVertexBufferDescriptor.size = sizeof(Transform2DComponent) * MAX_QUADS;
		transformVertexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::VERTEX);
		transformVertexBufferDescriptor.mapped = false;

		transformVertexBuffer = createBuffer(transformVertexBufferDescriptor);

		// Create index buffer.
		{
			const std::vector<uint16> indexBufferData = {
				0, 1, 2,
				0, 2, 3,
			};

			BufferDescriptor indexBufferDescriptor{};
			indexBufferDescriptor.size = sizeof(uint16) * indexBufferData.size();
			indexBufferDescriptor.usage = static_cast<uint32>(BufferUsage::INDEX);
			indexBufferDescriptor.mapped = true;

			indexBuffer = createBuffer(indexBufferDescriptor);
			indexBuffer->write(indexBufferData.data(), 0, sizeof(uint16) * indexBufferData.size());
			indexBuffer->unmap();
		}

		commandEncoder = createCommandEncoder();
	}

	void GUIRenderer::registerEntities(const ECS& ecs) {
		const ComponentPool<Transform2DComponent>& transformComponentPool = ecs.getComponentPool<Transform2DComponent>();

		if (transformComponentPool.componentCount >= MAX_QUADS) {
			throw std::runtime_error("Too many alive components.");
		}

		instanceCount = transformComponentPool.componentCount;

		const uint64 size = sizeof(Transform2DComponent) * instanceCount;

		transformVertexBuffer->map(0, size);
		transformVertexBuffer->write(transformComponentPool.components.data(), 0, size);
		transformVertexBuffer->unmap();
	}

	void GUIRenderer::render() {
		updateCameraUniform();

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
		commandEncoder->bindDescriptorSets(0);
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, positionVertexBuffer, sizeof(vec3<float32>));
		commandEncoder->bindVertexBuffer(1, transformVertexBuffer, sizeof(Transform2DComponent));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
		commandEncoder->drawIndexed(6, instanceCount, 0);
		commandEncoder->endRenderPass();

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->submit(commandBuffer, availableImageSemaphores[frameIndex], finishedRenderSemaphores[frameIndex], flightFences[frameIndex]);
		device->present(swapchain, finishedRenderSemaphores[frameIndex], nextImageIndex);
		device->waitIdle();

		frameIndex++;
		frameIndex %= MAX_FRAMES_IN_FLIGHT;
	}

	void GUIRenderer::updateCameraUniform() const {
		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		UniformBufferObject cameraUniformBufferObject;
		cameraUniformBufferObject.projection = {
			2.0f / width, 0.0f, 0.0f,
			0.0f, -2.0f / height, 0.0f,
			-1.0f, 1.0f, 1.0f,
		};

		cameraUniformBuffers[frameIndex]->write(&cameraUniformBufferObject, 0, sizeof(UniformBufferObject));
	}

	void GUIRenderer::onEvent(Event& event) {}
}