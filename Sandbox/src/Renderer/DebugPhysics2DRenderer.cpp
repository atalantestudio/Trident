#include "pch.hpp"

#include "DebugPhysics2DRenderer.hpp"

#include "Engine/API/Material/ColorMaterial.hpp"

namespace ProjectA {
	DebugPhysics2DRenderer::DebugPhysics2DRenderer() {
		initializeCameraUniformBuffer();
		initializeViewUniformBuffer();

		initializeGridVertexBuffer();
		initializeGridRenderPipeline();

		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		resize(width, height);
	}

	void DebugPhysics2DRenderer::initializeCameraUniformBuffer() {
		constexpr uint64 cameraBufferSize = sizeof(CameraUniform);

		BufferDescriptor cameraBufferDescriptor;
		cameraBufferDescriptor.size = cameraBufferSize;
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.persistent = true;

		cameraUniformBuffer = createBuffer(cameraBufferDescriptor);
		cameraMappedRange = cameraUniformBuffer->map<CameraUniform>(0, cameraBufferSize);

		cameraMappedRange->zoomLevel = DEFAULT_ZOOM_LEVEL;
	}

	void DebugPhysics2DRenderer::initializeViewUniformBuffer() {
		constexpr uint64 viewportUniformBufferSize = sizeof(ViewUniform);

		BufferDescriptor viewportUniformBufferDescriptor;
		viewportUniformBufferDescriptor.size = viewportUniformBufferSize;
		viewportUniformBufferDescriptor.usage = BufferUsage::UNIFORM;
		viewportUniformBufferDescriptor.persistent = true;

		viewUniformBuffer = createBuffer(viewportUniformBufferDescriptor);
		viewMappedRange = viewUniformBuffer->map<ViewUniform>(0, viewportUniformBufferSize);
	}

	void DebugPhysics2DRenderer::initializeGridVertexBuffer() {
		constexpr uint64 gridVertexBufferSize = 3 * sizeof(svec2<float32>);

		const ref<Buffer> gridVertexStagingBuffer = createStagingBuffer(gridVertexBufferSize);

		svec2<float32>* gridVertexMappedRange = gridVertexStagingBuffer->map<svec2<float32>>(0, gridVertexBufferSize);

		gridVertexMappedRange[0] = {-1.0f,  1.0f};
		gridVertexMappedRange[1] = { 3.0f,  1.0f};
		gridVertexMappedRange[2] = {-1.0f, -3.0f};

		gridVertexStagingBuffer->unmap();

		BufferDescriptor gridVertexBufferDescriptor{};
		gridVertexBufferDescriptor.size = gridVertexBufferSize;
		gridVertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

		gridVertexBuffer = createBuffer(gridVertexBufferDescriptor);

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(gridVertexStagingBuffer, gridVertexBuffer, 0, 0, gridVertexBufferSize);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void DebugPhysics2DRenderer::initializeGridRenderPipeline() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

		DescriptorSetLayoutBinding viewDescriptorSetLayoutBinding;
		viewDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		viewDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		viewDescriptorSetLayoutBinding.buffers = {viewUniformBuffer};

		DescriptorSetLayout gridPipelineDescriptorSetLayout;
		gridPipelineDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, viewDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor gridPipelineLayoutDescriptor;
		gridPipelineLayoutDescriptor.layouts = {gridPipelineDescriptorSetLayout};

		gridRenderPipelineLayout = createPipelineLayout(gridPipelineLayoutDescriptor);

		ShaderModuleDescriptor gridVertexShaderModuleDescriptor{};
		gridVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		gridVertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> gridVertexShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/Grid.vert.spv", gridVertexShaderModuleDescriptor);

		ShaderModuleDescriptor gridFragmentShaderModuleDescriptor{};
		gridFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		gridFragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> gridFragmentShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/Grid.frag.spv", gridFragmentShaderModuleDescriptor);

		VertexInputBinding gridVertexInputBinding(InputRate::VERTEX);
		gridVertexInputBinding.registerAttribute<svec2<float32>>(false);

		RenderPipelineDescriptor gridRenderPipelineDescriptor;
		gridRenderPipelineDescriptor.renderPass = getMainRenderPass();
		gridRenderPipelineDescriptor.pipelineLayout = gridRenderPipelineLayout;
		gridRenderPipelineDescriptor.vertexInputBindings = {gridVertexInputBinding};
		gridRenderPipelineDescriptor.vertexShaderModule = gridVertexShaderModule;
		gridRenderPipelineDescriptor.fragmentShaderModule = gridFragmentShaderModule;
		gridRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		gridRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		gridRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		gridRenderPipelineDescriptor.cullMode = CullMode::BACK;
		gridRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		gridRenderPipeline = createRenderPipeline(gridRenderPipelineDescriptor);
	}

