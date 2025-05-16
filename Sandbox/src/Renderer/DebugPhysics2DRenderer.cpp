#include "pch.hpp"

#include "DebugPhysics2DRenderer.hpp"

#include "Engine/API/Material/ColorMaterial.hpp"

namespace ProjectA {
	DebugPhysics2DRenderer::DebugPhysics2DRenderer() {
		initializeCameraUniformBuffer();
		initializeViewUniformBuffer();

		initializeGridVertexBuffer();
		initializeGridRenderPipeline();

		initializeVertexFeatureBuffer();
		initializeEdgeVertexFeatureBuffer();
		initializeFeaturePipelineLayout();
		initializeVertexFeatureRenderPipeline();
		initializeEdgeFeatureRenderPipeline();

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
		cameraDescriptorSetLayoutBinding.bufferOffset = sizeof(mat3x3<float32>);

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
		gridRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		gridRenderPipeline = createRenderPipeline(gridRenderPipelineDescriptor);
	}

	void DebugPhysics2DRenderer::initializeCenterOfMassBuffer() {
		const uint64 centerOfMassBufferSize = scene->getObjectCount() * sizeof(svec2<float32>);

		BufferDescriptor centerOfMassBufferDescriptor{};
		centerOfMassBufferDescriptor.size = centerOfMassBufferSize;
		centerOfMassBufferDescriptor.usage = BufferUsage::VERTEX;
		centerOfMassBufferDescriptor.persistent = true;

		centerOfMassBuffer = createBuffer(centerOfMassBufferDescriptor);
		centerOfMassMappedRange = centerOfMassBuffer->map<svec2<float32>>(0, centerOfMassBufferSize);
	}

	void DebugPhysics2DRenderer::initializeCenterOfMassRenderPipeline() {
		ShaderModuleDescriptor centerOfMassVertexShaderModuleDescriptor{};
		centerOfMassVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		centerOfMassVertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> centerOfMassVertexShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/CenterOfMass.vert.spv", centerOfMassVertexShaderModuleDescriptor);

		ShaderModuleDescriptor centerOfMassFragmentShaderModuleDescriptor{};
		centerOfMassFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		centerOfMassFragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> centerOfMassFragmentShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/CenterOfMass.frag.spv", centerOfMassFragmentShaderModuleDescriptor);

		VertexInputBinding centerOfMassVertexInputBinding(InputRate::VERTEX);
		centerOfMassVertexInputBinding.registerAttribute<svec2<float32>>(false);

		RenderPipelineDescriptor centerOfMassRenderPipelineDescriptor;
		centerOfMassRenderPipelineDescriptor.renderPass = getMainRenderPass();
		centerOfMassRenderPipelineDescriptor.pipelineLayout = featurePipelineLayout;
		centerOfMassRenderPipelineDescriptor.vertexInputBindings = {centerOfMassVertexInputBinding};
		centerOfMassRenderPipelineDescriptor.vertexShaderModule = centerOfMassVertexShaderModule;
		centerOfMassRenderPipelineDescriptor.fragmentShaderModule = centerOfMassFragmentShaderModule;
		centerOfMassRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
		centerOfMassRenderPipelineDescriptor.polygonMode = PolygonMode::POINT;
		centerOfMassRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		centerOfMassRenderPipeline = createRenderPipeline(centerOfMassRenderPipelineDescriptor);
	}

	void DebugPhysics2DRenderer::initializeVertexFeatureBuffer() {
		constexpr uint64 vertexFeatureBufferSize = 2 * sizeof(svec2<float32>);

		BufferDescriptor vertexFeatureBufferDescriptor{};
		vertexFeatureBufferDescriptor.size = vertexFeatureBufferSize;
		vertexFeatureBufferDescriptor.usage = BufferUsage::VERTEX;
		vertexFeatureBufferDescriptor.persistent = true;

		vertexFeatureBuffer = createBuffer(vertexFeatureBufferDescriptor);
		vertexFeatureMappedRange = vertexFeatureBuffer->map<svec2<float32>>(0, vertexFeatureBufferSize);
	}

	void DebugPhysics2DRenderer::initializeEdgeVertexFeatureBuffer() {
		constexpr uint64 edgeVertexFeatureBufferSize = 4 * sizeof(svec2<float32>);

		BufferDescriptor edgeVertexFeatureBufferDescriptor{};
		edgeVertexFeatureBufferDescriptor.size = edgeVertexFeatureBufferSize;
		edgeVertexFeatureBufferDescriptor.usage = BufferUsage::VERTEX;
		edgeVertexFeatureBufferDescriptor.persistent = true;

		edgeVertexFeatureBuffer = createBuffer(edgeVertexFeatureBufferDescriptor);
		edgeVertexFeatureMappedRange = edgeVertexFeatureBuffer->map<svec2<float32>>(0, edgeVertexFeatureBufferSize);
	}

