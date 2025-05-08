#include "pch.hpp"

#include "DebugPhysics2DRenderer.hpp"

#include "Engine/API/Material/ColorMaterial.hpp"

namespace ProjectA {
	DebugPhysics2DRenderer::DebugPhysics2DRenderer() {
		constexpr uint64 cameraBufferSize = sizeof(CameraUniform);

		BufferDescriptor cameraBufferDescriptor;
		cameraBufferDescriptor.size = cameraBufferSize;
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.persistent = true;

		cameraBuffer = createBuffer(cameraBufferDescriptor);
		cameraMappedRange = cameraBuffer->map<CameraUniform>(0, cameraBufferSize);

		cameraMappedRange->zoomLevel = 50.0f;

		initializeLineVertexBuffer();
		initializeLineColorUniformBuffer();
		initializeLineRenderPipeline();
	}

	void DebugPhysics2DRenderer::initializeLineVertexBuffer() {
		const ref<Swapchain> swapchain = getSwapchain();

		constexpr uint32 gap = 48;
		const uint32 columnCount = swapchain->getExtentWidth() / gap;
		const uint32 rowCount = swapchain->getExtentHeight() / gap;
		gridVertexCount = (columnCount + rowCount) * 2;
		const uint64 lineVertexBufferSize = static_cast<uint64>(4 + gridVertexCount) * sizeof(svec2<float32>);

		const ref<Buffer> lineVertexStagingBuffer = createStagingBuffer(lineVertexBufferSize);

		svec2<float32>* lineVertexMappedRange = lineVertexStagingBuffer->map<svec2<float32>>(0, lineVertexBufferSize);

		lineVertexMappedRange[0] = {-1.0f,  0.0f};
		lineVertexMappedRange[1] = { 1.0f,  0.0f};
		lineVertexMappedRange[2] = { 0.0f,  1.0f};
		lineVertexMappedRange[3] = { 0.0f, -1.0f};
		lineVertexMappedRange += 4;

		const float32 columnGap = gap / static_cast<float32>(swapchain->getExtentHeight()) * 2.0f;
		float32 x = columnGap;

		for (uint32 columnIndex = 0; columnIndex < columnCount; columnIndex += 2) {
			*lineVertexMappedRange++ = {-x,  1.0f};
			*lineVertexMappedRange++ = {-x, -1.0f};
			*lineVertexMappedRange++ = { x,  1.0f};
			*lineVertexMappedRange++ = { x, -1.0f};

			x += columnGap;
		}

		const float32 rowGap = gap / static_cast<float32>(swapchain->getExtentWidth()) * 2.0f;
		float32 y = rowGap;

		for (uint32 rowIndex = 0; rowIndex < rowCount; rowIndex += 2) {
			*lineVertexMappedRange++ = {-1.0f,  y};
			*lineVertexMappedRange++ = { 1.0f,  y};
			*lineVertexMappedRange++ = {-1.0f, -y};
			*lineVertexMappedRange++ = { 1.0f, -y};

			y += rowGap;
		}

		lineVertexStagingBuffer->unmap();

		BufferDescriptor lineVertexBufferDescriptor;
		lineVertexBufferDescriptor.size = lineVertexBufferSize;
		lineVertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

		lineVertexBuffer = createBuffer(lineVertexBufferDescriptor);

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(lineVertexStagingBuffer, lineVertexBuffer, 0, 0, lineVertexBufferSize);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void DebugPhysics2DRenderer::initializeLineColorUniformBuffer() {
		constexpr uint64 lineColorBufferSize = sizeof(vec4<float32>);

		BufferDescriptor lineColorUniformBufferDescriptor;
		lineColorUniformBufferDescriptor.size = lineColorBufferSize;
		lineColorUniformBufferDescriptor.usage = BufferUsage::UNIFORM;
		lineColorUniformBufferDescriptor.persistent = true;

		lineColorUniformBuffer = createBuffer(lineColorUniformBufferDescriptor);
		lineColorMappedRange = lineColorUniformBuffer->map<vec4<float32>>(0, lineColorBufferSize);
	}

	void DebugPhysics2DRenderer::initializeLineRenderPipeline() {
		DescriptorSetLayoutBinding lineColorDescriptorSetLayoutBinding;
		lineColorDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		lineColorDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		lineColorDescriptorSetLayoutBinding.buffers = {lineColorUniformBuffer};

		DescriptorSetLayout lineColorDescriptorSetLayout;
		lineColorDescriptorSetLayout.bindings = {lineColorDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor linePipelineLayoutDescriptor;
		linePipelineLayoutDescriptor.layouts = {lineColorDescriptorSetLayout};

		lineRenderPipelineLayout = createPipelineLayout(linePipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/DebugPhysics2DLine.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2D/DebugPhysics2DLine.frag.spv", fragmentShaderModuleDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>(false);

		RenderPipelineDescriptor lineRenderPipelineDescriptor;
		lineRenderPipelineDescriptor.renderPass = getMainRenderPass();
		lineRenderPipelineDescriptor.pipelineLayout = lineRenderPipelineLayout;
		lineRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
		lineRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		lineRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		lineRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_LIST;
		lineRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		lineRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		lineRenderPipelineDescriptor.cullMode = CullMode::BACK;
		lineRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		lineRenderPipeline = createRenderPipeline(lineRenderPipelineDescriptor);
	}

	void DebugPhysics2DRenderer::registerScene(const Scene2D& scene) {
		this->scene = scene;

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

		instanceBuffer = createBuffer(instanceBufferDescriptor);

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
		cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

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

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<vec4<float32>>(false);

		RenderPipelineDescriptor fillRenderPipelineDescriptor;
		fillRenderPipelineDescriptor.renderPass = getMainRenderPass();
		fillRenderPipelineDescriptor.pipelineLayout = renderPipelineLayout;
		fillRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
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
		strokeRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
		strokeRenderPipelineDescriptor.vertexShaderModule = strokeVertexShaderModule;
		strokeRenderPipelineDescriptor.fragmentShaderModule = strokeFragmentShaderModule;
		strokeRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_STRIP;
		strokeRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		strokeRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		strokeRenderPipelineDescriptor.cullMode = CullMode::BACK;
		strokeRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		strokeRenderPipeline = createRenderPipeline(strokeRenderPipelineDescriptor);
	}

	void DebugPhysics2DRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		updateCamera(width, height);

		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());

		renderGrid(commandEncoder);
		renderScene(commandEncoder);
	}

	void DebugPhysics2DRenderer::renderGrid(const ref<CommandEncoder>& commandEncoder) const {
		commandEncoder->bindRenderPipeline(lineRenderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindVertexBuffer(0, lineVertexBuffer, 0, sizeof(svec2<float32>));

		//*lineColorMappedRange = {0.867f, 0.867f, 0.867f, 1.0f};

		//commandEncoder->draw(gridVertexCount, 1, 4);

		*lineColorMappedRange = {0.733f, 0.733f, 0.733f, 1.0f};

		commandEncoder->draw(4, 1, 0);
	}

	void DebugPhysics2DRenderer::renderScene(const ref<CommandEncoder>& commandEncoder) const {
		uint32 indexOffset = 0;
		uint32 vertexOffset = 0;
		uint32 instanceOffset = 0;

		commandEncoder->bindRenderPipeline(fillRenderPipeline);
		commandEncoder->bindAllDescriptorSets();

		for (const Object2D& object : scene) {
			const Geometry2D& geometry = scene.getGeometry(object.geometryIndex);
			const uint32 indexCount = static_cast<uint32>(geometry.getIndices().size());
			const uint32 vertexCount = static_cast<uint32>(geometry.getVertices().size()) + 1;

			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, indexOffset * sizeof(uint16));
			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceOffset * sizeof(Instance), sizeof(Instance));
			commandEncoder->drawIndexed(indexCount, 1, 0);

			indexOffset += indexCount;
			vertexOffset += vertexCount;
			instanceOffset += 1;
		}

		vertexOffset = 0;

		commandEncoder->bindRenderPipeline(strokeRenderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(svec2<float32>));

		for (const Object2D& object : scene) {
			const Geometry2D& geometry = scene.getGeometry(object.geometryIndex);
			const uint32 vertexCount = static_cast<uint32>(geometry.getVertices().size()) + 1;

			commandEncoder->draw(vertexCount, 1, vertexOffset);

			vertexOffset += vertexCount;
		}
	}

	void DebugPhysics2DRenderer::updateCamera(float32 width, float32 height) const {
		const float32 left = -width / 2.0f;
		const float32 right = width / 2.0f;
		const float32 top = height / 2.0f;
		const float32 bottom = -height / 2.0f;

		cameraMappedRange->projection = {
			2.0f / (right - left),  0.0f,                  -(right + left) / (right - left),
			0.0f,                   2.0f / (top - bottom), -(top + bottom) / (top - bottom),
			0.0f,                   0.0f,                   1.0f,
		};
	}
}