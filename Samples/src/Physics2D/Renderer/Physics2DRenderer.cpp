#include "pch.hpp"

#include "Physics2DRenderer.hpp"

#include "Engine/API/Material/ColorFillStrokeMaterial.hpp"

namespace ProjectA {
	void Physics2DRenderer::build() {
		setupRenderPass();

		recreateMultisampleImage();
		recreateMultisampleImageView();
		recreateDepthStencilImage();
		recreateDepthStencilImageView();
		recreateFramebuffers();

		createCameraBuffer();
		createViewBuffer();

		createCameraViewDescriptorSetLayout();

		allocateDescriptorSets();

		updateCameraViewDescriptorSet();

		createCameraViewPipelineLayout();

		createGridRenderPipeline();
		createFillRenderPipeline();
		createStrokeRenderPipeline();

		/////

		/* createClosestPointBuffer();
		createDistanceRenderPipeline();

		createVertexFeatureBuffer();
		createEdgeVertexFeatureBuffer();

		createFeaturePipelineLayout();

		createVertexFeatureRenderPipeline();
		createEdgeFeatureRenderPipeline(); */

		updateCameraProjection();
		updateViewport();
	}

	void Physics2DRenderer::setupRenderPass() {
		ENGINE_STATIC_ASSERT(SAMPLE_COUNT > 1);

		Attachment swapchainAttachment{};
		swapchainAttachment.format = getSurface()->getFormat();
		swapchainAttachment.sampleCount = SAMPLE_COUNT;
		swapchainAttachment.loadOperation = LoadOperation::CLEAR;
		swapchainAttachment.storeOperation = StoreOperation::STORE;

		// TODO: Use ImageLayout::COLOR_ATTACHMENT_OPTIMAL.
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
	}

	void Physics2DRenderer::recreateMultisampleImage() {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		AttachmentImageDescriptor multisampleAttachmentImageDescriptor{};
		multisampleAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		multisampleAttachmentImageDescriptor.extent.width = swapchainExtent.width;
		multisampleAttachmentImageDescriptor.extent.height = swapchainExtent.height;
		multisampleAttachmentImageDescriptor.extent.depth = 1;
		multisampleAttachmentImageDescriptor.mipLevelCount = 1;
		multisampleAttachmentImageDescriptor.layerCount = 1;

		multisampleImage = renderPass->createColorAttachmentImage(multisampleAttachmentImageDescriptor, 0);
	}

	void Physics2DRenderer::recreateMultisampleImageView() {
		ImageViewDescriptor multisampleImageViewDescriptor{};
		multisampleImageViewDescriptor.mipLevelOffset = 0;
		multisampleImageViewDescriptor.mipLevelCount = multisampleImage->getMipLevelCount();
		multisampleImageViewDescriptor.layerOffset = 0;
		multisampleImageViewDescriptor.layerCount = multisampleImage->getLayerCount();

		multisampleImageView = multisampleImage->createView(multisampleImageViewDescriptor);
	}

	void Physics2DRenderer::recreateDepthStencilImage() {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		AttachmentImageDescriptor depthStencilAttachmentImageDescriptor{};
		depthStencilAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		depthStencilAttachmentImageDescriptor.extent.width = swapchainExtent.width;
		depthStencilAttachmentImageDescriptor.extent.height = swapchainExtent.height;
		depthStencilAttachmentImageDescriptor.extent.depth = 1;
		depthStencilAttachmentImageDescriptor.mipLevelCount = 1;
		depthStencilAttachmentImageDescriptor.layerCount = 1;

		depthStencilImage = renderPass->createDepthStencilAttachmentImage(depthStencilAttachmentImageDescriptor, 0);
	}

	void Physics2DRenderer::recreateDepthStencilImageView() {
		ImageViewDescriptor depthStencilImageViewDescriptor{};
		depthStencilImageViewDescriptor.mipLevelOffset = 0;
		depthStencilImageViewDescriptor.mipLevelCount = depthStencilImage->getMipLevelCount();
		depthStencilImageViewDescriptor.layerOffset = 0;
		depthStencilImageViewDescriptor.layerCount = depthStencilImage->getLayerCount();

		depthStencilImageView = depthStencilImage->createView(depthStencilImageViewDescriptor);
	}