	void DebugPhysics2DRenderer::initializeFeaturePipelineLayout() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

		DescriptorSetLayout featureDescriptorSetLayout;
		featureDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor featurePipelineLayoutDescriptor;
		featurePipelineLayoutDescriptor.layouts = {featureDescriptorSetLayout};

		featurePipelineLayout = createPipelineLayout(featurePipelineLayoutDescriptor);
	}

	void DebugPhysics2DRenderer::initializeVertexFeatureRenderPipeline() {
		ShaderModuleDescriptor vertexFeatureVertexShaderModuleDescriptor{};
		vertexFeatureVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexFeatureVertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexFeatureVertexShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/VertexFeature.vert.spv", vertexFeatureVertexShaderModuleDescriptor);

		ShaderModuleDescriptor vertexFeatureFragmentShaderModuleDescriptor{};
		vertexFeatureFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		vertexFeatureFragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexFeatureFragmentShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/VertexFeature.frag.spv", vertexFeatureFragmentShaderModuleDescriptor);

		VertexInputBinding vertexFeatureVertexInputBinding(InputRate::VERTEX);
		vertexFeatureVertexInputBinding.registerAttribute<svec2<float32>>(false);

		RenderPipelineDescriptor vertexFeatureRenderPipelineDescriptor;
		vertexFeatureRenderPipelineDescriptor.renderPass = getMainRenderPass();
		vertexFeatureRenderPipelineDescriptor.pipelineLayout = featurePipelineLayout;
		vertexFeatureRenderPipelineDescriptor.vertexInputBindings = {vertexFeatureVertexInputBinding};
		vertexFeatureRenderPipelineDescriptor.vertexShaderModule = vertexFeatureVertexShaderModule;
		vertexFeatureRenderPipelineDescriptor.fragmentShaderModule = vertexFeatureFragmentShaderModule;
		vertexFeatureRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
		vertexFeatureRenderPipelineDescriptor.polygonMode = PolygonMode::POINT;
		vertexFeatureRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		vertexFeatureRenderPipeline = createRenderPipeline(vertexFeatureRenderPipelineDescriptor);
	}

	void DebugPhysics2DRenderer::initializeEdgeFeatureRenderPipeline() {
		ShaderModuleDescriptor edgeFeatureVertexShaderModuleDescriptor{};
		edgeFeatureVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		edgeFeatureVertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> edgeFeatureVertexShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/EdgeFeature.vert.spv", edgeFeatureVertexShaderModuleDescriptor);

		ShaderModuleDescriptor edgeFeatureFragmentShaderModuleDescriptor{};
		edgeFeatureFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		edgeFeatureFragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> edgeFeatureFragmentShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/EdgeFeature.frag.spv", edgeFeatureFragmentShaderModuleDescriptor);

		VertexInputBinding edgeFeatureVertexInputBinding(InputRate::VERTEX);
		edgeFeatureVertexInputBinding.registerAttribute<svec2<float32>>(false);

		RenderPipelineDescriptor edgeFeatureRenderPipelineDescriptor;
		edgeFeatureRenderPipelineDescriptor.renderPass = getMainRenderPass();
		edgeFeatureRenderPipelineDescriptor.pipelineLayout = featurePipelineLayout;
		edgeFeatureRenderPipelineDescriptor.vertexInputBindings = {edgeFeatureVertexInputBinding};
		edgeFeatureRenderPipelineDescriptor.vertexShaderModule = edgeFeatureVertexShaderModule;
		edgeFeatureRenderPipelineDescriptor.fragmentShaderModule = edgeFeatureFragmentShaderModule;
		edgeFeatureRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_LIST;
		edgeFeatureRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		edgeFeatureRenderPipelineDescriptor.lineWidth = 3.0f;
		edgeFeatureRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		edgeFeatureRenderPipeline = createRenderPipeline(edgeFeatureRenderPipelineDescriptor);
	}

	void DebugPhysics2DRenderer::registerScene(const Scene2D& scene) {
		this->scene = &scene;

		uint32 vertexCount = 0;
		uint32 instanceCount = 0;

		for (const Object2D& object : scene) {
			const Geometry2D& geometry = scene.getGeometry(object.geometryIndex);

			vertexCount += static_cast<uint32>(geometry.vertices.size()) + 1;
			indexCount += static_cast<uint32>(geometry.indices.size());
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

			for (uint32 vertexIndex = 0; vertexIndex < geometry.vertices.size(); vertexIndex += 1) {
				const vec2<float32>& vertex = geometry.vertices[vertexIndex];

				vertexMappedRange[vertexIndex] = {vertex[0], vertex[1]};
			}

			// Duplicate the first vertex for line rendering.
			vertexMappedRange += geometry.vertices.size();
			*vertexMappedRange = {geometry.vertices[0][0], geometry.vertices[0][1]};

			std::copy(geometry.indices.begin(), geometry.indices.end(), indexMappedRange);

			instanceMappedRange->fillColor = material.fillColor;

			vertexMappedRange += 1;
			indexMappedRange += geometry.indices.size();
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
		strokeRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		strokeRenderPipeline = createRenderPipeline(strokeRenderPipelineDescriptor);

		initializeCenterOfMassBuffer();
		initializeCenterOfMassRenderPipeline();
	}

	const vec2<float32>& DebugPhysics2DRenderer::getNdcMousePosition() const {
		return mousePosition;
	}

	void DebugPhysics2DRenderer::setNdcMousePosition(float32 x, float32 y) {
		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 horizontalCellCount = swapchain->getExtentWidth() / cameraMappedRange->zoomLevel;
		const float32 verticalCellCount = swapchain->getExtentHeight() / cameraMappedRange->zoomLevel;

		x = (x / swapchain->getExtentWidth() - 0.5f) * horizontalCellCount;
		y = -(y / swapchain->getExtentHeight() - 0.5f) * verticalCellCount;

		mousePosition = {x, y};
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

	void DebugPhysics2DRenderer::updateCentersOfMass() const {
		for (uint32 instanceIndex = 0; instanceIndex < scene->getObjectCount(); instanceIndex += 1) {
			const Object2D& object = (*scene)[instanceIndex];

			centerOfMassMappedRange[instanceIndex] = {object.position[0], object.position[1]};
		}
	}

	void DebugPhysics2DRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		updateCentersOfMass();

		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());

		renderGrid(commandEncoder);
		renderScene(commandEncoder);
		renderClosestFeatures(commandEncoder);
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
			const uint32 indexCount = static_cast<uint32>(geometry.indices.size());
			const uint32 vertexCount = static_cast<uint32>(geometry.vertices.size()) + 1;

			commandEncoder->bindRenderPipeline(fillRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, indexOffset * sizeof(uint16));
			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance), sizeof(Instance));
			commandEncoder->drawIndexed(indexCount, 1, 0);

			commandEncoder->bindRenderPipeline(strokeRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance), sizeof(Instance));
			commandEncoder->draw(vertexCount, 1, 0);

			commandEncoder->bindRenderPipeline(centerOfMassRenderPipeline);
			commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, instanceIndex * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->draw(1, 1, 0);

			indexOffset += indexCount;
			vertexOffset += vertexCount;
		}
	}

	void DebugPhysics2DRenderer::renderClosestFeatures(const ref<CommandEncoder>& commandEncoder) const {
		if (nextVertexFeatureIndex > 0) {
			commandEncoder->bindRenderPipeline(vertexFeatureRenderPipeline);
			commandEncoder->bindVertexBuffer(0, vertexFeatureBuffer, 0, sizeof(svec2<float32>));
			commandEncoder->draw(nextVertexFeatureIndex, 1, 0);
		}

		if (nextEdgeVertexFeatureIndex > 0) {
			commandEncoder->bindRenderPipeline(edgeFeatureRenderPipeline);
			commandEncoder->bindVertexBuffer(0, edgeVertexFeatureBuffer, 0, sizeof(svec2<float32>));
			commandEncoder->draw(nextEdgeVertexFeatureIndex, 1, 0);
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

	void DebugPhysics2DRenderer::registerClosestFeature(const ClosestFeature& closestFeature) {
		switch (closestFeature.count) {
			case 1:
				vertexFeatureMappedRange[nextVertexFeatureIndex] = {closestFeature.vertices[0][0], closestFeature.vertices[0][1]};
				nextVertexFeatureIndex += 1;

				break;
			case 2:
				edgeVertexFeatureMappedRange[nextEdgeVertexFeatureIndex + 0] = {closestFeature.vertices[0][0], closestFeature.vertices[0][1]};
				edgeVertexFeatureMappedRange[nextEdgeVertexFeatureIndex + 1] = {closestFeature.vertices[1][0], closestFeature.vertices[1][1]};
				nextEdgeVertexFeatureIndex += 2;

				break;
			default:
				DEBUG_ASSERT(false);

				break;
		}
	}

	void DebugPhysics2DRenderer::clearClosestFeatures() {
		nextVertexFeatureIndex = 0;
		nextEdgeVertexFeatureIndex = 0;
	}
}