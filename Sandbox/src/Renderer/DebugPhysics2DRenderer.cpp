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

		cameraMappedRange->zoomLevel = 90.0f;
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

		const ref<ShaderModule> fillVertexShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2DFill.vert.spv", vertexShaderModuleDescriptor);
		const ref<ShaderModule> lineVertexShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2DLine.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fillFragmentShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2DFill.frag.spv", fragmentShaderModuleDescriptor);
		const ref<ShaderModule> lineFragmentShaderModule = loadShaderModule("Asset/Shader/DebugPhysics2DLine.frag.spv", fragmentShaderModuleDescriptor);

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

		RenderPipelineDescriptor lineRenderPipelineDescriptor;
		lineRenderPipelineDescriptor.renderPass = getMainRenderPass();
		lineRenderPipelineDescriptor.pipelineLayout = renderPipelineLayout;
		lineRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
		lineRenderPipelineDescriptor.vertexShaderModule = lineVertexShaderModule;
		lineRenderPipelineDescriptor.fragmentShaderModule = lineFragmentShaderModule;
		lineRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_STRIP;
		lineRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		lineRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		lineRenderPipelineDescriptor.cullMode = CullMode::BACK;
		lineRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		lineRenderPipeline = createRenderPipeline(lineRenderPipelineDescriptor);
	}

	void DebugPhysics2DRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		updateCamera(width, height);

		uint32 indexOffset = 0;
		uint32 vertexOffset = 0;
		uint32 instanceOffset = 0;

		commandEncoder->bindRenderPipeline(fillRenderPipeline);
		commandEncoder->bindAllDescriptorSets();

		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());

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

		commandEncoder->bindRenderPipeline(lineRenderPipeline);
		commandEncoder->bindAllDescriptorSets();

		for (const Object2D& object : scene) {
			const Geometry2D& geometry = scene.getGeometry(object.geometryIndex);
			const uint32 vertexCount = static_cast<uint32>(geometry.getVertices().size()) + 1;

			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->draw(vertexCount, 1, 0);

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