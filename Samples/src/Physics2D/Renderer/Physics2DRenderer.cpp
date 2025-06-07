#include "pch.hpp"

#include "Physics2DRenderer.hpp"

#include "Engine/API/Material/ColorMaterial.hpp"

namespace ProjectA {
	Physics2DRenderer::Physics2DRenderer() {
		DEBUG_STATIC_ASSERT(SAMPLE_COUNT > 1);

		const ref<Surface> surface = getSurface();

		ColorAttachment colorAttachment{};
		colorAttachment.format = surface->getFormat();
		colorAttachment.sampleCount = SAMPLE_COUNT;
		colorAttachment.loadOperation = LoadOperation::CLEAR;
		colorAttachment.storeOperation = StoreOperation::STORE;

		RenderPassDescriptor renderPassDescriptor;
		renderPassDescriptor.colorAttachments = {colorAttachment};
		renderPassDescriptor.depthStencilAttachment.format = ImageFormat::D32_SFLOAT;
		renderPassDescriptor.depthStencilAttachment.sampleCount = SAMPLE_COUNT;
		renderPassDescriptor.depthStencilAttachment.loadOperation = LoadOperation::CLEAR;
		renderPassDescriptor.resolve = true;
		renderPassDescriptor.depthStencil = true;
		renderPassDescriptor.presentAttachmentIndex = 0;

		renderPass = createRenderPass(renderPassDescriptor);

		updateColorAttachment();
		updateDepthStencilAttachment();

		const ref<Swapchain>& swapchain = getSwapchain();

		swapchain->getImages(renderPass);

		initializeCameraUniformBuffer();
		initializeViewUniformBuffer();

		initializeGridVertexBuffer();
		initializeGridRenderPipeline();

		initializeClosestPointBuffer();
		initializeDistanceRenderPipeline();

		initializeVertexFeatureBuffer();
		initializeEdgeVertexFeatureBuffer();
		initializeFeaturePipelineLayout();
		initializeVertexFeatureRenderPipeline();
		initializeEdgeFeatureRenderPipeline();

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());
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

	void Physics2DRenderer::updateColorAttachment() {
		const ref<Swapchain>& swapchain = getSwapchain();

		AttachmentImageDescriptor colorAttachmentImageDescriptor{};
		colorAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		colorAttachmentImageDescriptor.width = swapchain->getExtentWidth();
		colorAttachmentImageDescriptor.height = swapchain->getExtentHeight();
		colorAttachmentImageDescriptor.depth = 1;
		colorAttachmentImageDescriptor.mipLevelCount = 1;
		colorAttachmentImageDescriptor.arrayLayerCount = 1;

		colorImage = renderPass->createColorAttachmentImage(colorAttachmentImageDescriptor, 0);

		constexpr ImageViewDescriptor colorImageViewDescriptor;

		colorImageView = colorImage->createView(colorImageViewDescriptor);

		renderPass->setColorAttachmentImageView(colorImageView, 0);
	}

	void Physics2DRenderer::updateDepthStencilAttachment() {
		const ref<Swapchain>& swapchain = getSwapchain();

		AttachmentImageDescriptor depthStencilAttachmentImageDescriptor{};
		depthStencilAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		depthStencilAttachmentImageDescriptor.width = swapchain->getExtentWidth();
		depthStencilAttachmentImageDescriptor.height = swapchain->getExtentHeight();
		depthStencilAttachmentImageDescriptor.depth = 1;
		depthStencilAttachmentImageDescriptor.mipLevelCount = 1;
		depthStencilAttachmentImageDescriptor.arrayLayerCount = 1;

		depthStencilImage = renderPass->createDepthStencilAttachmentImage(depthStencilAttachmentImageDescriptor);

		constexpr ImageViewDescriptor depthStencilImageViewDescriptor;

		depthStencilImageView = depthStencilImage->createView(depthStencilImageViewDescriptor);

		renderPass->setDepthStencilAttachmentImageView(depthStencilImageView);
	}

	void Physics2DRenderer::initializeCameraUniformBuffer() {
		constexpr uint64 cameraBufferSize = sizeof(CameraUniform);

		BufferDescriptor cameraBufferDescriptor;
		cameraBufferDescriptor.size = cameraBufferSize;
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		cameraBufferDescriptor.persistent = true;

		cameraUniformBuffer = createBuffer(cameraBufferDescriptor);
		cameraMappedRange = cameraUniformBuffer->map<CameraUniform>(0, cameraBufferSize);

		cameraMappedRange->zoomLevel = DEFAULT_ZOOM_LEVEL;
	}

	void Physics2DRenderer::initializeViewUniformBuffer() {
		constexpr uint64 viewportUniformBufferSize = sizeof(ViewUniform);

		BufferDescriptor viewportUniformBufferDescriptor;
		viewportUniformBufferDescriptor.size = viewportUniformBufferSize;
		viewportUniformBufferDescriptor.usage = BufferUsage::UNIFORM;
		viewportUniformBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		viewportUniformBufferDescriptor.persistent = true;

		viewUniformBuffer = createBuffer(viewportUniformBufferDescriptor);
		viewMappedRange = viewUniformBuffer->map<ViewUniform>(0, viewportUniformBufferSize);
	}

