#include "pch.hpp"

#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/Filesystem/FileLoader.hpp"
#include "Engine/Renderer/Buffer/BufferUsage.hpp"
#include "Engine/Renderer/GUI/GUIRenderer.hpp"

namespace ProjectA {
	GUIRenderer::GUIRenderer() :
		cameraUniformBuffers(MAX_FRAMES_IN_FLIGHT),
		cameraUniformBufferMappedRanges(MAX_FRAMES_IN_FLIGHT),
		availableImageSemaphores(MAX_FRAMES_IN_FLIGHT),
		finishedRenderSemaphores(MAX_FRAMES_IN_FLIGHT),
		flightFences(MAX_FRAMES_IN_FLIGHT)
	{
		const RenderPassDescriptor renderPassDescriptor;

		renderPass = createRenderPass(renderPassDescriptor);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = renderPass;

		swapchain = createSwapchain(swapchainDescriptor);

		BufferDescriptor cameraUniformBufferDescriptor{};
		cameraUniformBufferDescriptor.size = sizeof(UniformBufferObject);
		cameraUniformBufferDescriptor.usage = BufferUsage::UNIFORM;

		for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			cameraUniformBuffers[i] = createBuffer(cameraUniformBufferDescriptor);
			cameraUniformBufferMappedRanges[i] = cameraUniformBuffers[i]->map(0, cameraUniformBufferDescriptor.size);
			availableImageSemaphores[i] = createSemaphore();
			finishedRenderSemaphores[i] = createSemaphore();
			flightFences[i] = createFence();
		}

		// Create color storage buffer.
		{
			BufferDescriptor colorStorageBufferDescriptor{};
			colorStorageBufferDescriptor.size = sizeof(vec4<float32>) * MAX_COLORS;
			colorStorageBufferDescriptor.usage = BufferUsage::STORAGE;

			colorStorageBuffer = createBuffer(colorStorageBufferDescriptor);
		}

		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding{};
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = cameraUniformBuffers;

		DescriptorSetLayoutBinding colorDescriptorSetLayoutBinding{};
		colorDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		colorDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		colorDescriptorSetLayoutBinding.buffers = {colorStorageBuffer};

		DescriptorSetLayout cameraAndColorDescriptorSetLayout;
		cameraAndColorDescriptorSetLayout.bindings.emplace_back(cameraDescriptorSetLayoutBinding); // Camera at set 0, binding 0.
		cameraAndColorDescriptorSetLayout.bindings.emplace_back(colorDescriptorSetLayoutBinding); // Color buffer at set 0, binding 1.

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts.emplace_back(cameraAndColorDescriptorSetLayout);

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

		const chain<uint8> vertexShaderModuleCode = FileLoader::loadByteBuffer(vertexShaderModulePath);
		const chain<uint8> fragmentShaderModuleCode = FileLoader::loadByteBuffer(fragmentShaderModulePath);

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
		renderPipelineDescriptor.vertexBufferLayout.add<uint32>(1, false);
		renderPipelineDescriptor.vertexBufferLayout.add<uint32>(1, false);
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);

		// Create position vertex buffer.
		{
			const std::vector<vec3<float32>> vertexPositions = {
				{0.0f, 0.0f, 1.0f},
				{1.0f, 0.0f, 1.0f},
				{1.0f, 1.0f, 1.0f},
				{0.0f, 1.0f, 1.0f},
			};

			BufferDescriptor bufferDescriptor{};
			bufferDescriptor.size = sizeof(vec3<float32>) * vertexPositions.size();
			bufferDescriptor.usage = BufferUsage::VERTEX;

			positionVertexBuffer = createBuffer(bufferDescriptor);

			BufferMappedRange mappedRange = positionVertexBuffer->map(0, bufferDescriptor.size);

			std::copy(vertexPositions.begin(), vertexPositions.end(), static_cast<vec3<float32>*>(mappedRange));

			positionVertexBuffer->unmap();
		}

		BufferDescriptor transformVertexBufferDescriptor{};
		transformVertexBufferDescriptor.size = sizeof(GUIComponent) * MAX_QUADS;
		transformVertexBufferDescriptor.usage = BufferUsage::VERTEX;

		transformVertexBuffer = createBuffer(transformVertexBufferDescriptor);

		// Create index buffer.
		{
			const std::vector<uint16> indices = {
				0, 1, 2,
				0, 2, 3,
			};

			BufferDescriptor bufferDescriptor{};
			bufferDescriptor.size = sizeof(uint16) * indices.size();
			bufferDescriptor.usage = BufferUsage::INDEX;

			indexBuffer = createBuffer(bufferDescriptor);

			BufferMappedRange mappedRange = indexBuffer->map(0, bufferDescriptor.size);

			std::copy(indices.begin(), indices.end(), static_cast<uint16*>(mappedRange));

			indexBuffer->unmap();
		}

		// Create command encoder.
		{
			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

			commandEncoder = createCommandEncoder(commandBufferDescriptor);
		}
	}

	void GUIRenderer::registerEntities(const ECS& ecs) {
		const ComponentPool<GUIComponent>& componentPool = ecs.getComponentPool<GUIComponent>();

		if (componentPool.componentCount >= MAX_QUADS) {
			throw std::runtime_error("Too many alive components.");
		}

		instanceCount = componentPool.componentCount;

		BufferMappedRange mappedRange = transformVertexBuffer->map(0, sizeof(GUIComponent) * instanceCount);

		std::copy(componentPool.components.begin(), componentPool.components.begin() + instanceCount, static_cast<GUIComponent*>(mappedRange));

		transformVertexBuffer->unmap();
	}

	void GUIRenderer::registerColors(const std::vector<vec4<float32>>& colors) const {
		BufferMappedRange mappedRange = colorStorageBuffer->map(0, sizeof(vec4<float32>) * colors.size());

		std::copy(colors.begin(), colors.end(), static_cast<vec4<float32>*>(mappedRange));

		colorStorageBuffer->unmap();
	}

	void GUIRenderer::render() {
		updateCamera();

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
		commandEncoder->bindVertexBuffer(1, transformVertexBuffer, sizeof(GUIComponent));
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

	void GUIRenderer::updateCamera() const {
		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		UniformBufferObject cameraUniformBufferObject;
		cameraUniformBufferObject.projection = {
			2.0f / width, 0.0f, 0.0f,
			0.0f, -2.0f / height, 0.0f,
			-1.0f, 1.0f, 1.0f,
		};

		std::copy(&cameraUniformBufferObject, &cameraUniformBufferObject + 1, static_cast<UniformBufferObject*>(cameraUniformBufferMappedRanges[frameIndex]));
	}

	void GUIRenderer::onEvent(Event& event) {}
}