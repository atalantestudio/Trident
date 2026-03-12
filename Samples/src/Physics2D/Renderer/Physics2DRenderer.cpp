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

		#if defined(SHOW_DISTANCE)
			createClosestPointBuffer();
		#endif

		#if defined(SHOW_FEATURES)
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

		createGridGraphicsPipeline();
		createFillGraphicsPipeline();
		createStrokeGraphicsPipeline();
		createCenterOfMassGraphicsPipeline();
		createPlaceholderFillGraphicsPipeline();
		createPlaceholderStrokeGraphicsPipeline();
		createPlaceholderCenterOfMassGraphicsPipeline();

		#if defined(SHOW_DISTANCE)
			createDistanceGraphicsPipeline();
		#endif

		#if defined(SHOW_FEATURES)
			createVertexFeatureGraphicsPipeline();
			createEdgeFeatureGraphicsPipeline();
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

		#if defined(BUILD_MODE_DEBUG)
			sequence<char8> name;
		#endif

		uint32 framebufferIndex = 0;

		while (framebufferIndex < framebuffers.count()) {
			#if defined(BUILD_MODE_DEBUG)
				name = Logger::format("In flight framebuffer []", framebufferIndex);

				SET_DEBUG_NAME(framebufferDescriptor, name);
			#endif

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

		vertexBuffer = createBuffer<Vertex>(vertexBufferDescriptor);
	}

	void Physics2DRenderer::recreateInstanceBuffer(uint64 size) {
		BufferDescriptor instanceBufferDescriptor{};
		instanceBufferDescriptor.size = size;
		instanceBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		instanceBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(instanceBufferDescriptor, "Instance");

		instanceBuffer = createBuffer<Instance>(instanceBufferDescriptor);

		instanceBufferMappedRange = instanceBuffer->map(0, instanceBuffer->getSize());
	}

	void Physics2DRenderer::recreateCenterOfMassBuffer(uint64 size) {
		BufferDescriptor centerOfMassBufferDescriptor{};
		centerOfMassBufferDescriptor.size = size;
		centerOfMassBufferDescriptor.usage = BufferUsage::VERTEX;
		centerOfMassBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		centerOfMassBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(centerOfMassBufferDescriptor, "Center of mass");

		centerOfMassBuffer = createBuffer<CenterOfMass>(centerOfMassBufferDescriptor);

		centerOfMassBufferMappedRange = centerOfMassBuffer->map(0, centerOfMassBuffer->getSize());
	}

	void Physics2DRenderer::recreateIndexBuffer(uint64 size) {
		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = size;
		indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
		indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(indexBufferDescriptor, "Index");

		indexBuffer = createBuffer<uint16>(indexBufferDescriptor);
	}

	void Physics2DRenderer::createCameraBuffer() {
		BufferDescriptor cameraBufferDescriptor{};
		cameraBufferDescriptor.size = sizeof(Camera);
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		cameraBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(cameraBufferDescriptor, "Camera");

		cameraBuffer = createBuffer<Camera>(cameraBufferDescriptor);

		cameraBufferMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());
		cameraBufferMappedRange->zoomLevel = DEFAULT_ZOOM_LEVEL;
	}

	void Physics2DRenderer::createViewBuffer() {
		BufferDescriptor viewBufferDescriptor{};
		viewBufferDescriptor.size = sizeof(View);
		viewBufferDescriptor.usage = BufferUsage::UNIFORM;
		viewBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		viewBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(viewBufferDescriptor, "View");

		viewBuffer = createBuffer<View>(viewBufferDescriptor);

		viewBufferMappedRange = viewBuffer->map(0, viewBuffer->getSize());
	}

	void Physics2DRenderer::createPlaceholderBuffer() {
		BufferDescriptor placeholderBufferDescriptor{};
		placeholderBufferDescriptor.size = sizeof(Placeholder);
		placeholderBufferDescriptor.usage = BufferUsage::UNIFORM;
		placeholderBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		placeholderBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(placeholderBufferDescriptor, "Placeholder");

		placeholderBuffer = createBuffer<Placeholder>(placeholderBufferDescriptor);

		placeholderBufferMappedRange = placeholderBuffer->map(0, placeholderBuffer->getSize());
		placeholderBufferMappedRange->offset = {0.0f, 0.0f};
	}

	#if defined(SHOW_DISTANCE)
		void Physics2DRenderer::createClosestPointBuffer() {
			BufferDescriptor closestPointBufferDescriptor{};
			closestPointBufferDescriptor.size = 2 * sizeof(Vertex);
			closestPointBufferDescriptor.usage = BufferUsage::VERTEX;
			closestPointBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
			closestPointBufferDescriptor.persistent = true;

			SET_DEBUG_NAME(closestPointBufferDescriptor, "Closest point");

			closestPointBuffer = createBuffer<Vertex>(closestPointBufferDescriptor);

			closestPointBufferMappedRange = closestPointBuffer->map<Vertex>(0, closestPointBuffer->getSize());
		}
	#endif

	#if defined(SHOW_FEATURES)
		void Physics2DRenderer::createVertexFeatureBuffer() {
			BufferDescriptor vertexFeatureBufferDescriptor{};
			vertexFeatureBufferDescriptor.size = 2 * sizeof(Vertex);
			vertexFeatureBufferDescriptor.usage = BufferUsage::VERTEX;
			vertexFeatureBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
			vertexFeatureBufferDescriptor.persistent = true;

			SET_DEBUG_NAME(vertexFeatureBufferDescriptor, "Vertex feature");

			vertexFeatureBuffer = createBuffer<Vertex>(vertexFeatureBufferDescriptor);

			vertexFeatureBufferMappedRange = vertexFeatureBuffer->map<Vertex>(0, vertexFeatureBuffer->getSize());
		}

		void Physics2DRenderer::createEdgeFeatureBuffer() {
			BufferDescriptor edgeFeatureBufferDescriptor{};
			edgeFeatureBufferDescriptor.size = 4 * sizeof(Vertex);
			edgeFeatureBufferDescriptor.usage = BufferUsage::VERTEX;
			edgeFeatureBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
			edgeFeatureBufferDescriptor.persistent = true;

			SET_DEBUG_NAME(edgeFeatureBufferDescriptor, "Edge feature");

			edgeFeatureBuffer = createBuffer<Vertex>(edgeFeatureBufferDescriptor);

			edgeFeatureBufferMappedRange = edgeFeatureBuffer->map<Vertex>(0, edgeFeatureBuffer->getSize());
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

	void Physics2DRenderer::createGridGraphicsPipeline() {
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

		GraphicsPipelineDescriptor gridGraphicsPipelineDescriptor{};
		gridGraphicsPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
		gridGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		gridGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		gridGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		gridGraphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
		gridGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
		gridGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		gridGraphicsPipelineDescriptor.lineWidth = 1.0f;
		gridGraphicsPipelineDescriptor.minSampleShading = 0.0f;
		gridGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		gridGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
		gridGraphicsPipelineDescriptor.depthStencilState.depthWrite = false;
		gridGraphicsPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::NEVER;
		gridGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		gridGraphicsPipelineDescriptor.renderPass = renderPass;
		gridGraphicsPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(gridGraphicsPipelineDescriptor, "Grid");

		gridGraphicsPipeline = createGraphicsPipeline(gridGraphicsPipelineDescriptor);
	}

	void Physics2DRenderer::createFillGraphicsPipeline() {
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

		GraphicsPipelineDescriptor fillGraphicsPipelineDescriptor{};
		fillGraphicsPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
		fillGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		fillGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		fillGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		fillGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		fillGraphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
		fillGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
		fillGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		fillGraphicsPipelineDescriptor.lineWidth = 1.0f;
		fillGraphicsPipelineDescriptor.minSampleShading = 0.0f;
		fillGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		fillGraphicsPipelineDescriptor.depthStencilState.depthTest = true;
		fillGraphicsPipelineDescriptor.depthStencilState.depthWrite = true;
		fillGraphicsPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		fillGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		fillGraphicsPipelineDescriptor.renderPass = renderPass;
		fillGraphicsPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(fillGraphicsPipelineDescriptor, "Fill");

		fillGraphicsPipeline = createGraphicsPipeline(fillGraphicsPipelineDescriptor);
	}

	void Physics2DRenderer::createStrokeGraphicsPipeline() {
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

		GraphicsPipelineDescriptor strokeGraphicsPipelineDescriptor{};
		strokeGraphicsPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
		strokeGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		strokeGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		strokeGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		strokeGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_STRIP;
		strokeGraphicsPipelineDescriptor.polygonMode = PolygonMode::LINE;
		strokeGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
		strokeGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		strokeGraphicsPipelineDescriptor.lineWidth = 1.0f;
		strokeGraphicsPipelineDescriptor.minSampleShading = 0.0f;
		strokeGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		strokeGraphicsPipelineDescriptor.depthStencilState.depthTest = true;
		strokeGraphicsPipelineDescriptor.depthStencilState.depthWrite = true;
		strokeGraphicsPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		strokeGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR | DynamicState::LINE_WIDTH;
		strokeGraphicsPipelineDescriptor.renderPass = renderPass;
		strokeGraphicsPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(strokeGraphicsPipelineDescriptor, "Stroke");

		strokeGraphicsPipeline = createGraphicsPipeline(strokeGraphicsPipelineDescriptor);
	}

	void Physics2DRenderer::createCenterOfMassGraphicsPipeline() {
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

		GraphicsPipelineDescriptor centerOfMassGraphicsPipelineDescriptor{};
		centerOfMassGraphicsPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
		centerOfMassGraphicsPipelineDescriptor.vertexInputBindings = {centerOfMassVertexInputBinding};
		centerOfMassGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		centerOfMassGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		centerOfMassGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
		centerOfMassGraphicsPipelineDescriptor.polygonMode = PolygonMode::POINT;
		centerOfMassGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
		centerOfMassGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		centerOfMassGraphicsPipelineDescriptor.lineWidth = 1.0f;
		centerOfMassGraphicsPipelineDescriptor.minSampleShading = 1.0f;
		centerOfMassGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		centerOfMassGraphicsPipelineDescriptor.depthStencilState.depthTest = true;
		centerOfMassGraphicsPipelineDescriptor.depthStencilState.depthWrite = true;
		centerOfMassGraphicsPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		centerOfMassGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		centerOfMassGraphicsPipelineDescriptor.renderPass = renderPass;
		centerOfMassGraphicsPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(centerOfMassGraphicsPipelineDescriptor, "Center of mass");

		centerOfMassGraphicsPipeline = createGraphicsPipeline(centerOfMassGraphicsPipelineDescriptor);
	}

	void Physics2DRenderer::createPlaceholderFillGraphicsPipeline() {
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

		GraphicsPipelineDescriptor placeholderFillGraphicsPipelineDescriptor{};
		placeholderFillGraphicsPipelineDescriptor.pipelineLayout = placeholderPipelineLayout;
		placeholderFillGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		placeholderFillGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		placeholderFillGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		placeholderFillGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		placeholderFillGraphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
		placeholderFillGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
		placeholderFillGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		placeholderFillGraphicsPipelineDescriptor.lineWidth = 1.0f;
		placeholderFillGraphicsPipelineDescriptor.minSampleShading = 0.0f;
		placeholderFillGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		placeholderFillGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
		placeholderFillGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		placeholderFillGraphicsPipelineDescriptor.renderPass = renderPass;
		placeholderFillGraphicsPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(placeholderFillGraphicsPipelineDescriptor, "Placeholder fill");

		placeholderFillGraphicsPipeline = createGraphicsPipeline(placeholderFillGraphicsPipelineDescriptor);
	}

	void Physics2DRenderer::createPlaceholderStrokeGraphicsPipeline() {
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

		GraphicsPipelineDescriptor placeholderStrokeGraphicsPipelineDescriptor{};
		placeholderStrokeGraphicsPipelineDescriptor.pipelineLayout = placeholderPipelineLayout;
		placeholderStrokeGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		placeholderStrokeGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		placeholderStrokeGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		placeholderStrokeGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_STRIP;
		placeholderStrokeGraphicsPipelineDescriptor.polygonMode = PolygonMode::LINE;
		placeholderStrokeGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
		placeholderStrokeGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		placeholderStrokeGraphicsPipelineDescriptor.lineWidth = 1.0f;
		placeholderStrokeGraphicsPipelineDescriptor.minSampleShading = 0.0f;
		placeholderStrokeGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		placeholderStrokeGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
		placeholderStrokeGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		placeholderStrokeGraphicsPipelineDescriptor.renderPass = renderPass;
		placeholderStrokeGraphicsPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(placeholderStrokeGraphicsPipelineDescriptor, "Placeholder stroke");

		placeholderStrokeGraphicsPipeline = createGraphicsPipeline(placeholderStrokeGraphicsPipelineDescriptor);
	}

	void Physics2DRenderer::createPlaceholderCenterOfMassGraphicsPipeline() {
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

		GraphicsPipelineDescriptor placeholderCenterOfMassGraphicsPipelineDescriptor{};
		placeholderCenterOfMassGraphicsPipelineDescriptor.pipelineLayout = placeholderPipelineLayout;
		placeholderCenterOfMassGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
		placeholderCenterOfMassGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		placeholderCenterOfMassGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		placeholderCenterOfMassGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
		placeholderCenterOfMassGraphicsPipelineDescriptor.polygonMode = PolygonMode::POINT;
		placeholderCenterOfMassGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
		placeholderCenterOfMassGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		placeholderCenterOfMassGraphicsPipelineDescriptor.lineWidth = 1.0f;
		placeholderCenterOfMassGraphicsPipelineDescriptor.minSampleShading = 1.0f;
		placeholderCenterOfMassGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		placeholderCenterOfMassGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
		placeholderCenterOfMassGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		placeholderCenterOfMassGraphicsPipelineDescriptor.renderPass = renderPass;
		placeholderCenterOfMassGraphicsPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(placeholderCenterOfMassGraphicsPipelineDescriptor, "Placeholder center of mass");

		placeholderCenterOfMassGraphicsPipeline = createGraphicsPipeline(placeholderCenterOfMassGraphicsPipelineDescriptor);
	}

	#if defined(SHOW_DISTANCE)
		void Physics2DRenderer::createDistanceGraphicsPipeline() {
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

			GraphicsPipelineDescriptor distanceGraphicsPipelineDescriptor{};
			distanceGraphicsPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
			distanceGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
			distanceGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
			distanceGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
			distanceGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_LIST;
			distanceGraphicsPipelineDescriptor.polygonMode = PolygonMode::LINE;
			distanceGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
			distanceGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
			distanceGraphicsPipelineDescriptor.lineWidth = 1.0f;
			distanceGraphicsPipelineDescriptor.minSampleShading = 0.0f;
			distanceGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
			distanceGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
			distanceGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
			distanceGraphicsPipelineDescriptor.renderPass = renderPass;
			distanceGraphicsPipelineDescriptor.subpassIndex = 0;

			SET_DEBUG_NAME(distanceGraphicsPipelineDescriptor, "Distance");

			distanceGraphicsPipeline = createGraphicsPipeline(distanceGraphicsPipelineDescriptor);
		}
	#endif

	#if defined(SHOW_FEATURES)
		void Physics2DRenderer::createVertexFeatureGraphicsPipeline() {
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

			GraphicsPipelineDescriptor vertexFeatureGraphicsPipelineDescriptor{};
			vertexFeatureGraphicsPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
			vertexFeatureGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
			vertexFeatureGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
			vertexFeatureGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
			vertexFeatureGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::POINT_LIST;
			vertexFeatureGraphicsPipelineDescriptor.polygonMode = PolygonMode::POINT;
			vertexFeatureGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
			vertexFeatureGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
			vertexFeatureGraphicsPipelineDescriptor.lineWidth = 1.0f;
			vertexFeatureGraphicsPipelineDescriptor.minSampleShading = 1.0f;
			vertexFeatureGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
			vertexFeatureGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
			vertexFeatureGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
			vertexFeatureGraphicsPipelineDescriptor.renderPass = renderPass;
			vertexFeatureGraphicsPipelineDescriptor.subpassIndex = 0;

			SET_DEBUG_NAME(vertexFeatureGraphicsPipelineDescriptor, "Vertex feature");

			vertexFeatureGraphicsPipeline = createGraphicsPipeline(vertexFeatureGraphicsPipelineDescriptor);
		}

		void Physics2DRenderer::createEdgeFeatureGraphicsPipeline() {
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

			GraphicsPipelineDescriptor edgeFeatureGraphicsPipelineDescriptor{};
			edgeFeatureGraphicsPipelineDescriptor.pipelineLayout = cameraViewPipelineLayout;
			edgeFeatureGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding};
			edgeFeatureGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
			edgeFeatureGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
			edgeFeatureGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::LINE_LIST;
			edgeFeatureGraphicsPipelineDescriptor.polygonMode = PolygonMode::LINE;
			edgeFeatureGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
			edgeFeatureGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
			edgeFeatureGraphicsPipelineDescriptor.lineWidth = 3.0f;
			edgeFeatureGraphicsPipelineDescriptor.minSampleShading = 0.0f;
			edgeFeatureGraphicsPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
			edgeFeatureGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
			edgeFeatureGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
			edgeFeatureGraphicsPipelineDescriptor.renderPass = renderPass;
			edgeFeatureGraphicsPipelineDescriptor.subpassIndex = 0;

			SET_DEBUG_NAME(edgeFeatureGraphicsPipelineDescriptor, "Edge feature");

			edgeFeatureGraphicsPipeline = createGraphicsPipeline(edgeFeatureGraphicsPipelineDescriptor);
		}
	#endif

	void Physics2DRenderer::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({cameraViewDescriptorSetLayout, placeholderDescriptorSetLayout});

		cameraViewDescriptorSet = descriptorSets[0];
		placeholderDescriptorSet = descriptorSets[1];
	}

	void Physics2DRenderer::copyBuffers(ref<Buffer<uint8>> stagingBuffer) const {
		BufferCopyRegion vertexBufferCopyRegion{};
		vertexBufferCopyRegion.sourceOffset = 0;
		vertexBufferCopyRegion.destinationOffset = 0;
		vertexBufferCopyRegion.size = vertexBuffer->getSize();

		BufferCopyRegion instanceBufferCopyRegion{};
		instanceBufferCopyRegion.sourceOffset = vertexBuffer->getSize();
		instanceBufferCopyRegion.destinationOffset = 0;
		instanceBufferCopyRegion.size = instanceBuffer->getSize();

		BufferCopyRegion indexBufferCopyRegion{};
		indexBufferCopyRegion.sourceOffset = vertexBuffer->getSize() + instanceBuffer->getSize();
		indexBufferCopyRegion.destinationOffset = 0;
		indexBufferCopyRegion.size = indexBuffer->getSize();

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

		transferQueue->submit({commandBuffer});
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

		const ref<Buffer<uint8>> stagingBuffer = createStagingBuffer<uint8>(vertexBufferSize + indexBufferSize + instanceBufferSize);

		uint8* mappedRange = stagingBuffer->map(0, stagingBuffer->getSize());
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

	#if defined(SHOW_DISTANCE)
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

	#if defined(SHOW_FEATURES)
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

		cameraDescriptorBuffer.size = cameraBuffer->getSize();

		DescriptorBuffer viewDescriptorBuffer{};
		viewDescriptorBuffer.buffer = viewBuffer;
		viewDescriptorBuffer.offset = 0;
		viewDescriptorBuffer.size = viewBuffer->getSize();

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
		placeholderDescriptorBuffer.size = placeholderBuffer->getSize();

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
		beginRenderPassDescriptor.framebuffer = framebuffers[getSwapchainImageIndex()];
		beginRenderPassDescriptor.clearValues = {colorClearValue, depthStencilClearValue, colorClearValue};

		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(swapchainExtent.width);
		const float32 height = static_cast<float32>(swapchainExtent.height);

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchainExtent);

		renderGrid(commandEncoder);
		renderScene(commandEncoder);

		#if defined(SHOW_DISTANCE)
			renderDistance(commandEncoder);
		#endif

		#if defined(SHOW_FEATURES)
			renderFeatures(commandEncoder);
		#endif

		if (selectedInstanceIndex != max<uint32>()) {
			renderSelectedInstancePlaceholder(commandEncoder);
		}

		commandEncoder->endRenderPass();
	}

	void Physics2DRenderer::renderGrid(ref<CommandEncoder> commandEncoder) const {
		commandEncoder->bindGraphicsPipeline(gridGraphicsPipeline);
		commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});
		commandEncoder->draw(3, 1, 0, 0);
	}

	void Physics2DRenderer::renderScene(ref<CommandEncoder> commandEncoder) const {
		commandEncoder->bindGraphicsPipeline(fillGraphicsPipeline);
		commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});
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

		commandEncoder->bindGraphicsPipeline(strokeGraphicsPipeline);
		commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});

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

		commandEncoder->bindGraphicsPipeline(centerOfMassGraphicsPipeline);
		commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});
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

		commandEncoder->bindGraphicsPipeline(placeholderCenterOfMassGraphicsPipeline);
		commandEncoder->bindDescriptorSets(placeholderPipelineLayout, 0, {cameraViewDescriptorSet, placeholderDescriptorSet});
		commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, selectedInstanceIndex * sizeof(CenterOfMass), sizeof(CenterOfMass));
		commandEncoder->draw(1, 1, 0, 0);

		commandEncoder->bindGraphicsPipeline(placeholderFillGraphicsPipeline);
		commandEncoder->bindDescriptorSets(placeholderPipelineLayout, 0, {cameraViewDescriptorSet, placeholderDescriptorSet});
		commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(Vertex), sizeof(Vertex));
		commandEncoder->bindVertexBuffer(1, instanceBuffer, selectedInstanceIndex * sizeof(Instance), sizeof(Instance));
		commandEncoder->drawIndexed(geometry->getIndexCount(), 1, 0, 0, 0);

		commandEncoder->bindGraphicsPipeline(placeholderStrokeGraphicsPipeline);
		commandEncoder->bindDescriptorSets(placeholderPipelineLayout, 0, {cameraViewDescriptorSet, placeholderDescriptorSet});
		commandEncoder->bindVertexBuffer(0, vertexBuffer, vertexOffset * sizeof(Vertex), sizeof(Vertex));
		commandEncoder->bindVertexBuffer(1, instanceBuffer, selectedInstanceIndex * sizeof(Instance), sizeof(Instance));
		commandEncoder->draw(geometry->getVertexCount() + 1, 1, 0, 0);
	}

	#if defined(SHOW_DISTANCE)
		void Physics2DRenderer::renderDistance(ref<CommandEncoder> commandEncoder) const {
			commandEncoder->bindGraphicsPipeline(distanceGraphicsPipeline);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});
			commandEncoder->bindVertexBuffer(0, closestPointBuffer, 0, sizeof(Vertex));
			commandEncoder->draw(2, 1, 0, 0);
		}
	#endif

	#if defined(SHOW_FEATURES)
		void Physics2DRenderer::renderFeatures(ref<CommandEncoder> commandEncoder) const {
			if (vertexFeatureCount > 0) {
				commandEncoder->bindGraphicsPipeline(vertexFeatureGraphicsPipeline);
				commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});
				commandEncoder->bindVertexBuffer(0, vertexFeatureBuffer, 0, sizeof(Vertex));
				commandEncoder->draw(vertexFeatureCount, 1, 0, 0);
			}

			if (edgeFeatureCount > 0) {
				commandEncoder->bindGraphicsPipeline(edgeFeatureGraphicsPipeline);
				commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});
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
		#if defined(SHOW_DISTANCE)
			closestPointBufferMappedRange[0].position = {0.0f, 0.0f};
			closestPointBufferMappedRange[1].position = {0.0f, 0.0f};
		#endif

		#if defined(SHOW_FEATURES)
			vertexFeatureCount = 0;
			edgeFeatureCount = 0;
		#endif
	}
}