	void Physics2DRenderer::initializeGridVertexBuffer() {
		constexpr uint64 gridVertexBufferSize = 3 * sizeof(svec2<float32>);

		const ref<Buffer> gridVertexStagingBuffer = createStagingBuffer(gridVertexBufferSize);

		svec2<float32>* gridVertexMappedRange = gridVertexStagingBuffer->map<svec2<float32>>(0, gridVertexBufferSize);

		gridVertexMappedRange[0] = {-1.0f,  1.0f};
		gridVertexMappedRange[1] = { 3.0f,  1.0f};
		gridVertexMappedRange[2] = {-1.0f, -3.0f};

		gridVertexStagingBuffer->unmap();

		BufferDescriptor gridVertexBufferDescriptor;
		gridVertexBufferDescriptor.size = gridVertexBufferSize;
		gridVertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		gridVertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		gridVertexBuffer = createBuffer(gridVertexBufferDescriptor);

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(gridVertexStagingBuffer, gridVertexBuffer, 0, 0, gridVertexBufferSize);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void Physics2DRenderer::initializeGridRenderPipeline() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

		// TODO: The offset must be a multiple of the minUniformBufferAlignment physical device limit.
		cameraDescriptorSetLayoutBinding.bufferOffset = 0;
		//cameraDescriptorSetLayoutBinding.bufferOffset = sizeof(mat3x3<float32>);

		DescriptorSetLayoutBinding viewDescriptorSetLayoutBinding;
		viewDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		viewDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		viewDescriptorSetLayoutBinding.buffers = {viewUniformBuffer};

		DescriptorSetLayout gridPipelineDescriptorSetLayout;
		gridPipelineDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, viewDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor gridPipelineLayoutDescriptor;
		gridPipelineLayoutDescriptor.layouts = {gridPipelineDescriptorSetLayout};

		gridPipelineLayout = createPipelineLayout(gridPipelineLayoutDescriptor);

		VertexInputBinding gridVertexInputBinding(InputRate::VERTEX);
		gridVertexInputBinding.registerAttribute<svec2<float32>>();

		ShaderModuleDescriptor gridVertexShaderModuleDescriptor{};
		gridVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		gridVertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor gridFragmentShaderModuleDescriptor{};
		gridFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		gridFragmentShaderModuleDescriptor.entryPoint = "main";

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor gridRenderPipelineDescriptor;
		gridRenderPipelineDescriptor.renderPass = renderPass;
		gridRenderPipelineDescriptor.pipelineLayout = gridPipelineLayout;
		gridRenderPipelineDescriptor.vertexInputBindings = {gridVertexInputBinding};
		gridRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Physics2D/Grid.vert.spv", gridVertexShaderModuleDescriptor);
		gridRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Physics2D/Grid.frag.spv", gridFragmentShaderModuleDescriptor);
		gridRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		gridRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		gridRenderPipelineDescriptor.cullMode = CullMode::BACK;
		gridRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		gridRenderPipelineDescriptor.lineWidth = 1.0f;
		gridRenderPipelineDescriptor.minSampleShading = 0.0f;
		gridRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		gridRenderPipelineDescriptor.depthStencilState.depthTest = false;
		gridRenderPipelineDescriptor.depthStencilState.depthWrite = false;
		gridRenderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::NEVER;
		gridRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		gridRenderPipeline = createRenderPipeline(gridRenderPipelineDescriptor);
	}

	void Physics2DRenderer::initializeCenterOfMassBuffer() {
		const uint64 centerOfMassBufferSize = scene->getObjectCount() * sizeof(svec3<float32>);

		BufferDescriptor centerOfMassBufferDescriptor;
		centerOfMassBufferDescriptor.size = centerOfMassBufferSize;
		centerOfMassBufferDescriptor.usage = BufferUsage::VERTEX;
		centerOfMassBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		centerOfMassBufferDescriptor.persistent = true;

		centerOfMassBuffer = createBuffer(centerOfMassBufferDescriptor);
		centerOfMassMappedRange = centerOfMassBuffer->map<svec3<float32>>(0, centerOfMassBufferSize);
	}

