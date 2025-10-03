#include "pch.hpp"

#include "Physics2DRenderer.hpp"

#include "Engine/API/Material/ColorFillStrokeMaterial.hpp"

namespace ProjectA {
	// TODO
	/* void Physics2DRenderer::build() {
		ENGINE_STATIC_ASSERT(SAMPLE_COUNT > 1);

		const ref<Surface> surface = getSurface();

		Attachment swapchainAttachment{};
		swapchainAttachment.format = surface->getFormat();
		swapchainAttachment.sampleCount = SAMPLE_COUNT;
		swapchainAttachment.loadOperation = LoadOperation::CLEAR;
		swapchainAttachment.storeOperation = StoreOperation::STORE;

		// TODO: Restore layout.
		// swapchainAttachment.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
		swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

		Attachment depthStencilAttachment{};
		depthStencilAttachment.format = ImageFormat::D32_SFLOAT;
		depthStencilAttachment.sampleCount = SAMPLE_COUNT;
		depthStencilAttachment.loadOperation = LoadOperation::CLEAR;
		depthStencilAttachment.storeOperation = StoreOperation::UNDEFINED;
		depthStencilAttachment.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		AttachmentReference swapchainAttachmentReference{};
		swapchainAttachmentReference.index = 0;
		swapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		AttachmentReference depthStencilAttachmentReference{};
		depthStencilAttachmentReference.index = 1;
		depthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		SubpassDescriptor subpassDescriptor{};
		subpassDescriptor.colorAttachmentReferences = {swapchainAttachmentReference};
		subpassDescriptor.depthStencilAttachmentReference = depthStencilAttachmentReference;

		const ref<Subpass> subpass = createSubpass(subpassDescriptor);

		RenderPassDescriptor renderPassDescriptor{};
		renderPassDescriptor.attachments = {swapchainAttachment, depthStencilAttachment};
		renderPassDescriptor.subpasses = {subpass};

		renderPass = createRenderPass(renderPassDescriptor);

		recreateFramebuffers();

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

		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 width = static_cast<float32>(swapchain->getExtent().width);
		const float32 height = static_cast<float32>(swapchain->getExtent().height);
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

	void Physics2DRenderer::recreateFramebuffers() {
		const ref<Swapchain>& swapchain = getSwapchain();
		const Extent2D extent = swapchain->getExtent();

		AttachmentImageDescriptor attachmentImageDescriptor{};
		attachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		attachmentImageDescriptor.extent.width = extent.width;
		attachmentImageDescriptor.extent.height = extent.height;
		attachmentImageDescriptor.extent.depth = 1;
		attachmentImageDescriptor.mipLevelCount = 1;
		attachmentImageDescriptor.layerCount = 1;

		ImageViewDescriptor attachmentImageViewDescriptor{};
		attachmentImageViewDescriptor.mipLevelOffset = 0;
		attachmentImageViewDescriptor.mipLevelCount = 1;
		attachmentImageViewDescriptor.layerOffset = 0;
		attachmentImageViewDescriptor.layerCount = 1;

		colorImage = renderPass->createColorAttachmentImage(attachmentImageDescriptor, 0);
		colorImageView = colorImage->createView(attachmentImageViewDescriptor);

		depthStencilImage = renderPass->createDepthStencilAttachmentImage(attachmentImageDescriptor, 0);
		depthStencilImageView = depthStencilImage->createView(attachmentImageViewDescriptor);

		for (ref<Framebuffer>& framebuffer : framebuffers) {
			framebuffer.~shared_ptr();
		}

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor{};
		framebufferDescriptor.renderPass = renderPass;
		framebufferDescriptor.imageViews = {colorImageView, depthStencilImageView, nullptr};
		framebufferDescriptor.extent = extent;
		framebufferDescriptor.layerCount = 1;

		for (uint32 framebufferIndex = 0; framebufferIndex < framebuffers.count; framebufferIndex += 1) {
			framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
		}
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
		const ref<Device> device = getDevice();

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

		sequence<BufferCopyRegion> gridVertexBufferCopyRegions(1);

		BufferCopyRegion& gridVertexBufferCopyRegion = gridVertexBufferCopyRegions[0];
		gridVertexBufferCopyRegion.sourceOffset = 0;
		gridVertexBufferCopyRegion.destinationOffset = 0;
		gridVertexBufferCopyRegion.size = gridVertexBufferSize;

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(gridVertexStagingBuffer, gridVertexBuffer, gridVertexBufferCopyRegions);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		device->waitIdle();
	}

	void Physics2DRenderer::initializeGridRenderPipeline() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.shaderStages = ShaderStage::FRAGMENT;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

		// TODO: The camera's projection isn't needed here, but the offset must be a multiple of the minUniformBufferAlignment physical device limit.
		cameraDescriptorSetLayoutBinding.bufferOffset = 0;
		// cameraDescriptorSetLayoutBinding.bufferOffset = sizeof(CameraUniform::projection);

		DescriptorSetLayoutBinding viewDescriptorSetLayoutBinding;
		viewDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		viewDescriptorSetLayoutBinding.shaderStages = ShaderStage::FRAGMENT;
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
		gridRenderPipelineDescriptor.pipelineLayout = gridPipelineLayout;
		gridRenderPipelineDescriptor.vertexInputBindings = {gridVertexInputBinding};
		gridRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Physics2D/Grid.vert", gridVertexShaderModuleDescriptor);
		gridRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Physics2D/Grid.frag", gridFragmentShaderModuleDescriptor);
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
		gridRenderPipelineDescriptor.renderPass = renderPass;
		gridRenderPipelineDescriptor.subpassIndex = 0;

		gridRenderPipeline = createRenderPipeline(gridRenderPipelineDescriptor);
	}

	void Physics2DRenderer::initializeCenterOfMassBuffer() {
		const uint32 rigidBodyCount = scene->getComponentPool<RigidBody2D>().componentCount;
		const uint64 centerOfMassBufferSize = rigidBodyCount * sizeof(vec3<float32>);

		BufferDescriptor centerOfMassBufferDescriptor;
		centerOfMassBufferDescriptor.size = centerOfMassBufferSize;
		centerOfMassBufferDescriptor.usage = BufferUsage::VERTEX;
		centerOfMassBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		centerOfMassBufferDescriptor.persistent = true;

		centerOfMassBuffer = createBuffer(centerOfMassBufferDescriptor);
		centerOfMassMappedRange = centerOfMassBuffer->map<vec3<float32>>(0, centerOfMassBufferSize);
	}

	void Physics2DRenderer::initializeCenterOfMassRenderPipeline() {
		VertexInputBinding centerOfMassVertexInputBinding(InputRate::VERTEX);
		centerOfMassVertexInputBinding.registerAttribute<vec3<float32>>();

		ShaderModuleDescriptor centerOfMassVertexShaderModuleDescriptor{};
		centerOfMassVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		centerOfMassVertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor centerOfMassFragmentShaderModuleDescriptor{};
		centerOfMassFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		centerOfMassFragmentShaderModuleDescriptor.entryPoint = "main";

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor centerOfMassRenderPipelineDescriptor;
		centerOfMassRenderPipelineDescriptor.pipelineLayout = featurePipelineLayout;
		centerOfMassRenderPipelineDescriptor.vertexInputBindings = {centerOfMassVertexInputBinding};
		centerOfMassRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Physics2D/CenterOfMass.vert", centerOfMassVertexShaderModuleDescriptor);
		centerOfMassRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Physics2D/CenterOfMass.frag", centerOfMassFragmentShaderModuleDescriptor);
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
		centerOfMassRenderPipelineDescriptor.renderPass = renderPass;
		centerOfMassRenderPipelineDescriptor.subpassIndex = 0;

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
		cameraDescriptorSetLayoutBinding.shaderStages = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

		DescriptorSetLayoutBinding viewDescriptorSetLayoutBinding;
		viewDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		viewDescriptorSetLayoutBinding.shaderStages = ShaderStage::FRAGMENT;
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
		distanceRenderPipelineDescriptor.pipelineLayout = distancePipelineLayout;
		distanceRenderPipelineDescriptor.vertexInputBindings = {distanceVertexInputBinding};
		distanceRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Physics2D/Distance.vert", distanceVertexShaderModuleDescriptor);
		distanceRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Physics2D/Distance.frag", distanceFragmentShaderModuleDescriptor);
		distanceRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_LIST;
		distanceRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		distanceRenderPipelineDescriptor.cullMode = CullMode::BACK;
		distanceRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		distanceRenderPipelineDescriptor.lineWidth = 1.0f;
		distanceRenderPipelineDescriptor.minSampleShading = 0.0f;
		distanceRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		distanceRenderPipelineDescriptor.depthStencilState.depthTest = false;
		distanceRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		distanceRenderPipelineDescriptor.renderPass = renderPass;
		distanceRenderPipelineDescriptor.subpassIndex = 0;

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
		cameraDescriptorSetLayoutBinding.shaderStages = ShaderStage::VERTEX;
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
		vertexFeatureRenderPipelineDescriptor.pipelineLayout = featurePipelineLayout;
		vertexFeatureRenderPipelineDescriptor.vertexInputBindings = {vertexFeatureVertexInputBinding};
		vertexFeatureRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Physics2D/VertexFeature.vert", vertexFeatureVertexShaderModuleDescriptor);
		vertexFeatureRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Physics2D/VertexFeature.frag", vertexFeatureFragmentShaderModuleDescriptor);
		vertexFeatureRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
		vertexFeatureRenderPipelineDescriptor.polygonMode = PolygonMode::POINT;
		vertexFeatureRenderPipelineDescriptor.cullMode = CullMode::BACK;
		vertexFeatureRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		vertexFeatureRenderPipelineDescriptor.lineWidth = 1.0f;
		vertexFeatureRenderPipelineDescriptor.minSampleShading = 1.0f;
		vertexFeatureRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		vertexFeatureRenderPipelineDescriptor.depthStencilState.depthTest = false;
		vertexFeatureRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		vertexFeatureRenderPipelineDescriptor.renderPass = renderPass;
		vertexFeatureRenderPipelineDescriptor.subpassIndex = 0;

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
		edgeFeatureRenderPipelineDescriptor.pipelineLayout = featurePipelineLayout;
		edgeFeatureRenderPipelineDescriptor.vertexInputBindings = {edgeFeatureVertexInputBinding};
		edgeFeatureRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Physics2D/EdgeFeature.vert", edgeFeatureVertexShaderModuleDescriptor);
		edgeFeatureRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Physics2D/EdgeFeature.frag", edgeFeatureFragmentShaderModuleDescriptor);
		edgeFeatureRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_LIST;
		edgeFeatureRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		edgeFeatureRenderPipelineDescriptor.cullMode = CullMode::BACK;
		edgeFeatureRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		edgeFeatureRenderPipelineDescriptor.lineWidth = 3.0f;
		edgeFeatureRenderPipelineDescriptor.minSampleShading = 0.0f;
		edgeFeatureRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		edgeFeatureRenderPipelineDescriptor.depthStencilState.depthTest = false;
		edgeFeatureRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		edgeFeatureRenderPipelineDescriptor.renderPass = renderPass;
		edgeFeatureRenderPipelineDescriptor.subpassIndex = 0;

		edgeFeatureRenderPipeline = createRenderPipeline(edgeFeatureRenderPipelineDescriptor);
	}

	void Physics2DRenderer::registerScene(const Scene2D& scene) {
		this->scene = &scene;

		uint32 vertexCount = 0;
		uint32 instanceCount = 0;

		const ComponentPool<RigidBody2D>& rigidBodyPool = scene.getComponentPool<RigidBody2D>();

		for (uint32 rigidBodyIndex = 0; rigidBodyIndex < rigidBodyPool.componentCount; rigidBodyIndex += 1) {
			const Entity entity = rigidBodyPool.componentIndexToEntity[rigidBodyIndex];
			const ref<Geometry2D>& geometry = scene.getGeometry(entity);

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
		uint16* indexMappedRange = pointer_cast<uint16*>(vertexMappedRange + vertexCount);
		Instance* instanceMappedRange = pointer_cast<Instance*>(indexMappedRange + indexCount);

		for (uint32 rigidBodyIndex = 0; rigidBodyIndex < rigidBodyPool.componentCount; rigidBodyIndex += 1) {
			const Entity entity = rigidBodyPool.componentIndexToEntity[rigidBodyIndex];
			const ref<Geometry2D>& geometry = scene.getGeometry(entity);
			const ref<Material>& material = scene.getMaterial(entity);

			for (uint32 vertexIndex = 0; vertexIndex < geometry->vertices.count; vertexIndex += 1) {
				const vec2<float32>& vertex = geometry->vertices[vertexIndex];

				vertexMappedRange[vertexIndex] = {vertex[0], vertex[1]};
			}

			// Duplicate the first vertex for line rendering.
			vertexMappedRange[geometry->getVertexCount()] = {geometry->vertices[0][0],geometry->vertices[0][1]};

			std::copy(geometry->indices.begin(), geometry->indices.end(), indexMappedRange);

			instanceMappedRange->fillColor = std::static_pointer_cast<ColorFillStrokeMaterial>(material)->fillColor;
			instanceMappedRange->strokeColor = std::static_pointer_cast<ColorFillStrokeMaterial>(material)->strokeColor;
			instanceMappedRange->z = static_cast<float32>(entity) / instanceCount;

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

		sequence<BufferCopyRegion> vertexBufferCopyRegions(1);

		BufferCopyRegion& vertexBufferCopyRegion = vertexBufferCopyRegions[0];
		vertexBufferCopyRegion.sourceOffset = 0;
		vertexBufferCopyRegion.destinationOffset = 0;
		vertexBufferCopyRegion.size = vertexBufferSize;

		sequence<BufferCopyRegion> indexBufferCopyRegions(1);

		BufferCopyRegion& indexBufferCopyRegion = indexBufferCopyRegions[0];
		indexBufferCopyRegion.sourceOffset = vertexBufferSize;
		indexBufferCopyRegion.destinationOffset = 0;
		indexBufferCopyRegion.size = indexBufferSize;

		sequence<BufferCopyRegion> instanceBufferCopyRegions(1);

		BufferCopyRegion& instanceBufferCopyRegion = instanceBufferCopyRegions[0];
		instanceBufferCopyRegion.sourceOffset = vertexBufferSize + indexBufferSize;
		instanceBufferCopyRegion.destinationOffset = 0;
		instanceBufferCopyRegion.size = instanceBufferSize;

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, vertexBufferCopyRegions);
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, indexBufferCopyRegions);
		commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, instanceBufferCopyRegions);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		device->waitIdle();

		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.shaderStages = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

		DescriptorSetLayoutBinding viewDescriptorSetLayoutBinding;
		viewDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		viewDescriptorSetLayoutBinding.shaderStages = ShaderStage::FRAGMENT;
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
		fillInstanceInputBinding.registerAttribute<float32>();

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		ColorBlendAttachment opaqueColorBlendAttachment{};
		opaqueColorBlendAttachment.blend = false;

		ColorBlendAttachment transparentColorBlendAttachment{};
		transparentColorBlendAttachment.blend = true;
		transparentColorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		transparentColorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		RenderPipelineDescriptor fillRenderPipelineDescriptor;
		fillRenderPipelineDescriptor.pipelineLayout = pipelineLayout;
		fillRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, fillInstanceInputBinding};
		fillRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Physics2D/Fill.vert", vertexShaderModuleDescriptor);
		fillRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Physics2D/Fill.frag", fragmentShaderModuleDescriptor);
		fillRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		fillRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		fillRenderPipelineDescriptor.cullMode = CullMode::BACK;
		fillRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		fillRenderPipelineDescriptor.lineWidth = 1.0f;
		fillRenderPipelineDescriptor.minSampleShading = 0.0f;
		fillRenderPipelineDescriptor.colorBlendState.attachments = {transparentColorBlendAttachment};
		fillRenderPipelineDescriptor.depthStencilState.depthTest = true;
		fillRenderPipelineDescriptor.depthStencilState.depthWrite = true;
		fillRenderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		fillRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		fillRenderPipelineDescriptor.renderPass = renderPass;
		fillRenderPipelineDescriptor.subpassIndex = 0;

		fillRenderPipeline = createRenderPipeline(fillRenderPipelineDescriptor);

		VertexInputBinding strokeInstanceInputBinding(InputRate::INSTANCE);
		strokeInstanceInputBinding.registerAttribute<mat3x3<float32>>();
		strokeInstanceInputBinding.registerAttribute<vec4<float32>>();
		strokeInstanceInputBinding.registerAttribute<vec4<float32>>();
		strokeInstanceInputBinding.registerAttribute<float32>();
		strokeInstanceInputBinding.registerAttribute<uint32>();

		RenderPipelineDescriptor strokeRenderPipelineDescriptor;
		strokeRenderPipelineDescriptor.pipelineLayout = pipelineLayout;
		strokeRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, strokeInstanceInputBinding};
		strokeRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Physics2D/Stroke.vert", vertexShaderModuleDescriptor);
		strokeRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Physics2D/Stroke.frag", fragmentShaderModuleDescriptor);
		strokeRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_STRIP;
		strokeRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		strokeRenderPipelineDescriptor.cullMode = CullMode::BACK;
		strokeRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		strokeRenderPipelineDescriptor.lineWidth = 1.0f;
		strokeRenderPipelineDescriptor.minSampleShading = 0.0f;
		strokeRenderPipelineDescriptor.colorBlendState.attachments = {transparentColorBlendAttachment};
		strokeRenderPipelineDescriptor.depthStencilState.depthTest = true;
		strokeRenderPipelineDescriptor.depthStencilState.depthWrite = true;
		strokeRenderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		strokeRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR | DynamicState::LINE_WIDTH;
		strokeRenderPipelineDescriptor.renderPass = renderPass;
		strokeRenderPipelineDescriptor.subpassIndex = 0;

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
		placeholderDescriptorSetLayoutBinding.shaderStages = ShaderStage::VERTEX;
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

		fillRenderPipelineDescriptor.pipelineLayout = placeholderFillPipelineLayout;
		fillRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, placeholderFillInstanceInputBinding};
		fillRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Physics2D/PlaceholderFill.vert", vertexShaderModuleDescriptor);
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
		strokeRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Physics2D/PlaceholderStroke.vert", vertexShaderModuleDescriptor);
		strokeRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Physics2D/PlaceholderStroke.frag", fragmentShaderModuleDescriptor);
		strokeRenderPipelineDescriptor.colorBlendState.attachments = {transparentColorBlendAttachment};
		strokeRenderPipelineDescriptor.depthStencilState.depthTest = false;
		strokeRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		placeholderStrokeRenderPipeline = createRenderPipeline(strokeRenderPipelineDescriptor);

		RenderPipelineDescriptor placeholderCenterOfMassRenderPipelineDescriptor;
		placeholderCenterOfMassRenderPipelineDescriptor.pipelineLayout = placeholderFillPipelineLayout;
		placeholderCenterOfMassRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
		placeholderCenterOfMassRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("Physics2D/PlaceholderCenterOfMass.vert", vertexShaderModuleDescriptor);
		placeholderCenterOfMassRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Physics2D/CenterOfMass.frag", fragmentShaderModuleDescriptor);
		placeholderCenterOfMassRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
		placeholderCenterOfMassRenderPipelineDescriptor.polygonMode = PolygonMode::POINT;
		placeholderCenterOfMassRenderPipelineDescriptor.cullMode = CullMode::BACK;
		placeholderCenterOfMassRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		placeholderCenterOfMassRenderPipelineDescriptor.lineWidth = 1.0f;
		placeholderCenterOfMassRenderPipelineDescriptor.minSampleShading = 1.0f;
		placeholderCenterOfMassRenderPipelineDescriptor.colorBlendState.attachments = {opaqueColorBlendAttachment};
		placeholderCenterOfMassRenderPipelineDescriptor.depthStencilState.depthTest = false;
		placeholderCenterOfMassRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		placeholderCenterOfMassRenderPipelineDescriptor.renderPass = renderPass;
		placeholderCenterOfMassRenderPipelineDescriptor.subpassIndex = 0;

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
		const float32 horizontalCellCount = swapchain->getExtent().width / cameraMappedRange->zoomLevel;
		const float32 verticalCellCount = swapchain->getExtent().height / cameraMappedRange->zoomLevel;

		x = (x / swapchain->getExtent().width - 0.5f) * horizontalCellCount;
		y = -(y / swapchain->getExtent().height - 0.5f) * verticalCellCount;

		ndcMousePosition = {x, y};
	}

	void Physics2DRenderer::zoom(float32 zoomLevelDelta) {
		float32 direction = zoomLevelDelta;

		if (direction != 0.0f) {
			direction /= abs(zoomLevelDelta);
		}

		const float32 zoomLevel = clamp(cameraMappedRange->zoomLevel + direction, MIN_ZOOM_LEVEL, MAX_ZOOM_LEVEL);

		cameraMappedRange->zoomLevel = zoomLevel;

		setMousePosition(mousePosition[0], mousePosition[1]);
	}

	void Physics2DRenderer::setClosestPoints(const vec2<float32>& closestPointA, const vec2<float32>& closestPointB) const {
		closestPointMappedRange[0] = {closestPointA[0], closestPointA[1]};
		closestPointMappedRange[1] = {closestPointB[0], closestPointB[1]};
	}

	void Physics2DRenderer::updateCentersOfMass() const {
		const ComponentPool<RigidBody2D>& rigidBodyPool = scene->getComponentPool<RigidBody2D>();

		for (uint32 rigidBodyIndex = 0; rigidBodyIndex < rigidBodyPool.componentCount; rigidBodyIndex += 1) {
			const RigidBody2D& rigidBody = rigidBodyPool.components[rigidBodyIndex];

			centerOfMassMappedRange[rigidBodyIndex] = {
				rigidBody.position[0],
				rigidBody.position[1],
				static_cast<float32>(rigidBodyIndex) / rigidBodyPool.componentCount,
			};
		}
	}

	void Physics2DRenderer::selectEntity(Entity entity) {
		if (selectedInstanceIndex != max<uint32>()) {
			instanceMappedRange[selectedInstanceIndex].selected = 0;
		}

		if (entity == max<uint32>()) {
			selectedInstanceIndex = entity;

			return;
		}

		const ComponentPool<RigidBody2D>& instancePool = scene->getComponentPool<RigidBody2D>();

		selectedInstanceIndex = instancePool.entityToComponentIndex[entity];
		instanceMappedRange[selectedInstanceIndex].selected = 1;
	}

	void Physics2DRenderer::setPlaceholderOffset(const vec2<float32>& offset) const {
		placeholderMappedRange->offset = {offset[0], offset[1]};
	}

	void Physics2DRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		updateCentersOfMass();

		const ClearValue colorClearValue = {0.0f, 0.0f, 0.0f, 1.0f};

		ClearValue depthStencilClearValue{};
		depthStencilClearValue.depth = 0.0f;
		depthStencilClearValue.stencil = 0;

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
		beginRenderPassDescriptor.clearValues = {colorClearValue, depthStencilClearValue, colorClearValue};

		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(swapchainExtent.width);
		const float32 height = static_cast<float32>(swapchainExtent.height);

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchainExtent);

		renderGrid(commandEncoder);
		renderScene(commandEncoder);
		renderDistance(commandEncoder);
		renderFeatures(commandEncoder);

		if (selectedInstanceIndex != max<uint32>()) {
			renderSelectedInstancePlaceholder(commandEncoder);
		}

		commandEncoder->endRenderPass();
	}

	void Physics2DRenderer::renderGrid(const ref<CommandEncoder>& commandEncoder) const {
		commandEncoder->bindRenderPipeline(gridRenderPipeline);

		// TODO
		BREAKPOINT();
		// commandEncoder->bindDescriptorSets();

		commandEncoder->bindVertexBuffer(0, gridVertexBuffer, 0, sizeof(svec2<float32>));
		commandEncoder->draw(3, 1, 0, 0);
	}

	void Physics2DRenderer::renderScene(const ref<CommandEncoder>& commandEncoder) const {
		commandEncoder->bindRenderPipeline(fillRenderPipeline);

		// TODO
		BREAKPOINT();
		// commandEncoder->bindDescriptorSets();

		commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);

		uint32 rigidBodyIndex = 0;
		uint32 vertexOffset = 0;

		const ComponentPool<RigidBody2D>& rigidBodyPool = scene->getComponentPool<RigidBody2D>();

		for (uint32 indexOffset = 0; rigidBodyIndex < rigidBodyPool.componentCount; rigidBodyIndex += 1) {
			const ref<Geometry2D>& geometry = rigidBodyPool.components[rigidBodyIndex].geometry;
			const uint32 indexCount = geometry->getIndexCount();
			const uint32 vertexCount = geometry->getVertexCount() + 1;

			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, rigidBodyIndex * sizeof(Instance), sizeof(Instance));
			commandEncoder->drawIndexed(indexCount, 1, indexOffset, 0, 0);

			indexOffset += indexCount;
			vertexOffset += vertexCount;
		}

		commandEncoder->bindRenderPipeline(strokeRenderPipeline);

		// TODO
		BREAKPOINT();
		// commandEncoder->bindDescriptorSets();

		rigidBodyIndex = 0;
		vertexOffset = 0;

		for (; rigidBodyIndex < rigidBodyPool.componentCount; rigidBodyIndex += 1) {
			const ref<Geometry2D>& geometry = rigidBodyPool.components[rigidBodyIndex].geometry;
			const uint32 vertexCount = geometry->getVertexCount() + 1;

			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, rigidBodyIndex * sizeof(Instance), sizeof(Instance));

			if (rigidBodyIndex == selectedInstanceIndex) {
				commandEncoder->setLineWidth(2.0f);
			}
			else {
				commandEncoder->setLineWidth(1.0f);
			}

			commandEncoder->draw(vertexCount, 1, 0, 0);

			vertexOffset += vertexCount;
		}

		commandEncoder->bindRenderPipeline(centerOfMassRenderPipeline);

		// TODO
		BREAKPOINT();
		// commandEncoder->bindDescriptorSets();

		commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, 0, sizeof(vec3<float32>));
		commandEncoder->draw(rigidBodyPool.componentCount, 1, 0, 0);
	}

	void Physics2DRenderer::renderDistance(const ref<CommandEncoder>& commandEncoder) const {
		commandEncoder->bindRenderPipeline(distanceRenderPipeline);

		// TODO
		BREAKPOINT();
		// commandEncoder->bindDescriptorSets();

		commandEncoder->bindVertexBuffer(0, closestPointBuffer, 0, sizeof(svec2<float32>));
		commandEncoder->draw(2, 1, 0, 0);
	}

	void Physics2DRenderer::renderFeatures(const ref<CommandEncoder>& commandEncoder) const {
		if (vertexFeatureCount > 0) {
			commandEncoder->bindRenderPipeline(vertexFeatureRenderPipeline);

			// TODO
			BREAKPOINT();
			// commandEncoder->bindDescriptorSets();

			commandEncoder->bindVertexBuffer(0, vertexFeatureBuffer, 0, sizeof(svec2<float32>));
			commandEncoder->draw(vertexFeatureCount, 1, 0, 0);
		}

		if (edgeVertexFeatureCount > 0) {
			commandEncoder->bindRenderPipeline(edgeFeatureRenderPipeline);

			// TODO
			BREAKPOINT();
			// commandEncoder->bindDescriptorSets();

			commandEncoder->bindVertexBuffer(0, edgeVertexFeatureBuffer, 0, sizeof(svec2<float32>));
			commandEncoder->draw(edgeVertexFeatureCount, 1, 0, 0);
		}
	}

	void Physics2DRenderer::renderSelectedInstancePlaceholder(const ref<CommandEncoder>& commandEncoder) const {
		const uint32 instanceCount = scene->getComponentPool<RigidBody2D>().componentCount;

		DEBUG_ASSERT(selectedInstanceIndex < instanceCount);

		uint32 vertexOffset = 0;

		for (Entity entity = 0; entity < selectedInstanceIndex; entity += 1) {
			const ref<Geometry2D>& geometry = scene->getGeometry(entity);

			vertexOffset += geometry->getVertexCount() + 1;
		}

		const ref<Geometry2D>& geometry = scene->getGeometry(selectedInstanceIndex);

		commandEncoder->bindRenderPipeline(placeholderCenterOfMassRenderPipeline);

		// TODO
		BREAKPOINT();
		// commandEncoder->bindDescriptorSets();

		commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, selectedInstanceIndex * sizeof(vec3<float32>), sizeof(svec2<float32>));
		commandEncoder->draw(1, 1, 0, 0);

		commandEncoder->bindRenderPipeline(placeholderFillRenderPipeline);

		// TODO
		BREAKPOINT();
		// commandEncoder->bindDescriptorSets();

		commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
		commandEncoder->bindVertexBuffer(1, instanceBuffer, selectedInstanceIndex * sizeof(Instance), sizeof(Instance));
		commandEncoder->drawIndexed(geometry->getIndexCount(), 1, 0, 0, 0);

		commandEncoder->bindRenderPipeline(placeholderStrokeRenderPipeline);

		// TODO
		BREAKPOINT();
		// commandEncoder->bindDescriptorSets();

		commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
		commandEncoder->draw(geometry->getVertexCount() + 1, 1, 0, 0);
	}

	void Physics2DRenderer::resize(WindowResizeEvent& event) {
		recreateFramebuffers();

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
	}

	void Physics2DRenderer::writeInstanceTransform(uint32 instanceIndex, const mat3x3<float32>& transform) const {
		instanceMappedRange[instanceIndex].world = transform;
	}

	void Physics2DRenderer::registerFeature(const Feature2D& feature) {
		switch (feature.getVertexCount()) {
			case 1:
				vertexFeatureMappedRange[vertexFeatureCount] = {feature.vertex[0], feature.vertex[1]};
				vertexFeatureCount += 1;

				break;
			case 2:
				edgeVertexFeatureMappedRange[edgeVertexFeatureCount + 0] = {feature.edge[0][0], feature.edge[0][1]};
				edgeVertexFeatureMappedRange[edgeVertexFeatureCount + 1] = {feature.edge[1][0], feature.edge[1][1]};
				edgeVertexFeatureCount += 2;

				break;
			default:
				DEBUG_ASSERT(false);

				break;
		}
	}

	void Physics2DRenderer::clearRegisteredData() {
		vertexFeatureCount = 0;
		edgeVertexFeatureCount = 0;
		closestPointMappedRange[0] = {0.0f, 0.0f};
		closestPointMappedRange[1] = {0.0f, 0.0f};
	} */
}