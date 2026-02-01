// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2DRenderer.hpp"

#include "Engine/API/Material/ColorFillStrokeMaterial.hpp"

namespace Trident {
	void Physics2DRenderer::build() {
		setupRenderPass();

		if (layerInput->swapchainImage == nullptr) {
			recreateMultisampleImage();
			recreateMultisampleImageView();
		}

		recreateDepthStencilImage();
		recreateDepthStencilImageView();
		recreateFramebuffers();

		createCameraBuffer();
		createViewBuffer();
		createPlaceholderBuffer();

		#ifdef SHOW_DISTANCE
			createClosestPointBuffer();
		#endif

		#ifdef SHOW_FEATURES
			createVertexFeatureBuffer();
			createEdgeFeatureBuffer();
		#endif

		createCameraViewDescriptorSetLayout();
		createPlaceholderDescriptorSetLayout();

		allocateDescriptorSets();

		updateCameraViewDescriptorSet();
		updatePlaceholderDescriptorSet();

		createCameraViewPipelineLayout();
		createPlaceholderPipelineLayout();

		createGridRenderPipeline();
		createFillRenderPipeline();
		createStrokeRenderPipeline();
		createCenterOfMassRenderPipeline();
		createPlaceholderFillRenderPipeline();
		createPlaceholderStrokeRenderPipeline();
		createPlaceholderCenterOfMassRenderPipeline();

		#ifdef SHOW_DISTANCE
			createDistanceRenderPipeline();
		#endif

		#ifdef SHOW_FEATURES
			createVertexFeatureRenderPipeline();
			createEdgeFeatureRenderPipeline();
		#endif

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

		// TODO(layer-superposition)
		swapchainAttachment.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
		// swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

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

		SET_DEBUG_NAME(multisampleAttachmentImageDescriptor, "Multisample attachment");

		layerInput->swapchainImage = renderPass->createColorAttachmentImage(multisampleAttachmentImageDescriptor, 0);
	}

	void Physics2DRenderer::recreateMultisampleImageView() {
		ImageViewDescriptor multisampleImageViewDescriptor{};
		multisampleImageViewDescriptor.mipLevelOffset = 0;
		multisampleImageViewDescriptor.mipLevelCount = 1;
		multisampleImageViewDescriptor.layerOffset = 0;
		multisampleImageViewDescriptor.layerCount = 1;

		SET_DEBUG_NAME(multisampleImageViewDescriptor, "Multisample attachment");

		layerInput->swapchainImageView = layerInput->swapchainImage->createView(multisampleImageViewDescriptor);
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

		SET_DEBUG_NAME(depthStencilAttachmentImageDescriptor, "Depth stencil attachment");

		depthStencilImage = renderPass->createDepthStencilAttachmentImage(depthStencilAttachmentImageDescriptor, 0);
	}

	void Physics2DRenderer::recreateDepthStencilImageView() {
		ImageViewDescriptor depthStencilImageViewDescriptor{};
		depthStencilImageViewDescriptor.mipLevelOffset = 0;
		depthStencilImageViewDescriptor.mipLevelCount = 1;
		depthStencilImageViewDescriptor.layerOffset = 0;
		depthStencilImageViewDescriptor.layerCount = 1;

		SET_DEBUG_NAME(depthStencilImageViewDescriptor, "Depth stencil attachment");

		depthStencilImageView = depthStencilImage->createView(depthStencilImageViewDescriptor);
	}

	void Physics2DRenderer::recreateFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor{};
		framebufferDescriptor.renderPass = renderPass;
		framebufferDescriptor.imageViews = {layerInput->swapchainImageView, depthStencilImageView, nullptr};
		framebufferDescriptor.extent = swapchain->getExtent();

		uint32 framebufferIndex = 0;

