// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2DRenderer.hpp"

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	void Physics2DRenderer::build() {
		setupRenderPass();

		#if PHYSICS_2D_RENDERER_SAMPLE_COUNT > 1
			if (layerInput->swapchainImage == nullptr) {
				recreateMultisampleImage();
				recreateMultisampleImageView();
			}
		#endif

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

		createGridPipeline();
		createRigidBodyFillPipeline();
		createRigidBodyStrokePipeline();
		createCenterOfMassPipeline();
		createRigidBodyPlaceholderFillPipeline();
		createRigidBodyPlaceholderStrokePipeline();
		createRigidBodyPlaceholderCenterOfMassPipeline();

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
		Attachment swapchainAttachment{};
		swapchainAttachment.format = getSurface()->getFormat();
		swapchainAttachment.sampleCount = PHYSICS_2D_RENDERER_SAMPLE_COUNT;
		swapchainAttachment.loadOperation = LoadOperation::CLEAR;
		swapchainAttachment.storeOperation = StoreOperation::STORE;

		switch (getGraphicsApi()) {
			case GraphicsAPI::DX12: {
				// TODO(DX12): The final layout must be PRESENT_SRC in order to have layer superposition working.
				swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

				break;
			}
			case GraphicsAPI::GL:
			case GraphicsAPI::VK: {
				// TODO(layer-superposition)
				swapchainAttachment.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

				break;
			}
		}

		Attachment depthStencilAttachment{};
		depthStencilAttachment.format = ImageFormat::D32_SFLOAT;
		depthStencilAttachment.sampleCount = PHYSICS_2D_RENDERER_SAMPLE_COUNT;
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

		layerInput->swapchainImage = renderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = {
					.width = swapchainExtent.width,
					.height = swapchainExtent.height,
					.depth = 1,
				},
				.mipLevelCount = 1,
				.layerCount = 1,
				#if defined(BUILD_MODE_DEBUG)
					.name = "Multisample attachment",
				#endif
			},
			0
		);
	}

	void Physics2DRenderer::recreateMultisampleImageView() {
		layerInput->swapchainImageView = layerInput->swapchainImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Multisample attachment",
			#endif
		});
	}

	void Physics2DRenderer::recreateDepthStencilImage() {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		depthStencilImage = renderPass->createDepthStencilAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = {
					.width = swapchainExtent.width,
					.height = swapchainExtent.height,
					.depth = 1,
				},
				.mipLevelCount = 1,
				.layerCount = 1,
				#if defined(BUILD_MODE_DEBUG)
					.name = "Depth stencil attachment",
				#endif
			},
			0
		);
	}

	void Physics2DRenderer::recreateDepthStencilImageView() {
		depthStencilImageView = depthStencilImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Depth stencil attachment",
			#endif
		});
	}

	void Physics2DRenderer::recreateFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor = {
			.renderPass = renderPass,
			.extent = swapchain->getExtent(),
		};

		#if PHYSICS_2D_RENDERER_SAMPLE_COUNT > 1
			framebufferDescriptor.imageViews = {layerInput->swapchainImageView, depthStencilImageView, nullptr};
		#else
			framebufferDescriptor.imageViews = {nullptr, depthStencilImageView};
		#endif

		#if defined(BUILD_MODE_DEBUG)
			sequence<char8> name;
		#endif

		uint32 framebufferIndex = 0;

		while (framebufferIndex < framebuffers.count()) {
			#if defined(BUILD_MODE_DEBUG)
				name = Logger::format("In flight framebuffer []", framebufferIndex);

				SET_DEBUG_NAME(framebufferDescriptor, name);
			#endif

			#if PHYSICS_2D_RENDERER_SAMPLE_COUNT > 1
				framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);
			#else
				framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);
			#endif

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

			framebufferIndex += 1;
		}
	}

	void Physics2DRenderer::createCameraBuffer() {
		cameraBuffer = createBuffer<Camera>({
			.size = sizeof(Camera),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Camera",
			#endif
		});

		cameraMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());
		cameraMappedRange->zoomLevel = DEFAULT_ZOOM_LEVEL;
	}

	void Physics2DRenderer::createViewBuffer() {
		viewBuffer = createBuffer<View>({
			.size = sizeof(View),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(BUILD_MODE_DEBUG)
				.name = "View",
			#endif
		});

		viewMappedRange = viewBuffer->map(0, viewBuffer->getSize());
	}

	void Physics2DRenderer::createPlaceholderBuffer() {
		placeholderBuffer = createBuffer<Placeholder>({
			.size = sizeof(Placeholder),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Placeholder",
			#endif
		});

		placeholderMappedRange = placeholderBuffer->map(0, placeholderBuffer->getSize());
		placeholderMappedRange->offset = {0.0f, 0.0f};
	}

	#if defined(SHOW_DISTANCE)
		void Physics2DRenderer::createClosestPointBuffer() {
			closestPointBuffer = createBuffer<Vertex>({
				.size = 2 * sizeof(Vertex),
				.usage = BufferUsage::VERTEX,
				.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
				.persistent = true,
				#if defined(BUILD_MODE_DEBUG)
					.name = "Closest point",
				#endif
			});

			closestPointMappedRange = closestPointBuffer->map(0, closestPointBuffer->getSize());
		}
	#endif

	#if defined(SHOW_FEATURES)
		void Physics2DRenderer::createVertexFeatureBuffer() {
			vertexFeatureBuffer = createBuffer<Vertex>({
				.size = 2 * sizeof(Vertex),
				.usage = BufferUsage::VERTEX,
				.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
				.persistent = true,
				#if defined(BUILD_MODE_DEBUG)
					.name = "Vertex feature",
				#endif
			});

			vertexFeatureMappedRange = vertexFeatureBuffer->map(0, vertexFeatureBuffer->getSize());
		}

		void Physics2DRenderer::createEdgeFeatureBuffer() {
			edgeFeatureBuffer = createBuffer<Vertex>({
				.size = 4 * sizeof(Vertex),
				.usage = BufferUsage::VERTEX,
				.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
				.persistent = true,
				#if defined(BUILD_MODE_DEBUG)
					.name = "Edge feature",
				#endif
			});

			edgeFeatureMappedRange = edgeFeatureBuffer->map(0, edgeFeatureBuffer->getSize());
		}
	#endif

	void Physics2DRenderer::createCameraViewDescriptorSetLayout() {
		cameraViewDescriptorSetLayout = createDescriptorSetLayout({
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
			},
			// View
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},
		});
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

	void Physics2DRenderer::createGridPipeline() {
		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Grid vertex");

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Grid fragment");

		const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/ScreenUVNDC.vert");
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/Grid.frag");

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

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
		gridGraphicsPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
		gridGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
		gridGraphicsPipelineDescriptor.depthStencilState.depthWrite = false;
		gridGraphicsPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::NEVER;
		gridGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		gridGraphicsPipelineDescriptor.renderPass = renderPass;
		gridGraphicsPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(gridGraphicsPipelineDescriptor, "Grid");

		gridPipeline = createGraphicsPipeline(gridGraphicsPipelineDescriptor);
	}

	void Physics2DRenderer::createRigidBodyFillPipeline() {
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

		SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Fill vertex");

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Fill fragment");

		const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/RigidBodyFill.vert");
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/FragmentRGBA.frag");

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = true;
		colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		rigidBodyFillPipeline = createGraphicsPipeline({
			.pipelineLayout = cameraViewPipelineLayout,
			.vertexInputBindings = {vertexInputBinding, instanceInputBinding},
			.vertexShaderModule = vertexShaderModule,
			.fragmentShaderModule = fragmentShaderModule,
			.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
			.polygonMode = PolygonMode::FILL,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = {colorBlendAttachment},
			},
			.depthStencilState = {
				.depthTest = true,
				.depthWrite = true,
				.compareOperation = CompareOperation::GREATER_OR_EQUAL,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Rigid body fill",
			#endif
		});
	}

	void Physics2DRenderer::createRigidBodyStrokePipeline() {
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

		SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Stroke vertex");

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Stroke fragment");

		const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/RigidBodyStroke.vert");
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/RigidBodyStroke.frag");

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = true;
		colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		rigidBodyStrokePipeline = createGraphicsPipeline({
			.pipelineLayout = cameraViewPipelineLayout,
			.vertexInputBindings = {vertexInputBinding, instanceInputBinding},
			.vertexShaderModule = vertexShaderModule,
			.fragmentShaderModule = fragmentShaderModule,
			.primitiveTopology = PrimitiveTopology::LINE_STRIP,
			.polygonMode = PolygonMode::LINE,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = {colorBlendAttachment},
			},
			.depthStencilState = {
				.depthTest = true,
				.depthWrite = true,
				.compareOperation = CompareOperation::GREATER_OR_EQUAL,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR /* | DynamicState::LINE_WIDTH */,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Rigid body stroke",
			#endif
		});
	}

	void Physics2DRenderer::createCenterOfMassPipeline() {
		VertexInputBinding centerOfMassVertexInputBinding(InputRate::VERTEX);
		centerOfMassVertexInputBinding.registerAttribute<vec3<float32>>();

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

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

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
		centerOfMassGraphicsPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
		centerOfMassGraphicsPipelineDescriptor.depthStencilState.depthTest = true;
		centerOfMassGraphicsPipelineDescriptor.depthStencilState.depthWrite = true;
		centerOfMassGraphicsPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		centerOfMassGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		centerOfMassGraphicsPipelineDescriptor.renderPass = renderPass;
		centerOfMassGraphicsPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(centerOfMassGraphicsPipelineDescriptor, "Center of mass");

		centerOfMassPipeline = createGraphicsPipeline(centerOfMassGraphicsPipelineDescriptor);
	}

	void Physics2DRenderer::createRigidBodyPlaceholderFillPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>();

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>();
		instanceInputBinding.registerAttribute<vec4<float32>>();
		instanceInputBinding.stride = sizeof(RigidBody);

		const ShaderModuleDescriptor vertexShaderModuleDescriptor = {
			.stage = ShaderStage::VERTEX,
			.entryPoint = "main",
			#if defined(BUILD_MODE_DEBUG)
				.name = "Rigid body placeholder fill vertex",
			#endif
		};

		ref<ShaderModule> vertexShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderFill.vert");

				break;
			}
			case GraphicsAPI::GL: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderFillSingleSet.vert");

				break;
			}
		}

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(
			{
				.stage = ShaderStage::FRAGMENT,
				.entryPoint = "main",
				#if defined(BUILD_MODE_DEBUG)
					.name = "Rigid body placeholder fill fragment",
				#endif
			},
			"Physics2D/FragmentRGBA.frag"
		);

		rigidBodyPlaceholderFillPipeline = createGraphicsPipeline({
			.pipelineLayout = placeholderPipelineLayout,
			.vertexInputBindings = {vertexInputBinding, instanceInputBinding},
			.vertexShaderModule = vertexShaderModule,
			.fragmentShaderModule = fragmentShaderModule,
			.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
			.polygonMode = PolygonMode::FILL,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = {
					{
						.blend = true,
						.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
						.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
					},
				},
			},
			.depthStencilState = {
				.depthTest = false,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Rigid body placeholder fill",
			#endif
		});
	}

	void Physics2DRenderer::createRigidBodyPlaceholderStrokePipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>();

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>();
		instanceInputBinding.stride = sizeof(RigidBody);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Rigid body placeholder stroke vertex");

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Rigid body placeholder stroke fragment");

		ref<ShaderModule> vertexShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderStroke.vert");

				break;
			}
			case GraphicsAPI::GL: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderStrokeSingleSet.vert");

				break;
			}
		}

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "Physics2D/PlaceholderStroke.frag");

		rigidBodyPlaceholderStrokePipeline = createGraphicsPipeline({
			.pipelineLayout = placeholderPipelineLayout,
			.vertexInputBindings = {vertexInputBinding, instanceInputBinding},
			.vertexShaderModule = vertexShaderModule,
			.fragmentShaderModule = fragmentShaderModule,
			.primitiveTopology = PrimitiveTopology::LINE_STRIP,
			.polygonMode = PolygonMode::LINE,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = {
					{
						.blend = true,
						.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
						.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
					},
				},
			},
			.depthStencilState = {
				.depthTest = false,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Rigid body placeholder stroke",
			#endif
		});
	}

	void Physics2DRenderer::createRigidBodyPlaceholderCenterOfMassPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec2<float32>>();
		vertexInputBinding.stride = sizeof(CenterOfMass);

		const ShaderModuleDescriptor vertexShaderModuleDescriptor = {
			.stage = ShaderStage::VERTEX,
			.entryPoint = "main",
			#if defined(BUILD_MODE_DEBUG)
				.name = "Rigid body placeholder center of mass vertex",
			#endif
		};

		ref<ShaderModule> vertexShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderCenterOfMass.vert");

				break;
			}
			case GraphicsAPI::GL: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderCenterOfMassSingleSet.vert");

				break;
			}
		}

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(
			{
				.stage = ShaderStage::FRAGMENT,
				.entryPoint = "main",
				#if defined(BUILD_MODE_DEBUG)
					.name = "Rigid body placeholder center of mass fragment",
				#endif
			},
			"Physics2D/CenterOfMass.frag"
		);

		rigidBodyPlaceholderCenterOfMassPipeline = createGraphicsPipeline({
			.pipelineLayout = placeholderPipelineLayout,
			.vertexInputBindings = {vertexInputBinding},
			.vertexShaderModule = vertexShaderModule,
			.fragmentShaderModule = fragmentShaderModule,
			.primitiveTopology = PrimitiveTopology::POINT_LIST,
			.polygonMode = PolygonMode::POINT,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 1.0f,
			.colorBlendState = {
				.attachments = {
					{
						.blend = false,
					},
				},
			},
			.depthStencilState = {
				.depthTest = false,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Rigid body placeholder center of mass",
			#endif
		});
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

			const sequence<ColorBlendAttachment> colorBlendAttachments = {
				{
					.blend = false,
				},
			};

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
			distanceGraphicsPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
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

			const sequence<ColorBlendAttachment> colorBlendAttachments = {
				{
					.blend = false,
				},
			};

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
			vertexFeatureGraphicsPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
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

			const sequence<ColorBlendAttachment> colorBlendAttachments = {
				{
					.blend = false,
				},
			};

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
			edgeFeatureGraphicsPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
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

	void Physics2DRenderer::setScene(const Scene2D* scene) {
		this->scene = scene;

		const sequence<uint8>& dataBuffer = scene->getDataBuffer();
		const sequence<Geometry2D>& geometries = scene->getGeometries();
		const sequence<Hull2D>& hulls = scene->getHulls();
		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();
		const ComponentPool<GeometryComponent>& geometryComponentPool = scene->ecs.getComponentPool<GeometryComponent>();
		const uint64 rigidBodyBufferSize = rigidBodyComponentPool.componentCount * sizeof(RigidBody);
		const ref<Buffer<uint8>> stagingBuffer = createStagingBuffer<uint8>(dataBuffer.count() + rigidBodyBufferSize);

		uint8* mappedRange = stagingBuffer->map(0, stagingBuffer->getSize());

		std::copy(dataBuffer.begin(), dataBuffer.end(), &mappedRange[0]);

		RigidBody* stagingRigidBodyMappedRange = pointer_cast<RigidBody*>(&mappedRange[dataBuffer.count()]);
		uint32 rigidBodyIndex = 0;

		while (rigidBodyIndex < rigidBodyComponentPool.componentCount) {
			const RigidBody2DComponent& rigidBodyComponent = rigidBodyComponentPool.components[rigidBodyIndex];
			const GeometryComponent geometryComponent = geometryComponentPool.components[rigidBodyComponentPool.componentIndexToEntity[rigidBodyIndex]];
			const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];
			const Hull2D& hull = hulls[geometry.hullIndex];

			RigidBody& rigidBody = stagingRigidBodyMappedRange[rigidBodyIndex];
			rigidBody.world = rigidBodyComponent.world;

			if (hull.getType() != Hull2DType::POLYGON) {
				// Make the hull more visible if it doesn't support wireframe visualization.
				rigidBody.fillColor = Color(0x8b9476ff).value;
			}
			else {
				rigidBody.fillColor = Color(0xf4fedbff).value;
			}

			rigidBody.strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};
			rigidBody.depth = static_cast<float32>(rigidBodyIndex) / rigidBodyComponentPool.componentCount;
			rigidBody.selected = 0;

			rigidBodyIndex += 1;
		}

		stagingBuffer->unmap();

		vertexBuffer = createBuffer<Vertex>({
			.size = (scene->getStaticVertexCount() + scene->getDynamicVertexCount()) * sizeof(Vertex),
			.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Vertex",
			#endif
		});
		indexBuffer = createBuffer<uint32>({
			.size = (scene->getStaticIndexCount() + scene->getDynamicIndexCount()) * sizeof(uint32),
			.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Index",
			#endif
		});
		rigidBodyBuffer = createBuffer<RigidBody>({
			.size = rigidBodyBufferSize,
			.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Rigid body",
			#endif
		});
		centerOfMassBuffer = createBuffer<CenterOfMass>({
			.size = rigidBodyComponentPool.componentCount * sizeof(CenterOfMass),
			.usage = BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Center of mass",
			#endif
		});

		rigidBodyMappedRange = rigidBodyBuffer->map(0, rigidBodyBuffer->getSize());
		centerOfMassMappedRange = centerOfMassBuffer->map(0, centerOfMassBuffer->getSize());

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Copy buffers",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {
			{
				.sourceOffset = 0,
				.destinationOffset = 0,
				.size = vertexBuffer->getSize(),
			},
		});
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, {
			{
				.sourceOffset = vertexBuffer->getSize(),
				.destinationOffset = 0,
				.size = indexBuffer->getSize(),
			},
		});
		commandEncoder->copyBufferToBuffer(stagingBuffer, rigidBodyBuffer, {
			{
				.sourceOffset = vertexBuffer->getSize() + indexBuffer->getSize(),
				.destinationOffset = 0,
				.size = rigidBodyBufferSize,
			},
		});

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer});
		transferQueue->waitIdle();
	}

	#if defined(SHOW_DISTANCE)
		void Physics2DRenderer::setClosestPoints(const vec2<float32>& closestPointA, const vec2<float32>& closestPointB) const {
			closestPointMappedRange[0].position = {closestPointA[0], closestPointA[1]};
			closestPointMappedRange[1].position = {closestPointB[0], closestPointB[1]};
		}
	#endif

	void Physics2DRenderer::setWorld(Entity entity, const mat3x3<float32>& world) const {
		rigidBodyMappedRange[entity].world = world;
	}

	void Physics2DRenderer::setPlaceholderOffset(vec2<float32> offset) const {
		placeholderMappedRange->offset = {offset[0], offset[1]};
	}

	#if defined(SHOW_FEATURES)
		void Physics2DRenderer::registerFeature(const Feature2D& feature) {
			switch (feature.vertexCount) {
				case 1: {
					vertexFeatureMappedRange[vertexFeatureCount].position = {feature.vertex[0], feature.vertex[1]};
					vertexFeatureCount += 1;

					break;
				}
				case 2: {
					edgeFeatureMappedRange[edgeFeatureCount * 2 + 0].position = {feature.edge[0][0], feature.edge[0][1]};
					edgeFeatureMappedRange[edgeFeatureCount * 2 + 1].position = {feature.edge[1][0], feature.edge[1][1]};
					edgeFeatureCount += 1;

					break;
				}
				default: {
					DEBUG_ASSERT(false);

					break;
				}
			}
		}
	#endif

	void Physics2DRenderer::selectEntity(Entity entity) {
		if (selectedEntity != max<Entity>()) {
			rigidBodyMappedRange[selectedEntity].selected = 0;
		}

		selectedEntity = entity;

		if (selectedEntity != max<Entity>()) {
			rigidBodyMappedRange[selectedEntity].selected = 1;
		}
	}

	void Physics2DRenderer::updateCameraViewDescriptorSet() const {
		DescriptorBuffer cameraDescriptorBuffer{};
		cameraDescriptorBuffer.buffer = cameraBuffer;
		cameraDescriptorBuffer.offset = 0;
		cameraDescriptorBuffer.size = cameraBuffer->getSize();

		DescriptorBuffer viewDescriptorBuffer{};
		viewDescriptorBuffer.buffer = viewBuffer;
		viewDescriptorBuffer.offset = 0;
		viewDescriptorBuffer.size = viewBuffer->getSize();

		const sequence<WriteDescriptorSet> writeDescriptorSets = {
			{
				.descriptorSet = cameraViewDescriptorSet,
				.bindingIndex = 0,
				.descriptorBuffers = {cameraDescriptorBuffer},
			},

			{
				.descriptorSet = cameraViewDescriptorSet,
				.bindingIndex = 1,
				.descriptorBuffers = {viewDescriptorBuffer},
			},
		};

		getDevice()->updateDescriptorSets(writeDescriptorSets);
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
		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();
		const uint32 componentCount = rigidBodyComponentPool.componentCount;
		const float32 inverseComponentCount = 1.0f / componentCount;

		uint32 componentIndex = 0;

		while (componentIndex < componentCount) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const RigidBody2DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

			centerOfMassMappedRange[entity].position = {rigidBodyComponent.position[0], rigidBodyComponent.position[1], componentIndex * inverseComponentCount};
			componentIndex += 1;
		}
	}

	void Physics2DRenderer::updateCameraProjection() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 w = static_cast<float32>(swapchainExtent.width);
		const float32 h = static_cast<float32>(swapchainExtent.height);
		const vec2<float32> O = -cameraPosition / (vec2<float32>(w, h) * 0.5f);

		// TODO: lyah::mat<3, 3, float32>::orthographic
		cameraMappedRange->projection = {
			2.0f / w,  0.0f,      0.0f,
			0.0f,      2.0f / h,  0.0f,
			O[0],      O[1],      1.0f,
		};
	}

	void Physics2DRenderer::updateViewport() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		viewMappedRange->viewport.x = static_cast<float32>(swapchainExtent.width);
		viewMappedRange->viewport.y = static_cast<float32>(swapchainExtent.height);
	}

	void Physics2DRenderer::zoom(float32 delta) {
		if (delta != 0.0f) {
			delta /= abs(delta);
		}

		cameraMappedRange->zoomLevel = clamp(cameraMappedRange->zoomLevel + delta, MIN_ZOOM_LEVEL, MAX_ZOOM_LEVEL);

		updateCameraProjection();
	}

	void Physics2DRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(swapchainExtent.width);
		const float32 height = static_cast<float32>(swapchainExtent.height);

		commandEncoder->beginRenderPass({
			.framebuffer = framebuffers[getSwapchainImageIndex()],
			.clearValues = {
				{1.0f, 1.0f, 1.0f, 1.0f},
				{
					.depth = 0.0f,
					.stencil = 0,
				},
				#if PHYSICS_2D_RENDERER_SAMPLE_COUNT > 1
					{0.0f, 0.0f, 0.0f, 1.0f},
				#endif
			},
		});
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchainExtent);

		// Grid
		{
			commandEncoder->bindGraphicsPipeline(gridPipeline);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});
			commandEncoder->draw(3, 1, 0, 0);
		}

		const sequence<Geometry2D>& geometries = scene->getGeometries();
		const sequence<Hull2D>& hulls = scene->getHulls();
		const ComponentPool<GeometryComponent>& geometryComponentPool = scene->ecs.getComponentPool<GeometryComponent>();

		uint32 componentIndex = 0;

		// Rigid body fill
		{
			commandEncoder->bindGraphicsPipeline(rigidBodyFillPipeline);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, scene->getStaticVertexCount() * sizeof(Vertex), sizeof(Vertex));
			commandEncoder->bindVertexBuffer(1, rigidBodyBuffer, 0, sizeof(RigidBody));
			commandEncoder->bindIndexBuffer<uint32>(indexBuffer, scene->getStaticIndexCount() * sizeof(uint32));
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});

			while (componentIndex < geometryComponentPool.componentCount) {
				const Entity entity = geometryComponentPool.componentIndexToEntity[componentIndex];
				const GeometryComponent geometryComponent = geometryComponentPool.components[componentIndex];
				const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];

				// TODO: Restore materials (test colors were 0xf4fedbff and 0xe6ddffff).
				commandEncoder->drawIndexed(geometry.getIndexRange().count, 1, geometry.getIndexRange().offset, geometry.getVertexPositionComponentRange().offset / 2, entity);

				componentIndex += 1;
			}
		}

		componentIndex = 0;

		// Rigid body stroke
		{
			commandEncoder->bindGraphicsPipeline(rigidBodyStrokePipeline);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, scene->getStaticVertexCount() * sizeof(Vertex), sizeof(Vertex));
			commandEncoder->bindVertexBuffer(1, rigidBodyBuffer, 0, sizeof(RigidBody));
			commandEncoder->bindIndexBuffer<uint32>(indexBuffer, scene->getStaticIndexCount() * sizeof(uint32));
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});

			while (componentIndex < geometryComponentPool.componentCount) {
				const Entity entity = geometryComponentPool.componentIndexToEntity[componentIndex];
				const GeometryComponent geometryComponent = geometryComponentPool.components[componentIndex];
				const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];
				const Hull2D& hull = hulls[geometry.hullIndex];

				// TODO: Restore materials (test colors were 0xf4fedbff and 0xe6ddffff).
				commandEncoder->drawIndexed(hull.getLineIndexRange().count, 1, hull.getLineIndexRange().offset, 0, entity);

				componentIndex += 1;
			}
		}

		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();

		// Center of mass
		{
			updateCentersOfMass();

			commandEncoder->bindGraphicsPipeline(centerOfMassPipeline);
			commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, 0, sizeof(CenterOfMass));
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, {cameraViewDescriptorSet});
			commandEncoder->draw(rigidBodyComponentPool.componentCount, 1, 0, 0);
		}

		/* #if defined(SHOW_DISTANCE)
			renderDistance(commandEncoder);
		#endif

		#if defined(SHOW_FEATURES)
			renderFeatures(commandEncoder);
		#endif */

		if (selectedEntity != max<Entity>()) {
			const GeometryComponent geometryComponent = scene->ecs.getComponent<GeometryComponent>(selectedEntity);
			const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];
			const Hull2D& hull = hulls[geometry.hullIndex];

			// Rigid body placeholder center of mass
			{
				commandEncoder->bindGraphicsPipeline(rigidBodyPlaceholderCenterOfMassPipeline);
				commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, 0, sizeof(CenterOfMass));
				commandEncoder->bindDescriptorSets(placeholderPipelineLayout, 0, {cameraViewDescriptorSet, placeholderDescriptorSet});
				commandEncoder->draw(1, 1, selectedEntity, 0);
			}

			// Rigid body placeholder fill
			{
				commandEncoder->bindGraphicsPipeline(rigidBodyPlaceholderFillPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, scene->getStaticVertexCount() * sizeof(Vertex), sizeof(Vertex));
				commandEncoder->bindVertexBuffer(1, rigidBodyBuffer, 0, sizeof(RigidBody));
				commandEncoder->bindIndexBuffer<uint32>(indexBuffer, scene->getStaticIndexCount() * sizeof(uint32));
				commandEncoder->bindDescriptorSets(placeholderPipelineLayout, 0, {cameraViewDescriptorSet, placeholderDescriptorSet});
				commandEncoder->drawIndexed(geometry.getIndexRange().count, 1, geometry.getIndexRange().offset, geometry.getVertexPositionComponentRange().offset / 2, selectedEntity);
			}

			// Rigid body placeholder stroke
			{
				commandEncoder->bindGraphicsPipeline(rigidBodyPlaceholderStrokePipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, scene->getStaticVertexCount() * sizeof(Vertex), sizeof(Vertex));
				commandEncoder->bindVertexBuffer(1, rigidBodyBuffer, 0, sizeof(RigidBody));
				commandEncoder->bindIndexBuffer<uint32>(indexBuffer, scene->getStaticIndexCount() * sizeof(uint32));
				commandEncoder->bindDescriptorSets(placeholderPipelineLayout, 0, {cameraViewDescriptorSet, placeholderDescriptorSet});
				commandEncoder->drawIndexed(hull.getLineIndexRange().count, 1, hull.getLineIndexRange().offset, 0, selectedEntity);
			}
		}

		commandEncoder->endRenderPass();
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
		#if PHYSICS_2D_RENDERER_SAMPLE_COUNT > 1
			if (layerInput->swapchainImage == nullptr) {
				recreateMultisampleImage();
				recreateMultisampleImageView();
			}
		#endif

		recreateDepthStencilImage();
		recreateDepthStencilImageView();
		recreateFramebuffers();

		updateCameraProjection();
		updateViewport();
	}

	void Physics2DRenderer::clearRegisteredData() {
		#if defined(SHOW_DISTANCE)
			closestPointMappedRange[0].position = {0.0f, 0.0f};
			closestPointMappedRange[1].position = {0.0f, 0.0f};
		#endif

		#if defined(SHOW_FEATURES)
			vertexFeatureCount = 0;
			edgeFeatureCount = 0;
		#endif
	}
}