	void Physics2DRenderer::initializeCenterOfMassRenderPipeline() {
		VertexInputBinding centerOfMassVertexInputBinding(InputRate::VERTEX);
		centerOfMassVertexInputBinding.registerAttribute<svec3<float32>>();

		ShaderModuleDescriptor centerOfMassVertexShaderModuleDescriptor{};
		centerOfMassVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		centerOfMassVertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor centerOfMassFragmentShaderModuleDescriptor{};
		centerOfMassFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		centerOfMassFragmentShaderModuleDescriptor.entryPoint = "main";

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor centerOfMassRenderPipelineDescriptor;
		centerOfMassRenderPipelineDescriptor.renderPass = renderPass;
		centerOfMassRenderPipelineDescriptor.pipelineLayout = featurePipelineLayout;
		centerOfMassRenderPipelineDescriptor.vertexInputBindings = {centerOfMassVertexInputBinding};
		centerOfMassRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Physics2D/CenterOfMass.vert.spv", centerOfMassVertexShaderModuleDescriptor);
		centerOfMassRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Physics2D/CenterOfMass.frag.spv", centerOfMassFragmentShaderModuleDescriptor);
		centerOfMassRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
		centerOfMassRenderPipelineDescriptor.polygonMode = PolygonMode::POINT;
		centerOfMassRenderPipelineDescriptor.cullMode = CullMode::BACK;
		centerOfMassRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		centerOfMassRenderPipelineDescriptor.lineWidth = 1.0f;
		centerOfMassRenderPipelineDescriptor.minSampleShading = 1.0f;
		centerOfMassRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		centerOfMassRenderPipelineDescriptor.depthStencilState.depthTest = true;
		centerOfMassRenderPipelineDescriptor.depthStencilState.depthWrite = true;
		centerOfMassRenderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		centerOfMassRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		centerOfMassRenderPipeline = createRenderPipeline(centerOfMassRenderPipelineDescriptor);
	}

	void Physics2DRenderer::initializeClosestPointBuffer() {
		constexpr uint64 closestPointBufferSize = 2 * sizeof(svec2<float32>);

		BufferDescriptor closestPointBufferDescriptor;
		closestPointBufferDescriptor.size = closestPointBufferSize;
		closestPointBufferDescriptor.usage = BufferUsage::VERTEX;
		closestPointBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		closestPointBufferDescriptor.persistent = true;

		closestPointBuffer = createBuffer(closestPointBufferDescriptor);
		closestPointMappedRange = closestPointBuffer->map<svec2<float32>>(0, closestPointBufferSize);
	}

	void Physics2DRenderer::initializeDistanceRenderPipeline() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

		DescriptorSetLayoutBinding viewDescriptorSetLayoutBinding;
		viewDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		viewDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		viewDescriptorSetLayoutBinding.buffers = {viewUniformBuffer};

		DescriptorSetLayout distanceDescriptorSetLayout;
		distanceDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, viewDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor distancePipelineLayoutDescriptor;
		distancePipelineLayoutDescriptor.layouts = {distanceDescriptorSetLayout};

		distancePipelineLayout = createPipelineLayout(distancePipelineLayoutDescriptor);

		VertexInputBinding distanceVertexInputBinding(InputRate::VERTEX);
		distanceVertexInputBinding.registerAttribute<svec2<float32>>();