		while (framebufferIndex < framebuffers.count()) {
			SET_DEBUG_NAME(framebufferDescriptor, Logger::format("In flight framebuffer []", framebufferIndex));

			framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

			framebufferIndex += 1;
		}
	}

	void Physics2DRenderer::recreateVertexBuffer(uint64 size) {
		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = size;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(vertexBufferDescriptor, "Vertex");

		vertexBuffer = createBuffer(vertexBufferDescriptor);
	}

	void Physics2DRenderer::recreateInstanceBuffer(uint64 size) {
		BufferDescriptor instanceBufferDescriptor{};
		instanceBufferDescriptor.size = size;
		instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		instanceBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(instanceBufferDescriptor, "Instance");

		instanceBuffer = createBuffer(instanceBufferDescriptor);

		instanceBufferMappedRange = instanceBuffer->map<Instance>(0, instanceBuffer->size);
	}

	void Physics2DRenderer::recreateCenterOfMassBuffer(uint64 size) {
		BufferDescriptor centerOfMassBufferDescriptor{};
		centerOfMassBufferDescriptor.size = size;
		centerOfMassBufferDescriptor.usage = BufferUsage::VERTEX;
		centerOfMassBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		centerOfMassBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(centerOfMassBufferDescriptor, "Center of mass");

		centerOfMassBuffer = createBuffer(centerOfMassBufferDescriptor);

		centerOfMassBufferMappedRange = centerOfMassBuffer->map<CenterOfMass>(0, centerOfMassBuffer->size);
	}

	void Physics2DRenderer::recreateIndexBuffer(uint64 size) {
		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = size;
		indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
		indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(indexBufferDescriptor, "Index");

		indexBuffer = createBuffer(indexBufferDescriptor);		
	}

	void Physics2DRenderer::createCameraBuffer() {
		BufferDescriptor cameraBufferDescriptor{};
		cameraBufferDescriptor.size = sizeof(Camera);
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		cameraBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(cameraBufferDescriptor, "Camera");

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

		SET_DEBUG_NAME(viewBufferDescriptor, "View");

		viewBuffer = createBuffer(viewBufferDescriptor);

		viewBufferMappedRange = viewBuffer->map<View>(0, viewBuffer->size);
	}

	void Physics2DRenderer::createPlaceholderBuffer() {
		BufferDescriptor placeholderBufferDescriptor{};
		placeholderBufferDescriptor.size = sizeof(Placeholder);
		placeholderBufferDescriptor.usage = BufferUsage::UNIFORM;
		placeholderBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		placeholderBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(placeholderBufferDescriptor, "Placeholder");

		placeholderBuffer = createBuffer(placeholderBufferDescriptor);

		placeholderBufferMappedRange = placeholderBuffer->map<Placeholder>(0, placeholderBuffer->size);
		placeholderBufferMappedRange->offset = {0.0f, 0.0f};
	}

	#ifdef SHOW_DISTANCE
		void Physics2DRenderer::createClosestPointBuffer() {
			BufferDescriptor closestPointBufferDescriptor{};
			closestPointBufferDescriptor.size = 2 * sizeof(Vertex);
			closestPointBufferDescriptor.usage = BufferUsage::VERTEX;
			closestPointBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
			closestPointBufferDescriptor.persistent = true;

			SET_DEBUG_NAME(closestPointBufferDescriptor, "Closest point");

			closestPointBuffer = createBuffer(closestPointBufferDescriptor);

			closestPointBufferMappedRange = closestPointBuffer->map<Vertex>(0, closestPointBuffer->size);
		}
	#endif

	#ifdef SHOW_FEATURES
		void Physics2DRenderer::createVertexFeatureBuffer() {
			BufferDescriptor vertexFeatureBufferDescriptor{};
			vertexFeatureBufferDescriptor.size = 2 * sizeof(Vertex);
			vertexFeatureBufferDescriptor.usage = BufferUsage::VERTEX;
			vertexFeatureBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
			vertexFeatureBufferDescriptor.persistent = true;

			SET_DEBUG_NAME(vertexFeatureBufferDescriptor, "Vertex feature");

			vertexFeatureBuffer = createBuffer(vertexFeatureBufferDescriptor);

			vertexFeatureBufferMappedRange = vertexFeatureBuffer->map<Vertex>(0, vertexFeatureBuffer->size);
		}

		void Physics2DRenderer::createEdgeFeatureBuffer() {
			BufferDescriptor edgeFeatureBufferDescriptor{};
			edgeFeatureBufferDescriptor.size = 4 * sizeof(Vertex);
			edgeFeatureBufferDescriptor.usage = BufferUsage::VERTEX;
			edgeFeatureBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
			edgeFeatureBufferDescriptor.persistent = true;

			SET_DEBUG_NAME(edgeFeatureBufferDescriptor, "Edge feature");

			edgeFeatureBuffer = createBuffer(edgeFeatureBufferDescriptor);

			edgeFeatureBufferMappedRange = edgeFeatureBuffer->map<Vertex>(0, edgeFeatureBuffer->size);
		}
	#endif

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

	void Physics2DRenderer::createPlaceholderDescriptorSetLayout() {
		DescriptorSetLayoutBinding placeholderBinding{};
		placeholderBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		placeholderBinding.descriptorCount = 1;
		placeholderBinding.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT;

		placeholderDescriptorSetLayout = createDescriptorSetLayout({placeholderBinding});
	}

	void Physics2DRenderer::createCameraViewPipelineLayout() {
		PipelineLayoutDescriptor cameraViewPipelineLayoutDescriptor{};
		cameraViewPipelineLayoutDescriptor.descriptorSetLayouts = {cameraViewDescriptorSetLayout};

		SET_DEBUG_NAME(cameraViewPipelineLayoutDescriptor, "Camera view");

		cameraViewPipelineLayout = createPipelineLayout(cameraViewPipelineLayoutDescriptor);
	}

	void Physics2DRenderer::createPlaceholderPipelineLayout() {
		PipelineLayoutDescriptor placeholderPipelineLayoutDescriptor{};
		placeholderPipelineLayoutDescriptor.descriptorSetLayouts = {cameraViewDescriptorSetLayout, placeholderDescriptorSetLayout};

		SET_DEBUG_NAME(placeholderPipelineLayoutDescriptor, "Placeholder");

		placeholderPipelineLayout = createPipelineLayout(placeholderPipelineLayoutDescriptor);
	}

	void Physics2DRenderer::createGridRenderPipeline() {
		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Grid vertex");

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Grid fragment");

		const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Base/ScreenUVNDC.vert");
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/Grid.frag");

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

		SET_DEBUG_NAME(gridRenderPipelineDescriptor, "Grid");

		gridRenderPipeline = createRenderPipeline(gridRenderPipelineDescriptor);
	}

	void Physics2DRenderer::createFillRenderPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>("");

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>("");
		instanceInputBinding.registerAttribute<vec4<float32>>("");
		instanceInputBinding.registerAttribute<vec4<float32>>("");
		instanceInputBinding.registerAttribute<float32>("");

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Fill vertex");

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Fill fragment");

		const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/Fill.vert");
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Base/FragmentRGBA.frag");

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = true;
		colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

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
		fillRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		fillRenderPipelineDescriptor.depthStencilState.depthTest = true;
		fillRenderPipelineDescriptor.depthStencilState.depthWrite = true;
		fillRenderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		fillRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		fillRenderPipelineDescriptor.renderPass = renderPass;
		fillRenderPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(fillRenderPipelineDescriptor, "Fill");

		fillRenderPipeline = createRenderPipeline(fillRenderPipelineDescriptor);
	}

	void Physics2DRenderer::createStrokeRenderPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>("");

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>("");
		instanceInputBinding.registerAttribute<vec4<float32>>("");
		instanceInputBinding.registerAttribute<vec4<float32>>("");
		instanceInputBinding.registerAttribute<float32>("");
		instanceInputBinding.registerAttribute<uint32>("");

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Stroke vertex");

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Stroke fragment");

		const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/Stroke.vert");
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/Stroke.frag");

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = true;
		colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

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
		strokeRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		strokeRenderPipelineDescriptor.depthStencilState.depthTest = true;
		strokeRenderPipelineDescriptor.depthStencilState.depthWrite = true;
		strokeRenderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		strokeRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR | DynamicState::LINE_WIDTH;
		strokeRenderPipelineDescriptor.renderPass = renderPass;
		strokeRenderPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(strokeRenderPipelineDescriptor, "Stroke");

		strokeRenderPipeline = createRenderPipeline(strokeRenderPipelineDescriptor);
	}

	void Physics2DRenderer::createCenterOfMassRenderPipeline() {
		VertexInputBinding centerOfMassVertexInputBinding(InputRate::VERTEX);
		centerOfMassVertexInputBinding.registerAttribute<vec3<float32>>("");

		ShaderModuleDescriptor centerOfMassVertexShaderModuleDescriptor{};
		centerOfMassVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		centerOfMassVertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(centerOfMassVertexShaderModuleDescriptor, "Center of mass vertex");

		ShaderModuleDescriptor centerOfMassFragmentShaderModuleDescriptor{};
		centerOfMassFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		centerOfMassFragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(centerOfMassFragmentShaderModuleDescriptor, "Center of mass fragment");

		const ref<ShaderModule> vertexShaderModule = loadShaderModule(centerOfMassVertexShaderModuleDescriptor, "Physics2D/CenterOfMass.vert");
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(centerOfMassFragmentShaderModuleDescriptor, "Physics2D/CenterOfMass.frag");

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor centerOfMassRenderPipelineDescriptor;
		centerOfMassRenderPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
		centerOfMassRenderPipelineDescriptor.vertexInputBindings = {centerOfMassVertexInputBinding};
		centerOfMassRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		centerOfMassRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
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

		SET_DEBUG_NAME(centerOfMassRenderPipelineDescriptor, "Center of mass");

		centerOfMassRenderPipeline = createRenderPipeline(centerOfMassRenderPipelineDescriptor);
	}

	void Physics2DRenderer::createPlaceholderFillRenderPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>("");

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>("");
		instanceInputBinding.registerAttribute<vec4<float32>>("");

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Placeholder fill vertex");

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Placeholder fill fragment");

		ref<ShaderModule> vertexShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::CPU: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
			case GraphicsAPI::DX12: {
				ENGINE_ERROR_UNIMPLEMENTED();

				break;
			}
			case GraphicsAPI::GL: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderFillSingleSet.vert");

				break;
			}
			case GraphicsAPI::VK: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderFill.vert");

				break;
			}
		}

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Base/FragmentRGBA.frag");

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = true;
		colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		RenderPipelineDescriptor placeholderFillRenderPipelineDescriptor{};
		placeholderFillRenderPipelineDescriptor.pipelineLayout = placeholderPipelineLayout;
		placeholderFillRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		placeholderFillRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		placeholderFillRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		placeholderFillRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		placeholderFillRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		placeholderFillRenderPipelineDescriptor.cullMode = CullMode::BACK;
		placeholderFillRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		placeholderFillRenderPipelineDescriptor.lineWidth = 1.0f;
		placeholderFillRenderPipelineDescriptor.minSampleShading = 0.0f;
		placeholderFillRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		placeholderFillRenderPipelineDescriptor.depthStencilState.depthTest = false;
		placeholderFillRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		placeholderFillRenderPipelineDescriptor.renderPass = renderPass;
		placeholderFillRenderPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(placeholderFillRenderPipelineDescriptor, "Placeholder fill");

		placeholderFillRenderPipeline = createRenderPipeline(placeholderFillRenderPipelineDescriptor);
	}

	void Physics2DRenderer::createPlaceholderStrokeRenderPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>("");

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>("");

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Placeholder stroke vertex");

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Placeholder stroke fragment");

		ref<ShaderModule> vertexShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::CPU: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
			case GraphicsAPI::DX12: {
				ENGINE_ERROR_UNIMPLEMENTED();

				break;
			}
			case GraphicsAPI::GL: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderStrokeSingleSet.vert");

				break;
			}
			case GraphicsAPI::VK: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderStroke.vert");

				break;
			}
		}

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/PlaceholderStroke.frag");

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = true;
		colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		RenderPipelineDescriptor placeholderStrokeRenderPipelineDescriptor{};
		placeholderStrokeRenderPipelineDescriptor.pipelineLayout = placeholderPipelineLayout;
		placeholderStrokeRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		placeholderStrokeRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		placeholderStrokeRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		placeholderStrokeRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_STRIP;
		placeholderStrokeRenderPipelineDescriptor.polygonMode = PolygonMode::LINE;
		placeholderStrokeRenderPipelineDescriptor.cullMode = CullMode::BACK;
		placeholderStrokeRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		placeholderStrokeRenderPipelineDescriptor.lineWidth = 1.0f;
		placeholderStrokeRenderPipelineDescriptor.minSampleShading = 0.0f;
		placeholderStrokeRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		placeholderStrokeRenderPipelineDescriptor.depthStencilState.depthTest = false;
		placeholderStrokeRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		placeholderStrokeRenderPipelineDescriptor.renderPass = renderPass;
		placeholderStrokeRenderPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(placeholderStrokeRenderPipelineDescriptor, "Placeholder stroke");

		placeholderStrokeRenderPipeline = createRenderPipeline(placeholderStrokeRenderPipelineDescriptor);
	}

	void Physics2DRenderer::createPlaceholderCenterOfMassRenderPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>("");

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Placeholder center of mass vertex");

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Placeholder center of mass fragment");

		ref<ShaderModule> vertexShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::CPU: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
			case GraphicsAPI::DX12: {
				ENGINE_ERROR_UNIMPLEMENTED();

				break;
			}
			case GraphicsAPI::GL: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderCenterOfMassSingleSet.vert");

				break;
			}
			case GraphicsAPI::VK: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderCenterOfMass.vert");

				break;
			}
		}

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/CenterOfMass.frag");

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor placeholderCenterOfMassRenderPipelineDescriptor{};
		placeholderCenterOfMassRenderPipelineDescriptor.pipelineLayout = placeholderPipelineLayout;
		placeholderCenterOfMassRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
		placeholderCenterOfMassRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		placeholderCenterOfMassRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		placeholderCenterOfMassRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
		placeholderCenterOfMassRenderPipelineDescriptor.polygonMode = PolygonMode::POINT;
		placeholderCenterOfMassRenderPipelineDescriptor.cullMode = CullMode::BACK;
		placeholderCenterOfMassRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		placeholderCenterOfMassRenderPipelineDescriptor.lineWidth = 1.0f;
		placeholderCenterOfMassRenderPipelineDescriptor.minSampleShading = 1.0f;
		placeholderCenterOfMassRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		placeholderCenterOfMassRenderPipelineDescriptor.depthStencilState.depthTest = false;
		placeholderCenterOfMassRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		placeholderCenterOfMassRenderPipelineDescriptor.renderPass = renderPass;
		placeholderCenterOfMassRenderPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(placeholderCenterOfMassRenderPipelineDescriptor, "Placeholder center of mass");

		placeholderCenterOfMassRenderPipeline = createRenderPipeline(placeholderCenterOfMassRenderPipelineDescriptor);
	}

	#ifdef SHOW_DISTANCE
		void Physics2DRenderer::createDistanceRenderPipeline() {
			VertexInputBinding vertexInputBinding(InputRate::VERTEX);
			vertexInputBinding.registerAttribute<svec2<float32>>();

			ShaderModuleDescriptor vertexShaderModuleDescriptor{};
			vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
			vertexShaderModuleDescriptor.entryPoint = "main";

			SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Distance vertex");

			ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
			fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
			fragmentShaderModuleDescriptor.entryPoint = "main";

			SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Distance fragment");

			const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/Distance.vert");
			const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/Distance.frag");

			ColorBlendAttachment colorBlendAttachment{};
			colorBlendAttachment.blend = false;

			RenderPipelineDescriptor distanceRenderPipelineDescriptor{};
			distanceRenderPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
			distanceRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
			distanceRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
			distanceRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
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

			SET_DEBUG_NAME(distanceRenderPipelineDescriptor, "Distance");

			distanceRenderPipeline = createRenderPipeline(distanceRenderPipelineDescriptor);
		}
	#endif

	#ifdef SHOW_FEATURES
		void Physics2DRenderer::createVertexFeatureRenderPipeline() {
			VertexInputBinding vertexInputBinding(InputRate::VERTEX);
			vertexInputBinding.registerAttribute<svec2<float32>>();

			ShaderModuleDescriptor vertexShaderModuleDescriptor{};
			vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
			vertexShaderModuleDescriptor.entryPoint = "main";

			SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Vertex feature vertex");

			ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
			fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
			fragmentShaderModuleDescriptor.entryPoint = "main";

			SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Vertex feature fragment");

			const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/VertexFeature.vert");
			const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/VertexFeature.frag");

			ColorBlendAttachment colorBlendAttachment{};
			colorBlendAttachment.blend = false;

			RenderPipelineDescriptor vertexFeatureRenderPipelineDescriptor;
			vertexFeatureRenderPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
			vertexFeatureRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
			vertexFeatureRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
			vertexFeatureRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
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

			SET_DEBUG_NAME(vertexFeatureRenderPipelineDescriptor, "Vertex feature");

			vertexFeatureRenderPipeline = createRenderPipeline(vertexFeatureRenderPipelineDescriptor);
		}

		void Physics2DRenderer::createEdgeFeatureRenderPipeline() {
			VertexInputBinding vertexInputBinding(InputRate::VERTEX);
			vertexInputBinding.registerAttribute<svec2<float32>>();

			ShaderModuleDescriptor vertexShaderModuleDescriptor{};
			vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
			vertexShaderModuleDescriptor.entryPoint = "main";

			SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Edge feature vertex");

			ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
			fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
			fragmentShaderModuleDescriptor.entryPoint = "main";

			SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Edge feature fragment");

			const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/EdgeFeature.vert");
			const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/EdgeFeature.frag");

			ColorBlendAttachment colorBlendAttachment{};
			colorBlendAttachment.blend = false;

			RenderPipelineDescriptor edgeFeatureRenderPipelineDescriptor;
			edgeFeatureRenderPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
			edgeFeatureRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
			edgeFeatureRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
			edgeFeatureRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
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

			SET_DEBUG_NAME(edgeFeatureRenderPipelineDescriptor, "Edge feature");

			edgeFeatureRenderPipeline = createRenderPipeline(edgeFeatureRenderPipelineDescriptor);
		}
	#endif

	void Physics2DRenderer::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({cameraViewDescriptorSetLayout, placeholderDescriptorSetLayout});

		cameraViewDescriptorSet = descriptorSets[0];
		placeholderDescriptorSet = descriptorSets[1];
	}

	void Physics2DRenderer::copyBuffers(ref<Buffer> stagingBuffer) const {
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

		SET_DEBUG_NAME(commandBufferDescriptor, "Buffer copy command buffer");

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, {instanceBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, {indexBufferCopyRegion});

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
		transferQueue->waitIdle();
	}

	const vec2<float32>& Physics2DRenderer::getNdcMousePosition() const {
		return ndcMousePosition;
	}

	void Physics2DRenderer::setScene(const Scene2D& scene) {
		this->scene = &scene;

		const ComponentPool<RigidBody2D>& rigidBodyPool = scene.getComponentPool<RigidBody2D>();
		const uint32 instanceCount = rigidBodyPool.componentCount;

		uint32 vertexCount = 0;
		uint32 indexCount = 0;

		for (uint32 instanceIndex = 0; instanceIndex < instanceCount; instanceIndex += 1) {
			const Entity entity = rigidBodyPool.componentIndexToEntity[instanceIndex];
			const ref<Geometry2D> geometry = scene.getGeometry(entity);

			vertexCount += geometry->getVertexCount() + 1;
			indexCount += geometry->getIndexCount();
		}

		const uint64 vertexBufferSize = vertexCount * sizeof(Vertex);
		const uint64 instanceBufferSize = instanceCount * sizeof(Instance);
		const uint64 indexBufferSize = indexCount * sizeof(uint16);

		const ref<Buffer> stagingBuffer = createStagingBuffer(vertexBufferSize + indexBufferSize + instanceBufferSize);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->size);
		Vertex* vertexMappedRange = pointer_cast<Vertex*>(mappedRange);
		Instance* instanceMappedRange = pointer_cast<Instance*>(mappedRange + vertexBufferSize);
		uint16* indexMappedRange = pointer_cast<uint16*>(mappedRange + vertexBufferSize + instanceBufferSize);

		const float32 inverseInstanceCount = 1.0f / instanceCount;

		for (uint32 instanceIndex = 0; instanceIndex < instanceCount; instanceIndex += 1) {
			const Entity entity = rigidBodyPool.componentIndexToEntity[instanceIndex];
			const ref<Geometry2D> geometry = scene.getGeometry(entity);
			const ref<PhysicsMaterial> material = scene.getMaterial(entity);

			for (uint32 vertexIndex = 0; vertexIndex < geometry->vertices.count(); vertexIndex += 1) {
				const vec2<float32>& vertex = geometry->vertices[vertexIndex];

				vertexMappedRange[vertexIndex].position = {vertex[0], vertex[1]};
			}

			// Duplicate the first vertex for line rendering.
			vertexMappedRange[geometry->getVertexCount()].position = {geometry->vertices[0][0], geometry->vertices[0][1]};
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
		recreateCenterOfMassBuffer(instanceCount * sizeof(CenterOfMass));
		recreateIndexBuffer(indexBufferSize);

		copyBuffers(stagingBuffer);
	}

	#ifdef SHOW_DISTANCE
		void Physics2DRenderer::setClosestPoints(const vec2<float32>& closestPointA, const vec2<float32>& closestPointB) const {
			closestPointBufferMappedRange[0].position = {closestPointA[0], closestPointA[1]};
			closestPointBufferMappedRange[1].position = {closestPointB[0], closestPointB[1]};
		}
	#endif

	void Physics2DRenderer::setInstanceTransform(uint32 instanceIndex, const mat3x3<float32>& transform) const {
		instanceBufferMappedRange[instanceIndex].world = transform;
	}

	void Physics2DRenderer::setMousePosition(float32 x, float32 y) {
		mousePosition = {x, y};

		const float32 inverseZoomLevel = 1.0f / cameraBufferMappedRange->zoomLevel;

		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 horizontalCellCount = swapchainExtent.width * inverseZoomLevel;
		const float32 verticalCellCount = swapchainExtent.height * inverseZoomLevel;

		x = (x / swapchainExtent.width - 0.5f) * horizontalCellCount;
		y = -(y / swapchainExtent.height - 0.5f) * verticalCellCount;

		ndcMousePosition = {x, y};
	}

	void Physics2DRenderer::setPlaceholderOffset(const vec2<float32>& offset) const {
		placeholderBufferMappedRange->offset = {offset[0], offset[1]};
	}

	#ifdef SHOW_FEATURES
		void Physics2DRenderer::registerFeature(const Feature2D& feature) {
			switch (feature.getVertexCount()) {
				case 1:
					vertexFeatureBufferMappedRange[vertexFeatureCount].position = {feature.vertex[0], feature.vertex[1]};
					vertexFeatureCount += 1;

					break;
				case 2:
					edgeFeatureBufferMappedRange[edgeFeatureCount * 2 + 0].position = {feature.edge[0][0], feature.edge[0][1]};
					edgeFeatureBufferMappedRange[edgeFeatureCount * 2 + 1].position = {feature.edge[1][0], feature.edge[1][1]};
					edgeFeatureCount += 1;

					break;
				default:
					DEBUG_ASSERT(false);

					break;
			}
		}
	#endif

	void Physics2DRenderer::selectEntity(Entity entity) {
		if (selectedInstanceIndex != max<uint32>()) {
			instanceBufferMappedRange[selectedInstanceIndex].selected = 0;
		}

		if (entity == max<uint32>()) {
			selectedInstanceIndex = entity;

			return;
		}

		selectedInstanceIndex = scene->getComponentPool<RigidBody2D>().entityToComponentIndex[entity];
		instanceBufferMappedRange[selectedInstanceIndex].selected = 1;
	}

	void Physics2DRenderer::updateCameraViewDescriptorSet() const {
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

	void Physics2DRenderer::updatePlaceholderDescriptorSet() const {
		DescriptorBuffer placeholderDescriptorBuffer{};
		placeholderDescriptorBuffer.buffer = placeholderBuffer;
		placeholderDescriptorBuffer.offset = 0;
		placeholderDescriptorBuffer.size = placeholderBuffer->size;

		WriteDescriptorSet placeholderWriteDescriptorSet{};
		placeholderWriteDescriptorSet.descriptorSet = placeholderDescriptorSet;
		placeholderWriteDescriptorSet.bindingIndex = 0;
		placeholderWriteDescriptorSet.descriptorBuffers = {placeholderDescriptorBuffer};

		getDevice()->updateDescriptorSets({placeholderWriteDescriptorSet});
	}

	void Physics2DRenderer::updateCentersOfMass() const {
		const ComponentPool<RigidBody2D>& rigidBodyPool = scene->getComponentPool<RigidBody2D>();
		const uint32 instanceCount = rigidBodyPool.componentCount;
		const float32 inverseInstanceCount = 1.0f / instanceCount;

		for (uint32 instanceIndex = 0; instanceIndex < instanceCount; instanceIndex += 1) {
			const RigidBody2D& rigidBody = rigidBodyPool.components[instanceIndex];

			centerOfMassBufferMappedRange[instanceIndex].position = {
				rigidBody.position[0],
				rigidBody.position[1],
				instanceIndex * inverseInstanceCount,
			};
		}
	}

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

		setMousePosition(mousePosition[0], mousePosition[1]);
	}

	void Physics2DRenderer::render(ref<CommandEncoder> commandEncoder) const {
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

		#ifdef SHOW_DISTANCE
			renderDistance(commandEncoder);
		#endif

		#ifdef SHOW_FEATURES
			renderFeatures(commandEncoder);
		#endif

		if (selectedInstanceIndex != max<uint32>()) {
			renderSelectedInstancePlaceholder(commandEncoder);
		}

		commandEncoder->endRenderPass();
	}

	void Physics2DRenderer::renderGrid(ref<CommandEncoder> commandEncoder) const {
		commandEncoder->bindRenderPipeline(gridRenderPipeline);
		commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, {cameraViewDescriptorSet});
		commandEncoder->draw(3, 1, 0, 0);
	}

	void Physics2DRenderer::renderScene(ref<CommandEncoder> commandEncoder) const {
		commandEncoder->bindRenderPipeline(fillRenderPipeline);
		commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, {cameraViewDescriptorSet});
		commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);

		const ComponentPool<RigidBody2D>& rigidBodyPool = scene->getComponentPool<RigidBody2D>();
		const uint32 instanceCount = rigidBodyPool.componentCount;

		uint32 instanceIndex = 0;
		uint32 vertexOffset = 0;
		uint32 indexOffset = 0;

		while (instanceIndex < instanceCount) {
			const ref<Geometry2D> geometry = rigidBodyPool.components[instanceIndex].geometry;
			const uint32 indexCount = geometry->getIndexCount();
			const uint32 vertexCount = geometry->getVertexCount() + 1;

			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(Vertex), sizeof(Vertex));
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
			const ref<Geometry2D> geometry = rigidBodyPool.components[instanceIndex].geometry;
			const uint32 vertexCount = geometry->getVertexCount() + 1;

			commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(Vertex), sizeof(Vertex));
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

		updateCentersOfMass();

		commandEncoder->bindRenderPipeline(centerOfMassRenderPipeline);
		commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, {cameraViewDescriptorSet});
		commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, 0, sizeof(CenterOfMass));
		commandEncoder->draw(instanceCount, 1, 0, 0);
	}

	void Physics2DRenderer::renderSelectedInstancePlaceholder(ref<CommandEncoder> commandEncoder) const {
		const uint32 instanceCount = scene->getComponentPool<RigidBody2D>().componentCount;

		DEBUG_ASSERT(selectedInstanceIndex < instanceCount);

		uint32 vertexOffset = 0;

		for (Entity entity = 0; entity < selectedInstanceIndex; entity += 1) {
			const ref<Geometry2D> geometry = scene->getGeometry(entity);

			vertexOffset += geometry->getVertexCount() + 1;
		}

		const ref<Geometry2D> geometry = scene->getGeometry(selectedInstanceIndex);

		commandEncoder->bindRenderPipeline(placeholderCenterOfMassRenderPipeline);
		commandEncoder->bindDescriptorSets(placeholderPipelineLayout, {cameraViewDescriptorSet, placeholderDescriptorSet});
		commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, selectedInstanceIndex * sizeof(CenterOfMass), sizeof(CenterOfMass));
		commandEncoder->draw(1, 1, 0, 0);

		commandEncoder->bindRenderPipeline(placeholderFillRenderPipeline);
		commandEncoder->bindDescriptorSets(placeholderPipelineLayout, {cameraViewDescriptorSet, placeholderDescriptorSet});
		commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(Vertex), sizeof(Vertex));
		commandEncoder->bindVertexBuffer(1, instanceBuffer, selectedInstanceIndex * sizeof(Instance), sizeof(Instance));
		commandEncoder->drawIndexed(geometry->getIndexCount(), 1, 0, 0, 0);

		commandEncoder->bindRenderPipeline(placeholderStrokeRenderPipeline);
		commandEncoder->bindDescriptorSets(placeholderPipelineLayout, {cameraViewDescriptorSet, placeholderDescriptorSet});
		commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(Vertex), sizeof(Vertex));
		commandEncoder->bindVertexBuffer(1, instanceBuffer, selectedInstanceIndex * sizeof(Instance), sizeof(Instance));
		commandEncoder->draw(geometry->getVertexCount() + 1, 1, 0, 0);
	}

	#ifdef SHOW_DISTANCE
		void Physics2DRenderer::renderDistance(ref<CommandEncoder> commandEncoder) const {
			commandEncoder->bindRenderPipeline(distanceRenderPipeline);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, {cameraViewDescriptorSet});
			commandEncoder->bindVertexBuffer(0, closestPointBuffer, 0, sizeof(Vertex));
			commandEncoder->draw(2, 1, 0, 0);
		}
	#endif

	#ifdef SHOW_FEATURES
		void Physics2DRenderer::renderFeatures(ref<CommandEncoder> commandEncoder) const {
			if (vertexFeatureCount > 0) {
				commandEncoder->bindRenderPipeline(vertexFeatureRenderPipeline);
				commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, {cameraViewDescriptorSet});
				commandEncoder->bindVertexBuffer(0, vertexFeatureBuffer, 0, sizeof(Vertex));
				commandEncoder->draw(vertexFeatureCount, 1, 0, 0);
			}

			if (edgeFeatureCount > 0) {
				commandEncoder->bindRenderPipeline(edgeFeatureRenderPipeline);
				commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, {cameraViewDescriptorSet});
				commandEncoder->bindVertexBuffer(0, edgeFeatureBuffer, 0, sizeof(Vertex));
				commandEncoder->draw(edgeFeatureCount * 2, 1, 0, 0);
			}
		}
	#endif

	void Physics2DRenderer::resize(WindowResizeEvent& event) {
		if (layerInput->swapchainImage == nullptr) {
			recreateMultisampleImage();
			recreateMultisampleImageView();
		}

		recreateDepthStencilImage();
		recreateDepthStencilImageView();
		recreateFramebuffers();

		updateCameraProjection();
		updateViewport();
	}

	void Physics2DRenderer::clearRegisteredData() {
		#ifdef SHOW_DISTANCE
			closestPointBufferMappedRange[0].position = {0.0f, 0.0f};
			closestPointBufferMappedRange[1].position = {0.0f, 0.0f};
		#endif

		#ifdef SHOW_FEATURES
			vertexFeatureCount = 0;
			edgeFeatureCount = 0;
		#endif
	}
}