	void Physics2DRenderer::recreateFramebuffers() {
		const ref<Swapchain>& swapchain = getSwapchain();

		for (ref<Framebuffer>& framebuffer : framebuffers) {
			framebuffer.~shared_ptr();
		}

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor{};
		framebufferDescriptor.renderPass = renderPass;
		framebufferDescriptor.imageViews = {multisampleImageView, depthStencilImageView, nullptr};
		framebufferDescriptor.extent = swapchain->getExtent();
		framebufferDescriptor.layerCount = 1;

		for (uint32 framebufferIndex = 0; framebufferIndex < framebuffers.count; framebufferIndex += 1) {
			framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
		}
	}

	void Physics2DRenderer::recreateVertexBuffer(uint64 size) {
		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = size;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexBuffer = createBuffer(vertexBufferDescriptor);
	}

	void Physics2DRenderer::recreateInstanceBuffer(uint64 size) {
		BufferDescriptor instanceBufferDescriptor{};
		instanceBufferDescriptor.size = size;
		instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		instanceBufferDescriptor.persistent = true;

		instanceBuffer = createBuffer(instanceBufferDescriptor);

		instanceBufferMappedRange = instanceBuffer->map<Instance>(0, instanceBuffer->size);
	}

	void Physics2DRenderer::recreateIndexBuffer(uint64 size) {
		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = size;
		indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
		indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		indexBuffer = createBuffer(indexBufferDescriptor);		
	}

	void Physics2DRenderer::copyBuffers(const ref<Buffer>& stagingBuffer) const {
		BufferCopyRegion vertexBufferCopyRegion{};
		vertexBufferCopyRegion.sourceOffset = 0;
		vertexBufferCopyRegion.destinationOffset = 0;
		vertexBufferCopyRegion.size = vertexBuffer->size;

		BufferCopyRegion instanceBufferCopyRegion{};
		instanceBufferCopyRegion.sourceOffset = vertexBuffer->size;
		instanceBufferCopyRegion.destinationOffset = 0;
		instanceBufferCopyRegion.size = instanceBuffer->size;

		BufferCopyRegion indexBufferCopyRegion{};
		indexBufferCopyRegion.sourceOffset = vertexBuffer->size + instanceBuffer->size;
		indexBufferCopyRegion.destinationOffset = 0;
		indexBufferCopyRegion.size = indexBuffer->size;

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, {instanceBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, {indexBufferCopyRegion});

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		getDevice()->waitIdle();
	}

	void Physics2DRenderer::createCameraBuffer() {
		BufferDescriptor cameraBufferDescriptor{};
		cameraBufferDescriptor.size = sizeof(Camera);
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		cameraBufferDescriptor.persistent = true;

		cameraBuffer = createBuffer(cameraBufferDescriptor);

		cameraBufferMappedRange = cameraBuffer->map<Camera>(0, cameraBuffer->size);
		cameraBufferMappedRange->zoomLevel = DEFAULT_ZOOM_LEVEL;
	}

	void Physics2DRenderer::createViewBuffer() {
		BufferDescriptor viewBufferDescriptor{};
		viewBufferDescriptor.size = sizeof(View);
		viewBufferDescriptor.usage = BufferUsage::UNIFORM;
		viewBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		viewBufferDescriptor.persistent = true;

		viewBuffer = createBuffer(viewBufferDescriptor);

		viewBufferMappedRange = viewBuffer->map<View>(0, viewBuffer->size);
	}

	void Physics2DRenderer::createCameraViewDescriptorSetLayout() {
		DescriptorSetLayoutBinding cameraBinding{};
		cameraBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraBinding.descriptorCount = 1;
		cameraBinding.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding viewBinding{};
		viewBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		viewBinding.descriptorCount = 1;
		viewBinding.shaderStages = ShaderStage::FRAGMENT;

		cameraViewDescriptorSetLayout = createDescriptorSetLayout({cameraBinding, viewBinding});
	}

	void Physics2DRenderer::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({cameraViewDescriptorSetLayout});