		ShaderModuleDescriptor distanceVertexShaderModuleDescriptor{};
		distanceVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		distanceVertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor distanceFragmentShaderModuleDescriptor{};
		distanceFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		distanceFragmentShaderModuleDescriptor.entryPoint = "main";

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor distanceRenderPipelineDescriptor;
		distanceRenderPipelineDescriptor.renderPass = renderPass;
		distanceRenderPipelineDescriptor.pipelineLayout = distancePipelineLayout;
		distanceRenderPipelineDescriptor.vertexInputBindings = {distanceVertexInputBinding};
		distanceRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Physics2D/Distance.vert.spv", distanceVertexShaderModuleDescriptor);
		distanceRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Physics2D/Distance.frag.spv", distanceFragmentShaderModuleDescriptor);
		distanceRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_LIST;
		distanceRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		distanceRenderPipelineDescriptor.cullMode = CullMode::BACK;
		distanceRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		distanceRenderPipelineDescriptor.lineWidth = 1.0f;
		distanceRenderPipelineDescriptor.minSampleShading = 0.0f;
		distanceRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		distanceRenderPipelineDescriptor.depthStencilState.depthTest = false;
		distanceRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		distanceRenderPipeline = createRenderPipeline(distanceRenderPipelineDescriptor);
	}

	void Physics2DRenderer::initializeVertexFeatureBuffer() {
		constexpr uint64 vertexFeatureBufferSize = 2 * sizeof(svec2<float32>);

		BufferDescriptor vertexFeatureBufferDescriptor;
		vertexFeatureBufferDescriptor.size = vertexFeatureBufferSize;
		vertexFeatureBufferDescriptor.usage = BufferUsage::VERTEX;
		vertexFeatureBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		vertexFeatureBufferDescriptor.persistent = true;

		vertexFeatureBuffer = createBuffer(vertexFeatureBufferDescriptor);
		vertexFeatureMappedRange = vertexFeatureBuffer->map<svec2<float32>>(0, vertexFeatureBufferSize);
	}

	void Physics2DRenderer::initializeEdgeVertexFeatureBuffer() {
		constexpr uint64 edgeVertexFeatureBufferSize = 4 * sizeof(svec2<float32>);

		BufferDescriptor edgeVertexFeatureBufferDescriptor;
		edgeVertexFeatureBufferDescriptor.size = edgeVertexFeatureBufferSize;
		edgeVertexFeatureBufferDescriptor.usage = BufferUsage::VERTEX;
		edgeVertexFeatureBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		edgeVertexFeatureBufferDescriptor.persistent = true;

		edgeVertexFeatureBuffer = createBuffer(edgeVertexFeatureBufferDescriptor);
		edgeVertexFeatureMappedRange = edgeVertexFeatureBuffer->map<svec2<float32>>(0, edgeVertexFeatureBufferSize);
	}

	void Physics2DRenderer::initializeFeaturePipelineLayout() {
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

	void Physics2DRenderer::initializeVertexFeatureRenderPipeline() {
		VertexInputBinding vertexFeatureVertexInputBinding(InputRate::VERTEX);
		vertexFeatureVertexInputBinding.registerAttribute<svec2<float32>>();

		ShaderModuleDescriptor vertexFeatureVertexShaderModuleDescriptor{};
		vertexFeatureVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexFeatureVertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor vertexFeatureFragmentShaderModuleDescriptor{};
		vertexFeatureFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		vertexFeatureFragmentShaderModuleDescriptor.entryPoint = "main";

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor vertexFeatureRenderPipelineDescriptor;
		vertexFeatureRenderPipelineDescriptor.renderPass = renderPass;
		vertexFeatureRenderPipelineDescriptor.pipelineLayout = featurePipelineLayout;
		vertexFeatureRenderPipelineDescriptor.vertexInputBindings = {vertexFeatureVertexInputBinding};
		vertexFeatureRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Physics2D/VertexFeature.vert.spv", vertexFeatureVertexShaderModuleDescriptor);
		vertexFeatureRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Physics2D/VertexFeature.frag.spv", vertexFeatureFragmentShaderModuleDescriptor);
		vertexFeatureRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
		vertexFeatureRenderPipelineDescriptor.polygonMode = PolygonMode::POINT;
		vertexFeatureRenderPipelineDescriptor.cullMode = CullMode::BACK;
		vertexFeatureRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		vertexFeatureRenderPipelineDescriptor.lineWidth = 1.0f;
		vertexFeatureRenderPipelineDescriptor.minSampleShading = 1.0f;
		vertexFeatureRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		vertexFeatureRenderPipelineDescriptor.depthStencilState.depthTest = false;
		vertexFeatureRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		vertexFeatureRenderPipeline = createRenderPipeline(vertexFeatureRenderPipelineDescriptor);
	}

	void Physics2DRenderer::initializeEdgeFeatureRenderPipeline() {
		VertexInputBinding edgeFeatureVertexInputBinding(InputRate::VERTEX);
		edgeFeatureVertexInputBinding.registerAttribute<svec2<float32>>();

		ShaderModuleDescriptor edgeFeatureVertexShaderModuleDescriptor{};
		edgeFeatureVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		edgeFeatureVertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor edgeFeatureFragmentShaderModuleDescriptor{};
		edgeFeatureFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		edgeFeatureFragmentShaderModuleDescriptor.entryPoint = "main";

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor edgeFeatureRenderPipelineDescriptor;
		edgeFeatureRenderPipelineDescriptor.renderPass = renderPass;
		edgeFeatureRenderPipelineDescriptor.pipelineLayout = featurePipelineLayout;
		edgeFeatureRenderPipelineDescriptor.vertexInputBindings = {edgeFeatureVertexInputBinding};
		edgeFeatureRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Physics2D/EdgeFeature.vert.spv", edgeFeatureVertexShaderModuleDescriptor);
		edgeFeatureRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Physics2D/EdgeFeature.frag.spv", edgeFeatureFragmentShaderModuleDescriptor);
		edgeFeatureRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_LIST;
		edgeFeatureRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		edgeFeatureRenderPipelineDescriptor.cullMode = CullMode::BACK;
		edgeFeatureRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		edgeFeatureRenderPipelineDescriptor.lineWidth = 3.0f;
		edgeFeatureRenderPipelineDescriptor.minSampleShading = 0.0f;
		edgeFeatureRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		edgeFeatureRenderPipelineDescriptor.depthStencilState.depthTest = false;
		edgeFeatureRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		edgeFeatureRenderPipeline = createRenderPipeline(edgeFeatureRenderPipelineDescriptor);
	}

	void Physics2DRenderer::registerScene(const Scene2D& scene) {
		this->scene = &scene;

		uint32 vertexCount = 0;
		uint32 instanceCount = 0;

		for (const Object2D& object : scene) {
			const ref<Geometry2D>& geometry = scene.getGeometry(object.geometryIndex);

			vertexCount += geometry->getVertexCount() + 1;
			indexCount += geometry->getIndexCount();
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

		for (uint32 objectIndex = 0; objectIndex < scene.getObjectCount(); objectIndex += 1) {
			const Object2D& object = scene[objectIndex];
			const ref<Geometry2D>& geometry = scene.getGeometry(object.geometryIndex);
			const ref<Material>& material = scene.getMaterial(object.materialIndex);

			for (uint32 vertexIndex = 0; vertexIndex < geometry->vertices.size(); vertexIndex += 1) {
				const vec2<float32>& vertex = geometry->vertices[vertexIndex];

				vertexMappedRange[vertexIndex] = {vertex[0], vertex[1]};
			}

			// Duplicate the first vertex for line rendering.
			vertexMappedRange[geometry->getVertexCount()] = {geometry->vertices[0][0],geometry->vertices[0][1]};

			std::copy(geometry->indices.begin(), geometry->indices.end(), indexMappedRange);

			const float32 z = static_cast<float32>(objectIndex) / instanceCount;

			instanceMappedRange->fillColor = std::static_pointer_cast<ColorMaterial>(material)->fillColor;
			instanceMappedRange->z = {z, z, z, z};

			vertexMappedRange += geometry->getVertexCount() + 1;
			indexMappedRange += geometry->getIndexCount();
			instanceMappedRange += 1;
		}

		stagingBuffer->unmap();

		BufferDescriptor vertexBufferDescriptor;
		vertexBufferDescriptor.size = vertexBufferSize;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexBuffer = createBuffer(vertexBufferDescriptor);

		BufferDescriptor indexBufferDescriptor;
		indexBufferDescriptor.size = indexBufferSize;
		indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
		indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		indexBuffer = createBuffer(indexBufferDescriptor);

		BufferDescriptor instanceBufferDescriptor;
		instanceBufferDescriptor.size = instanceBufferSize;
		instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
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

		device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
		device->waitIdle();

		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

		DescriptorSetLayoutBinding viewDescriptorSetLayoutBinding;
		viewDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		viewDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		viewDescriptorSetLayoutBinding.buffers = {viewUniformBuffer};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>();

		VertexInputBinding fillInstanceInputBinding(InputRate::INSTANCE);
		fillInstanceInputBinding.registerAttribute<mat3x3<float32>>();
		fillInstanceInputBinding.registerAttribute<vec4<float32>>();
		fillInstanceInputBinding.registerAttribute<vec4<float32>>();

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		ColorBlendAttachment opaqueColorBlendAttachment{};
		opaqueColorBlendAttachment.blend = false;

		RenderPipelineDescriptor fillRenderPipelineDescriptor;
		fillRenderPipelineDescriptor.renderPass = renderPass;
		fillRenderPipelineDescriptor.pipelineLayout = pipelineLayout;
		fillRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, fillInstanceInputBinding};
		fillRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Physics2D/Fill.vert.spv", vertexShaderModuleDescriptor);
		fillRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Physics2D/Fill.frag.spv", fragmentShaderModuleDescriptor);
		fillRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		fillRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		fillRenderPipelineDescriptor.cullMode = CullMode::BACK;
		fillRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		fillRenderPipelineDescriptor.lineWidth = 1.0f;
		fillRenderPipelineDescriptor.minSampleShading = 0.0f;
		fillRenderPipelineDescriptor.colorBlendState.attachments = {opaqueColorBlendAttachment};
		fillRenderPipelineDescriptor.depthStencilState.depthTest = true;
		fillRenderPipelineDescriptor.depthStencilState.depthWrite = true;
		fillRenderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		fillRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		fillRenderPipeline = createRenderPipeline(fillRenderPipelineDescriptor);

		VertexInputBinding strokeInstanceInputBinding(InputRate::INSTANCE);
		strokeInstanceInputBinding.registerAttribute<mat3x3<float32>>();
		strokeInstanceInputBinding.registerAttribute<vec4<float32>>();
		strokeInstanceInputBinding.registerAttribute<vec4<float32>>();
		strokeInstanceInputBinding.registerAttribute<uint32>();

		RenderPipelineDescriptor strokeRenderPipelineDescriptor;
		strokeRenderPipelineDescriptor.renderPass = renderPass;
		strokeRenderPipelineDescriptor.pipelineLayout = pipelineLayout;
		strokeRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, strokeInstanceInputBinding};
		strokeRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Physics2D/Stroke.vert.spv", vertexShaderModuleDescriptor);
		strokeRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Physics2D/Stroke.frag.spv", fragmentShaderModuleDescriptor);
		strokeRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_STRIP;
		strokeRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		strokeRenderPipelineDescriptor.cullMode = CullMode::BACK;
		strokeRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		strokeRenderPipelineDescriptor.lineWidth = 1.0f;
		strokeRenderPipelineDescriptor.minSampleShading = 0.0f;
		strokeRenderPipelineDescriptor.colorBlendState.attachments = {opaqueColorBlendAttachment};
		strokeRenderPipelineDescriptor.depthStencilState.depthTest = true;
		strokeRenderPipelineDescriptor.depthStencilState.depthWrite = true;
		strokeRenderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		strokeRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR | DynamicState::LINE_WIDTH;

		strokeRenderPipeline = createRenderPipeline(strokeRenderPipelineDescriptor);

		constexpr uint64 placeholderBufferSize = sizeof(PlaceholderUniform);

		BufferDescriptor placeholderBufferDescriptor;
		placeholderBufferDescriptor.size = placeholderBufferSize;
		placeholderBufferDescriptor.usage = BufferUsage::UNIFORM;
		placeholderBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		placeholderBufferDescriptor.persistent = true;

		placeholderUniformBuffer = createBuffer(placeholderBufferDescriptor);
		placeholderMappedRange = placeholderUniformBuffer->map<PlaceholderUniform>(0, placeholderBufferSize);
		placeholderMappedRange->offset = {0.0f, 0.0f};

		DescriptorSetLayoutBinding placeholderDescriptorSetLayoutBinding;
		placeholderDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		placeholderDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		placeholderDescriptorSetLayoutBinding.buffers = {placeholderUniformBuffer};

		DescriptorSetLayout placeholderFillDescriptorSetLayout;
		placeholderFillDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, placeholderDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor placeholderFillPipelineLayoutDescriptor;
		placeholderFillPipelineLayoutDescriptor.layouts = {placeholderFillDescriptorSetLayout};

		placeholderFillPipelineLayout = createPipelineLayout(placeholderFillPipelineLayoutDescriptor);

		VertexInputBinding placeholderFillInstanceInputBinding(InputRate::INSTANCE);
		placeholderFillInstanceInputBinding.registerAttribute<mat3x3<float32>>();
		placeholderFillInstanceInputBinding.registerAttribute<vec4<float32>>();

		VertexInputBinding placeholderStrokeInstanceInputBinding(InputRate::INSTANCE);
		placeholderStrokeInstanceInputBinding.registerAttribute<mat3x3<float32>>();

		ColorBlendAttachment transparentColorBlendAttachment{};
		transparentColorBlendAttachment.blend = true;
		transparentColorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		transparentColorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		fillRenderPipelineDescriptor.pipelineLayout = placeholderFillPipelineLayout;
		fillRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, placeholderFillInstanceInputBinding};
		fillRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Physics2D/PlaceholderFill.vert.spv", vertexShaderModuleDescriptor);
		fillRenderPipelineDescriptor.colorBlendState.attachments = {transparentColorBlendAttachment};
		fillRenderPipelineDescriptor.depthStencilState.depthTest = false;

		placeholderFillRenderPipeline = createRenderPipeline(fillRenderPipelineDescriptor);

		DescriptorSetLayout placeholderStrokeDescriptorSetLayout;
		placeholderStrokeDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, placeholderDescriptorSetLayoutBinding, viewDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor placeholderStrokePipelineLayoutDescriptor;
		placeholderStrokePipelineLayoutDescriptor.layouts = {placeholderStrokeDescriptorSetLayout};

		placeholderStrokePipelineLayout = createPipelineLayout(placeholderStrokePipelineLayoutDescriptor);

		strokeRenderPipelineDescriptor.pipelineLayout = placeholderStrokePipelineLayout;
		strokeRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, placeholderStrokeInstanceInputBinding};
		strokeRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Physics2D/PlaceholderStroke.vert.spv", vertexShaderModuleDescriptor);
		strokeRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Physics2D/PlaceholderStroke.frag.spv", fragmentShaderModuleDescriptor);
		strokeRenderPipelineDescriptor.colorBlendState.attachments = {transparentColorBlendAttachment};
		strokeRenderPipelineDescriptor.depthStencilState.depthTest = false;
		strokeRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		placeholderStrokeRenderPipeline = createRenderPipeline(strokeRenderPipelineDescriptor);

		RenderPipelineDescriptor placeholderCenterOfMassRenderPipelineDescriptor;
		placeholderCenterOfMassRenderPipelineDescriptor.renderPass = renderPass;
		placeholderCenterOfMassRenderPipelineDescriptor.pipelineLayout = placeholderFillPipelineLayout;
		placeholderCenterOfMassRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
		placeholderCenterOfMassRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Asset/Shader/Physics2D/PlaceholderCenterOfMass.vert.spv", vertexShaderModuleDescriptor);
		placeholderCenterOfMassRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Asset/Shader/Physics2D/CenterOfMass.frag.spv", fragmentShaderModuleDescriptor);
		placeholderCenterOfMassRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
		placeholderCenterOfMassRenderPipelineDescriptor.polygonMode = PolygonMode::POINT;
		placeholderCenterOfMassRenderPipelineDescriptor.cullMode = CullMode::BACK;
		placeholderCenterOfMassRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		placeholderCenterOfMassRenderPipelineDescriptor.lineWidth = 1.0f;
		placeholderCenterOfMassRenderPipelineDescriptor.minSampleShading = 1.0f;
		placeholderCenterOfMassRenderPipelineDescriptor.colorBlendState.attachments = {opaqueColorBlendAttachment};
		placeholderCenterOfMassRenderPipelineDescriptor.depthStencilState.depthTest = false;
		placeholderCenterOfMassRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		placeholderCenterOfMassRenderPipeline = createRenderPipeline(placeholderCenterOfMassRenderPipelineDescriptor);

		initializeCenterOfMassBuffer();
		initializeCenterOfMassRenderPipeline();
	}

	const vec2<float32>& Physics2DRenderer::getNdcMousePosition() const {
		return ndcMousePosition;
	}

	void Physics2DRenderer::setMousePosition(float32 x, float32 y) {
		mousePosition = {x, y};

		const ref<Swapchain>& swapchain = getSwapchain();
		const float32 horizontalCellCount = swapchain->getExtentWidth() / cameraMappedRange->zoomLevel;
		const float32 verticalCellCount = swapchain->getExtentHeight() / cameraMappedRange->zoomLevel;

		x = (x / swapchain->getExtentWidth() - 0.5f) * horizontalCellCount;
		y = -(y / swapchain->getExtentHeight() - 0.5f) * verticalCellCount;

		ndcMousePosition = {x, y};
	}

	void Physics2DRenderer::zoom(float32 zoomLevelDelta) {
		float32 direction = zoomLevelDelta;

		if (direction != 0.0f) {
			direction /= abs(zoomLevelDelta);
		}

		float32 zoomLevel = cameraMappedRange->zoomLevel + direction;

		// TODO: lyah::max
		if (zoomLevel < MIN_ZOOM_LEVEL) {
			zoomLevel = MIN_ZOOM_LEVEL;
		}

		// TODO: lyah::min
		if (zoomLevel > MAX_ZOOM_LEVEL) {
			zoomLevel = MAX_ZOOM_LEVEL;
		}

		cameraMappedRange->zoomLevel = zoomLevel;

		setMousePosition(mousePosition[0], mousePosition[1]);
	}

	void Physics2DRenderer::setClosestPoints(const vec2<float32>& closestPointA, const vec2<float32>& closestPointB) const {
		closestPointMappedRange[0] = {closestPointA[0], closestPointA[1]};
		closestPointMappedRange[1] = {closestPointB[0], closestPointB[1]};
	}

	void Physics2DRenderer::updateCentersOfMass() const {
		for (uint32 instanceIndex = 0; instanceIndex < scene->getObjectCount(); instanceIndex += 1) {
			const Object2D& A = (*scene)[instanceIndex];
			const float32 z = static_cast<float32>(instanceIndex) / scene->getObjectCount();

			centerOfMassMappedRange[instanceIndex] = {A.position[0], A.position[1], z};
		}
	}

	void Physics2DRenderer::selectObject(uint32 objectIndex) {
		if (selectedObjectIndex != std::numeric_limits<uint32>::max()) {
			instanceMappedRange[selectedObjectIndex].selected = 0;
		}

		selectedObjectIndex = objectIndex;

		if (selectedObjectIndex != std::numeric_limits<uint32>::max()) {
			instanceMappedRange[selectedObjectIndex].selected = 1;
		}
	}

	void Physics2DRenderer::setPlaceholderOffset(const vec2<float32>& offset) const {
		placeholderMappedRange->offset = {offset[0], offset[1]};
	}

	void Physics2DRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		updateCentersOfMass();

		const ref<Swapchain>& swapchain = getSwapchain();
		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		const ClearValue colorAttachmentClearValue = {1.0f, 1.0f, 1.0f, 1.0f};

		const ClearValue resolveAttachmentClearValue = {1.0f, 1.0f, 1.0f, 1.0f};

		ClearValue depthStencilAttachmentClearValue{};
		depthStencilAttachmentClearValue.depth = 0.0f;
		depthStencilAttachmentClearValue.stencil = 0;

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = renderPass;
		beginRenderPassDescriptor.swapchain = swapchain;
		beginRenderPassDescriptor.clearValues = {colorAttachmentClearValue, resolveAttachmentClearValue, depthStencilAttachmentClearValue};
		beginRenderPassDescriptor.imageIndex = getImageIndex();

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());

		renderGrid(commandEncoder);
		renderScene(commandEncoder);
		renderDistance(commandEncoder);
		renderClosestFeatures(commandEncoder);

		if (selectedObjectIndex != std::numeric_limits<uint32>::max()) {
			renderSelectedObjectPlaceholder(commandEncoder);
		}

		commandEncoder->endRenderPass();
	}

	void Physics2DRenderer::renderGrid(const ref<CommandEncoder>& commandEncoder) const {
		commandEncoder->bindRenderPipeline(gridRenderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindVertexBuffer(0, gridVertexBuffer, 0, sizeof(svec2<float32>));
		commandEncoder->draw(3, 1, 0);
	}

	void Physics2DRenderer::renderScene(const ref<CommandEncoder>& commandEncoder) const {
		const uint32 objectCount = scene->getObjectCount();

		commandEncoder->bindRenderPipeline(fillRenderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);

		for (uint32 objectIndex = 0, indexOffset = 0, vertexOffset = 0; objectIndex < objectCount; objectIndex += 1) {
			const Object2D& object = (*scene)[objectIndex];
			const ref<Geometry2D>& geometry = object.getGeometry();
			const uint32 indexCount = geometry->getIndexCount();
			const uint32 vertexCount = geometry->getVertexCount() + 1;

			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, objectIndex * sizeof(Instance), sizeof(Instance));
			commandEncoder->drawIndexed(indexCount, 1, indexOffset);

			indexOffset += indexCount;
			vertexOffset += vertexCount;
		}

		commandEncoder->bindRenderPipeline(strokeRenderPipeline);
		commandEncoder->bindAllDescriptorSets();

		for (uint32 objectIndex = 0, vertexOffset = 0; objectIndex < objectCount; objectIndex += 1) {
			const Object2D& object = (*scene)[objectIndex];
			const ref<Geometry2D>& geometry = object.getGeometry();
			const uint32 vertexCount = geometry->getVertexCount() + 1;

			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, objectIndex * sizeof(Instance), sizeof(Instance));

			if (objectIndex == selectedObjectIndex) {
				commandEncoder->setLineWidth(2.0f);
			}
			else {
				commandEncoder->setLineWidth(1.0f);
			}

			commandEncoder->draw(vertexCount, 1, 0);

			vertexOffset += vertexCount;
		}

		commandEncoder->bindRenderPipeline(centerOfMassRenderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, 0, sizeof(svec3<float32>));
		commandEncoder->draw(objectCount, 1, 0);
	}

	void Physics2DRenderer::renderDistance(const ref<CommandEncoder>& commandEncoder) const {
		commandEncoder->bindRenderPipeline(distanceRenderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindVertexBuffer(0, closestPointBuffer, 0, sizeof(svec2<float32>));
		commandEncoder->draw(2, 1, 0);
	}

	void Physics2DRenderer::renderClosestFeatures(const ref<CommandEncoder>& commandEncoder) const {
		if (vertexFeatureCount > 0) {
			commandEncoder->bindRenderPipeline(vertexFeatureRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->bindVertexBuffer(0, vertexFeatureBuffer, 0, sizeof(svec2<float32>));
			commandEncoder->draw(vertexFeatureCount, 1, 0);
		}

		if (edgeVertexFeatureCount > 0) {
			commandEncoder->bindRenderPipeline(edgeFeatureRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->bindVertexBuffer(0, edgeVertexFeatureBuffer, 0, sizeof(svec2<float32>));
			commandEncoder->draw(edgeVertexFeatureCount, 1, 0);
		}
	}

	void Physics2DRenderer::renderSelectedObjectPlaceholder(const ref<CommandEncoder>& commandEncoder) const {
		uint32 indexOffset = 0;
		uint32 vertexOffset = 0;
		uint32 objectIndex = 0;

		while (objectIndex < selectedObjectIndex) {
			const Object2D& object = (*scene)[objectIndex];
			const ref<Geometry2D>& geometry = object.getGeometry();

			indexOffset += geometry->getIndexCount();
			vertexOffset += geometry->getVertexCount() + 1;
			objectIndex += 1;
		}

		commandEncoder->bindRenderPipeline(placeholderCenterOfMassRenderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, selectedObjectIndex * sizeof(svec3<float32>), sizeof(svec2<float32>));
		commandEncoder->draw(1, 1, 0);

		const uint32 indexCount = (*scene)[selectedObjectIndex].getGeometry()->getIndexCount();

		commandEncoder->bindRenderPipeline(placeholderFillRenderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
		commandEncoder->bindVertexBuffer(1, instanceBuffer, selectedObjectIndex * sizeof(Instance), sizeof(Instance));
		commandEncoder->drawIndexed(indexCount, 1, 0);

		const uint32 vertexCount = (*scene)[selectedObjectIndex].getGeometry()->getVertexCount() + 1;

		commandEncoder->bindRenderPipeline(placeholderStrokeRenderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
		commandEncoder->draw(vertexCount, 1, 0);
	}

	void Physics2DRenderer::resize(WindowResizeEvent& event) {
		const float32 width = static_cast<float32>(event.getWidth());
		const float32 height = static_cast<float32>(event.getHeight());
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

		updateColorAttachment();
		updateDepthStencilAttachment();

		const ref<Swapchain>& swapchain = getSwapchain();

		swapchain->getImages(renderPass);
	}

	void Physics2DRenderer::writeInstanceTransform(uint32 instanceIndex, const mat3x3<float32>& transform) const {
		instanceMappedRange[instanceIndex].world = transform;
	}

	void Physics2DRenderer::clearClosestFeatureCounters() {
		vertexFeatureCount = 0;
		edgeVertexFeatureCount = 0;
	}

	void Physics2DRenderer::registerClosestFeature(const ClosestFeature2D& closestFeature) {
		switch (closestFeature.count) {
			case 1:
				vertexFeatureMappedRange[vertexFeatureCount] = {closestFeature.vertex[0], closestFeature.vertex[1]};
				vertexFeatureCount += 1;

				break;
			case 2:
				edgeVertexFeatureMappedRange[edgeVertexFeatureCount + 0] = {closestFeature.edge[0][0], closestFeature.edge[0][1]};
				edgeVertexFeatureMappedRange[edgeVertexFeatureCount + 1] = {closestFeature.edge[1][0], closestFeature.edge[1][1]};
				edgeVertexFeatureCount += 2;

				break;
			default:
				DEBUG_ASSERT(false);

				break;
		}
	}
}