	void DebugPhysics2DRenderer::registerScene(const Scene2D& scene) {
		this->scene = &scene;

		uint32 vertexCount = 0;
		uint32 instanceCount = 0;

		for (const Object2D& object : scene) {
			const Geometry2D& geometry = scene.getGeometry(object.geometryIndex);

			vertexCount += static_cast<uint32>(geometry.getVertices().size()) + 1;
			indexCount += static_cast<uint32>(geometry.getIndices().size());
			instanceCount += 1;
		}

		const uint64 vertexBufferSize = vertexCount * sizeof(svec2<float32>);
		const uint64 indexBufferSize = indexCount * sizeof(uint16);
		const uint64 instanceBufferSize = instanceCount * sizeof(Instance);

		const uint64 stagingBufferSize = vertexBufferSize + indexBufferSize + instanceBufferSize;

		const ref<Device> device = getDevice();
		const ref<Buffer> stagingBuffer = createStagingBuffer(stagingBufferSize);

		void* mappedRange = stagingBuffer->map<void>(0, stagingBufferSize);
		svec2<float32>* vertexMappedRange = static_cast<svec2<float32>*>(mappedRange);
		uint16* indexMappedRange = static_cast<uint16*>(static_cast<void*>(vertexMappedRange + vertexCount));
		Instance* instanceMappedRange = static_cast<Instance*>(static_cast<void*>(indexMappedRange + indexCount));

		for (const Object2D& object : scene) {
			const Geometry2D& geometry = scene.getGeometry(object.geometryIndex);
			const ColorMaterial& material = *std::static_pointer_cast<ColorMaterial>(scene.getMaterial(object.materialIndex)).get();
			const chain<svec2<float32>>& vertices = geometry.getVertices();
			const chain<uint16>& indices = geometry.getIndices();

			std::copy(vertices.begin(), vertices.end(), vertexMappedRange);

			// Duplicate the first vertex for line rendering.
			vertexMappedRange += vertices.size();
			*vertexMappedRange = vertices[0];

			std::copy(indices.begin(), indices.end(), indexMappedRange);

			instanceMappedRange->fillColor = material.fillColor;

			vertexMappedRange += 1;
			indexMappedRange += indices.size();
			instanceMappedRange += 1;
		}

		stagingBuffer->unmap();

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = vertexBufferSize;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

		vertexBuffer = createBuffer(vertexBufferDescriptor);

		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = indexBufferSize;
		indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;

		indexBuffer = createBuffer(indexBufferDescriptor);

		BufferDescriptor instanceBufferDescriptor{};
		instanceBufferDescriptor.size = instanceBufferSize;
		instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		instanceBufferDescriptor.persistent = true;

		instanceBuffer = createBuffer(instanceBufferDescriptor);
		this->instanceMappedRange = instanceBuffer->map<Instance>(0, instanceBufferSize);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, 0, 0, vertexBufferSize);
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, vertexBufferSize, 0, indexBufferSize);
		commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, vertexBufferSize + indexBufferSize, 0, instanceBufferSize);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();

		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

		renderPipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fillVertexShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/DebugPhysics2DFill.vert.spv", vertexShaderModuleDescriptor);
		const ref<ShaderModule> strokeVertexShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/DebugPhysics2DStroke.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fillFragmentShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/DebugPhysics2DFill.frag.spv", fragmentShaderModuleDescriptor);
		const ref<ShaderModule> strokeFragmentShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/DebugPhysics2DStroke.frag.spv", fragmentShaderModuleDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>(false);

		VertexInputBinding fillInstanceInputBinding(InputRate::INSTANCE);
		fillInstanceInputBinding.registerAttribute<mat3x3<float32>>(false);
		fillInstanceInputBinding.registerAttribute<vec4<float32>>(false);

		//VertexInputBinding strokeInstanceInputBinding(InputRate::INSTANCE);
		//strokeInstanceInputBinding.registerAttribute<mat3x3<float32>>(false);

		RenderPipelineDescriptor fillRenderPipelineDescriptor;
		fillRenderPipelineDescriptor.renderPass = getMainRenderPass();
		fillRenderPipelineDescriptor.pipelineLayout = renderPipelineLayout;
		fillRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, fillInstanceInputBinding};
		fillRenderPipelineDescriptor.vertexShaderModule = fillVertexShaderModule;
		fillRenderPipelineDescriptor.fragmentShaderModule = fillFragmentShaderModule;
		fillRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		fillRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		fillRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		fillRenderPipelineDescriptor.cullMode = CullMode::BACK;
		fillRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		fillRenderPipeline = createRenderPipeline(fillRenderPipelineDescriptor);

		RenderPipelineDescriptor strokeRenderPipelineDescriptor;
		strokeRenderPipelineDescriptor.renderPass = getMainRenderPass();
		strokeRenderPipelineDescriptor.pipelineLayout = renderPipelineLayout;
		strokeRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, fillInstanceInputBinding};
		strokeRenderPipelineDescriptor.vertexShaderModule = strokeVertexShaderModule;
		strokeRenderPipelineDescriptor.fragmentShaderModule = strokeFragmentShaderModule;
		strokeRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_STRIP;
		strokeRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		strokeRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		strokeRenderPipelineDescriptor.cullMode = CullMode::BACK;
		strokeRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		strokeRenderPipeline = createRenderPipeline(strokeRenderPipelineDescriptor);
	}

	void DebugPhysics2DRenderer::zoom(float32 zoomLevelDelta) const {
		float32 zoomLevel = cameraMappedRange->zoomLevel + zoomLevelDelta / 120.0f * ZOOM_SENSITIVITY;

		// TODO: lyah::max
		if (zoomLevel < MIN_ZOOM_LEVEL) {
			zoomLevel = MIN_ZOOM_LEVEL;
		}

		// TODO: lyah::min
		if (zoomLevel > MAX_ZOOM_LEVEL) {
			zoomLevel = MAX_ZOOM_LEVEL;
		}

		cameraMappedRange->zoomLevel = zoomLevel;
	}

	void DebugPhysics2DRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());

		renderGrid(commandEncoder);
		renderScene(commandEncoder);
	}

	void DebugPhysics2DRenderer::renderGrid(const ref<CommandEncoder>& commandEncoder) const {
		commandEncoder->bindRenderPipeline(gridRenderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindVertexBuffer(0, gridVertexBuffer, 0, sizeof(svec2<float32>));
		commandEncoder->draw(3, 1, 0);
	}

	void DebugPhysics2DRenderer::renderScene(const ref<CommandEncoder>& commandEncoder) const {
		uint32 indexOffset = 0;
		uint32 vertexOffset = 0;

		for (uint32 instanceIndex = 0; instanceIndex < scene->getObjectCount(); instanceIndex += 1) {
			const Object2D& object = (*scene)[instanceIndex];
			const Geometry2D& geometry = scene->getGeometry(object.geometryIndex);
			const uint32 indexCount = static_cast<uint32>(geometry.getIndices().size());
			const uint32 vertexCount = static_cast<uint32>(geometry.getVertices().size()) + 1;

			commandEncoder->bindRenderPipeline(fillRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, indexOffset * sizeof(uint16));
			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance), sizeof(Instance));
			commandEncoder->drawIndexed(indexCount, 1, 0);

			commandEncoder->bindRenderPipeline(strokeRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance), sizeof(Instance));
			commandEncoder->draw(vertexCount, 1, vertexOffset);

			indexOffset += indexCount;
			vertexOffset += vertexCount;
		}
	}

	void DebugPhysics2DRenderer::resize(float32 width, float32 height) const {
		const float32 left = -width / 2.0f;
		const float32 right = width / 2.0f;
		const float32 top = height / 2.0f;
		const float32 bottom = -height / 2.0f;

		cameraMappedRange->projection = {
			2.0f / (right - left),  0.0f,                  -(right + left) / (right - left),
			0.0f,                   2.0f / (top - bottom), -(top + bottom) / (top - bottom),
			0.0f,                   0.0f,                   1.0f,
		};

		viewMappedRange->viewport.x = width;
		viewMappedRange->viewport.y = height;
	}

	void DebugPhysics2DRenderer::writeInstanceTransform(uint32 instanceIndex, const mat3x3<float32>& transform) const {
		instanceMappedRange[instanceIndex].world = transform;
	}
}