		cameraViewDescriptorSet = descriptorSets[0];
	}

	void Physics2DRenderer::updateCameraViewDescriptorSet() {
		DescriptorBuffer cameraDescriptorBuffer{};
		cameraDescriptorBuffer.buffer = cameraBuffer;

		// TODO
		// The camera's projection isn't used when rendering the grid.
		// The offset must be a multiple of the minUniformBufferAlignment physical device limit.
		cameraDescriptorBuffer.offset = 0;
		// cameraDescriptorBuffer.offset = sizeof(Camera::projection);

		cameraDescriptorBuffer.size = cameraBuffer->size;

		DescriptorBuffer viewDescriptorBuffer{};
		viewDescriptorBuffer.buffer = viewBuffer;
		viewDescriptorBuffer.offset = 0;
		viewDescriptorBuffer.size = viewBuffer->size;

		WriteDescriptorSet cameraWriteDescriptorSet{};
		cameraWriteDescriptorSet.descriptorSet = cameraViewDescriptorSet;
		cameraWriteDescriptorSet.bindingIndex = 0;
		cameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet viewWriteDescriptorSet{};
		viewWriteDescriptorSet.descriptorSet = cameraViewDescriptorSet;
		viewWriteDescriptorSet.bindingIndex = 1;
		viewWriteDescriptorSet.descriptorBuffers = {viewDescriptorBuffer};

		getDevice()->updateDescriptorSets({cameraWriteDescriptorSet, viewWriteDescriptorSet});
	}

	void Physics2DRenderer::createCameraViewPipelineLayout() {
		PipelineLayoutDescriptor cameraViewPipelineLayoutDescriptor{};
		cameraViewPipelineLayoutDescriptor.descriptorSetLayouts = {cameraViewDescriptorSetLayout};

		cameraViewPipelineLayout = createPipelineLayout(cameraViewPipelineLayoutDescriptor);
	}

	void Physics2DRenderer::createGridRenderPipeline() {
		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("Base/ScreenUVNDC.vert", vertexShaderModuleDescriptor);
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Physics2D/Grid.frag", fragmentShaderModuleDescriptor);

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor gridRenderPipelineDescriptor{};
		gridRenderPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
		gridRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		gridRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
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

	void Physics2DRenderer::createFillRenderPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>();

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>();
		instanceInputBinding.registerAttribute<vec4<float32>>();
		instanceInputBinding.registerAttribute<vec4<float32>>();
		instanceInputBinding.registerAttribute<float32>();

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("Physics2D/Fill.vert", vertexShaderModuleDescriptor);
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Base/FragmentRGBA.frag", fragmentShaderModuleDescriptor);

		ColorBlendAttachment transparentColorBlendAttachment{};
		transparentColorBlendAttachment.blend = true;
		transparentColorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		transparentColorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		RenderPipelineDescriptor fillRenderPipelineDescriptor{};
		fillRenderPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
		 fillRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		fillRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		fillRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
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
	}

	void Physics2DRenderer::createStrokeRenderPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>();

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>();
		instanceInputBinding.registerAttribute<vec4<float32>>();
		instanceInputBinding.registerAttribute<vec4<float32>>();
		instanceInputBinding.registerAttribute<float32>();
		instanceInputBinding.registerAttribute<uint32>();

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("Physics2D/Stroke.vert", vertexShaderModuleDescriptor);
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Physics2D/Stroke.frag", fragmentShaderModuleDescriptor);

		ColorBlendAttachment transparentColorBlendAttachment{};
		transparentColorBlendAttachment.blend = true;
		transparentColorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		transparentColorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		RenderPipelineDescriptor strokeRenderPipelineDescriptor{};
		strokeRenderPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
		strokeRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		strokeRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		strokeRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
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
	}

	/* void Physics2DRenderer::createCenterOfMassBuffer() {
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

	void Physics2DRenderer::createClosestPointBuffer() {
		BufferDescriptor closestPointBufferDescriptor{};
		closestPointBufferDescriptor.size = 2 * sizeof(svec2<float32>);
		closestPointBufferDescriptor.usage = BufferUsage::VERTEX;
		closestPointBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		closestPointBufferDescriptor.persistent = true;

		closestPointBuffer = createBuffer(closestPointBufferDescriptor);

		closestPointBufferMappedRange = closestPointBuffer->map<svec2<float32>>(0, closestPointBuffer->size);
	}

	void Physics2DRenderer::createDistanceRenderPipeline() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.shaderStages = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

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

	void Physics2DRenderer::createCenterOfMassRenderPipeline() {
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

	void Physics2DRenderer::createVertexFeatureBuffer() {
		constexpr uint64 vertexFeatureBufferSize = 2 * sizeof(svec2<float32>);

		BufferDescriptor vertexFeatureBufferDescriptor;
		vertexFeatureBufferDescriptor.size = vertexFeatureBufferSize;
		vertexFeatureBufferDescriptor.usage = BufferUsage::VERTEX;
		vertexFeatureBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		vertexFeatureBufferDescriptor.persistent = true;

		vertexFeatureBuffer = createBuffer(vertexFeatureBufferDescriptor);
		vertexFeatureMappedRange = vertexFeatureBuffer->map<svec2<float32>>(0, vertexFeatureBufferSize);
	}

	void Physics2DRenderer::createEdgeVertexFeatureBuffer() {
		constexpr uint64 edgeVertexFeatureBufferSize = 4 * sizeof(svec2<float32>);

		BufferDescriptor edgeVertexFeatureBufferDescriptor;
		edgeVertexFeatureBufferDescriptor.size = edgeVertexFeatureBufferSize;
		edgeVertexFeatureBufferDescriptor.usage = BufferUsage::VERTEX;
		edgeVertexFeatureBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		edgeVertexFeatureBufferDescriptor.persistent = true;

		edgeVertexFeatureBuffer = createBuffer(edgeVertexFeatureBufferDescriptor);
		edgeVertexFeatureMappedRange = edgeVertexFeatureBuffer->map<svec2<float32>>(0, edgeVertexFeatureBufferSize);
	}

	void Physics2DRenderer::createFeaturePipelineLayout() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.shaderStages = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

		DescriptorSetLayout featureDescriptorSetLayout;
		featureDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor featurePipelineLayoutDescriptor;
		featurePipelineLayoutDescriptor.layouts = {featureDescriptorSetLayout};

		featurePipelineLayout = createPipelineLayout(featurePipelineLayoutDescriptor);
	}

	void Physics2DRenderer::createVertexFeatureRenderPipeline() {
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

	void Physics2DRenderer::createEdgeFeatureRenderPipeline() {
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
	} */

	void Physics2DRenderer::setScene(const Scene2D& scene) {
		this->scene = &scene;

		const ComponentPool<RigidBody2D>& rigidBodyPool = scene.getComponentPool<RigidBody2D>();
		const uint32 instanceCount = rigidBodyPool.componentCount;

		uint32 vertexCount = 0;

		for (uint32 instanceIndex = 0; instanceIndex < instanceCount; instanceIndex += 1) {
			const Entity entity = rigidBodyPool.componentIndexToEntity[instanceIndex];
			const ref<Geometry2D>& geometry = scene.getGeometry(entity);

			vertexCount += geometry->getVertexCount() + 1;
			indexCount += geometry->getIndexCount();
		}

		const uint64 vertexBufferSize = vertexCount * sizeof(svec2<float32>);
		const uint64 instanceBufferSize = instanceCount * sizeof(Instance);
		const uint64 indexBufferSize = indexCount * sizeof(uint16);

		const ref<Buffer> stagingBuffer = createStagingBuffer(vertexBufferSize + indexBufferSize + instanceBufferSize);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->size);
		svec2<float32>* vertexMappedRange = pointer_cast<svec2<float32>*>(mappedRange);
		Instance* instanceMappedRange = pointer_cast<Instance*>(mappedRange + vertexBufferSize);
		uint16* indexMappedRange = pointer_cast<uint16*>(mappedRange + vertexBufferSize + instanceBufferSize);

		const float32 inverseInstanceCount = 1.0f / instanceCount;

		for (uint32 instanceIndex = 0; instanceIndex < instanceCount; instanceIndex += 1) {
			const Entity entity = rigidBodyPool.componentIndexToEntity[instanceIndex];
			const ref<Geometry2D>& geometry = scene.getGeometry(entity);
			const ref<Material>& material = scene.getMaterial(entity);

			for (uint32 vertexIndex = 0; vertexIndex < geometry->vertices.count; vertexIndex += 1) {
				const vec2<float32>& vertex = geometry->vertices[vertexIndex];

				vertexMappedRange[vertexIndex] = {vertex[0], vertex[1]};
			}

			// Duplicate the first vertex for line rendering.
			vertexMappedRange[geometry->getVertexCount()] = {geometry->vertices[0][0], geometry->vertices[0][1]};
			vertexMappedRange += geometry->getVertexCount() + 1;

			instanceMappedRange[instanceIndex].fillColor = std::static_pointer_cast<ColorFillStrokeMaterial>(material)->fillColor;
			instanceMappedRange[instanceIndex].strokeColor = std::static_pointer_cast<ColorFillStrokeMaterial>(material)->strokeColor;
			instanceMappedRange[instanceIndex].depth = entity * inverseInstanceCount;

			std::copy(geometry->indices.begin(), geometry->indices.end(), indexMappedRange);

			indexMappedRange += geometry->getIndexCount();
		}

		stagingBuffer->unmap();

		recreateVertexBuffer(vertexBufferSize);
		recreateInstanceBuffer(instanceBufferSize);
		recreateIndexBuffer(indexBufferSize);

		copyBuffers(stagingBuffer);

		/* DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding;
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.shaderStages = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

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
		fillRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("Base/FragmentRGBA.frag", fragmentShaderModuleDescriptor);
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

		constexpr uint64 placeholderBufferSize = sizeof(Placeholder);

		BufferDescriptor placeholderBufferDescriptor;
		placeholderBufferDescriptor.size = placeholderBufferSize;
		placeholderBufferDescriptor.usage = BufferUsage::UNIFORM;
		placeholderBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		placeholderBufferDescriptor.persistent = true;

		placeholderBuffer = createBuffer(placeholderBufferDescriptor);

		placeholderBufferMappedRange = placeholderBuffer->map<Placeholder>(0, placeholderBufferSize);
		placeholderBufferMappedRange->offset = {0.0f, 0.0f};

		DescriptorSetLayoutBinding placeholderDescriptorSetLayoutBinding;
		placeholderDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		placeholderDescriptorSetLayoutBinding.shaderStages = ShaderStage::VERTEX;
		placeholderDescriptorSetLayoutBinding.buffers = {placeholderBuffer};

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
		initializeCenterOfMassRenderPipeline(); */
	}

	void Physics2DRenderer::setInstanceTransform(uint32 instanceIndex, const mat3x3<float32>& transform) const {
		instanceBufferMappedRange[instanceIndex].world = transform;
	}

	/* const vec2<float32>& Physics2DRenderer::getNdcMousePosition() const {
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

	void Physics2DRenderer::setClosestPoints(const vec2<float32>& closestPointA, const vec2<float32>& closestPointB) const {
		closestPointMappedRange[0] = {closestPointA[0], closestPointA[1]};
		closestPointMappedRange[1] = {closestPointB[0], closestPointB[1]};
	} */

	void Physics2DRenderer::updateCameraProjection() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 left = swapchainExtent.width * -0.5f;
		const float32 right = swapchainExtent.width * 0.5f;
		const float32 top = swapchainExtent.height * 0.5f;
		const float32 bottom = swapchainExtent.height * -0.5f;

		cameraBufferMappedRange->projection = {
			2.0f / (right - left),  0.0f,                  -(right + left) / (right - left),
			0.0f,                   2.0f / (top - bottom), -(top + bottom) / (top - bottom),
			0.0f,                   0.0f,                   1.0f,
		};
	}

	void Physics2DRenderer::updateViewport() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		viewBufferMappedRange->viewport.x = static_cast<float32>(swapchainExtent.width);
		viewBufferMappedRange->viewport.y = static_cast<float32>(swapchainExtent.height);
	}

	void Physics2DRenderer::zoom(float32 zoomLevelDelta) {
		float32 direction = zoomLevelDelta;

		if (direction != 0.0f) {
			direction /= abs(zoomLevelDelta);
		}

		cameraBufferMappedRange->zoomLevel = clamp(cameraBufferMappedRange->zoomLevel + direction, MIN_ZOOM_LEVEL, MAX_ZOOM_LEVEL);

		// setMousePosition(mousePosition[0], mousePosition[1]);
	}

	/* void Physics2DRenderer::updateCentersOfMass() const {
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
			instanceBufferMappedRange[selectedInstanceIndex].selected = 0;
		}

		if (entity == max<uint32>()) {
			selectedInstanceIndex = entity;

			return;
		}

		const ComponentPool<RigidBody2D>& instancePool = scene->getComponentPool<RigidBody2D>();

		selectedInstanceIndex = instancePool.entityToComponentIndex[entity];
		instanceBufferMappedRange[selectedInstanceIndex].selected = 1;
	}

	void Physics2DRenderer::setPlaceholderOffset(const vec2<float32>& offset) const {
		placeholderBufferMappedRange->offset = {offset[0], offset[1]};
	} */

	void Physics2DRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		// updateCentersOfMass();

		ClearValue colorClearValue = {0.0f, 0.0f, 0.0f, 1.0f};

		ClearValue depthStencilClearValue{};
		depthStencilClearValue.depth = 0.0f;
		depthStencilClearValue.stencil = 0;

		BeginRenderPassDescriptor beginRenderPassDescriptor{};
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
		/* renderDistance(commandEncoder);
		renderFeatures(commandEncoder);

		if (selectedInstanceIndex != max<uint32>()) {
			renderSelectedInstancePlaceholder(commandEncoder);
		} */

		commandEncoder->endRenderPass();
	}

	void Physics2DRenderer::renderGrid(const ref<CommandEncoder>& commandEncoder) const {
		commandEncoder->bindRenderPipeline(gridRenderPipeline);
		commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, {cameraViewDescriptorSet});
		commandEncoder->draw(3, 1, 0, 0);
	}

	void Physics2DRenderer::renderScene(const ref<CommandEncoder>& commandEncoder) const {
		commandEncoder->bindRenderPipeline(fillRenderPipeline);
		commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, {cameraViewDescriptorSet});
		commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);

		const ComponentPool<RigidBody2D>& rigidBodyPool = scene->getComponentPool<RigidBody2D>();
		const uint32 instanceCount = rigidBodyPool.componentCount;

		uint32 instanceIndex = 0;
		uint32 vertexOffset = 0;
		uint32 indexOffset = 0;

		while (instanceIndex < instanceCount) {
			const ref<Geometry2D>& geometry = rigidBodyPool.components[instanceIndex].geometry;
			const uint32 indexCount = geometry->getIndexCount();
			const uint32 vertexCount = geometry->getVertexCount() + 1;

			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance), sizeof(Instance));
			commandEncoder->drawIndexed(indexCount, 1, indexOffset, 0, 0);

			instanceIndex += 1;
			vertexOffset += vertexCount;
			indexOffset += indexCount;
		}

		commandEncoder->bindRenderPipeline(strokeRenderPipeline);
		commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, {cameraViewDescriptorSet});

		instanceIndex = 0;
		vertexOffset = 0;

		while (instanceIndex < instanceCount) {
			const ref<Geometry2D>& geometry = rigidBodyPool.components[instanceIndex].geometry;
			const uint32 vertexCount = geometry->getVertexCount() + 1;

			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(svec2<float32>), sizeof(svec2<float32>));
			commandEncoder->bindVertexBuffer(1, instanceBuffer, instanceIndex * sizeof(Instance), sizeof(Instance));

			if (instanceIndex == selectedInstanceIndex) {
				commandEncoder->setLineWidth(2.0f);
			}
			else {
				commandEncoder->setLineWidth(1.0f);
			}

			commandEncoder->draw(vertexCount, 1, 0, 0);

			instanceIndex += 1;
			vertexOffset += vertexCount;
		}

		/* commandEncoder->bindRenderPipeline(centerOfMassRenderPipeline);
		// TODO
		BREAKPOINT();
		// commandEncoder->bindDescriptorSets();
		commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, 0, sizeof(vec3<float32>));
		commandEncoder->draw(rigidBodyPool.componentCount, 1, 0, 0); */
	}

	/* void Physics2DRenderer::renderDistance(const ref<CommandEncoder>& commandEncoder) const {
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
	} */

	void Physics2DRenderer::resize(WindowResizeEvent& event) {
		recreateMultisampleImage();
		recreateMultisampleImageView();
		recreateDepthStencilImage();
		recreateDepthStencilImageView();
		recreateFramebuffers();

		updateCameraProjection();
		updateViewport();
	}

	/* void Physics2DRenderer::registerFeature(const Feature2D& feature) {
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