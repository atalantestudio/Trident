#include "pch.hpp"

#include "Engine/API/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/3D/VisibilityRenderer.hpp"

namespace ProjectA {
	void VisibilityRenderer::build() {
		setupRenderPass();

		recreateVisibilityImage();
		recreateVisibilityImageView();
		recreateDepthStencilImage();
		recreateDepthStencilImageView();
		recreateFramebuffers();

		createCameraDescriptorSetLayout();
		createClusterDescriptorSetLayout();
		createInstanceDescriptorSetLayout();
		createDebugDescriptorSetLayout();

		allocateDescriptorSets();

		createVisibilityPipelineLayout();
		createDebugPipelineLayout();

		createVisibilityRenderPipeline();
		createDebugRenderPipeline();

		createIndirectBuffer();
		createCameraBuffer();

		updateViewport();
	}

	void VisibilityRenderer::postSetScene() {
		createVertexPositionComponentBuffer();
		createVertexNormalComponentBuffer();
		createVertexTextureCoordinateComponentBuffer();
		createVertexPositionIndexBuffer();
		createVertexNormalIndexBuffer();
		createVertexTextureCoordinateIndexBuffer();
		createClusterBuffer();
		createInstanceBuffer();
		createGeometryBuffer();

		fillSceneBuffers();

		updateBufferDescriptorSets();
		updateDebugInputAttachmentDescriptorSets();
	}

	void VisibilityRenderer::postSetCameraController() {
		const PerspectiveCamera& camera = getCameraController<PerspectiveCamera>().getCamera();

		cameraMappedRange->nearPlane = camera.getNearPlane();
		cameraMappedRange->farPlane = camera.getFarPlane();
	}

	void VisibilityRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		const ClearValue swapchainClearValue = {0.0f, 0.0f, 0.0f, 1.0f};
		const ClearValue visibilityClearValue = {0.0f, 0.0f, 0.0f, 1.0f};

		ClearValue depthStencilClearValue{};
		depthStencilClearValue.depth = 1.0f;
		depthStencilClearValue.stencil = 0;

		BeginRenderPassDescriptor beginRenderPassDescriptor{};
		beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
		beginRenderPassDescriptor.clearValues = {swapchainClearValue, visibilityClearValue, depthStencilClearValue};

		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(swapchainExtent.width);
		const float32 height = static_cast<float32>(swapchainExtent.height);

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchainExtent);

		// Visibility subpass
		{
			commandEncoder->bindRenderPipeline(visibilityRenderPipeline);
			commandEncoder->bindDescriptorSets(visibilityPipelineLayout, {cameraDescriptorSet, clusterDescriptorSet, instanceDescriptorSet});
			commandEncoder->drawIndirect(indirectBuffer, 0, 1, 0);
		}

		commandEncoder->nextSubpass();

		// Debug subpass
		{
			commandEncoder->bindRenderPipeline(debugRenderPipeline);
			commandEncoder->bindDescriptorSets(debugPipelineLayout, {debugDescriptorSet, clusterDescriptorSet, instanceDescriptorSet});
			commandEncoder->draw(3, 1, 0, 0);
		}

		commandEncoder->endRenderPass();
	}

	void VisibilityRenderer::resize(WindowResizeEvent& event) {
		recreateVisibilityImage();
		recreateVisibilityImageView();
		recreateDepthStencilImage();
		recreateDepthStencilImageView();
		recreateFramebuffers();

		updateDebugInputAttachmentDescriptorSets();
	}

	void VisibilityRenderer::updateCameraPosition() const {
		cameraMappedRange->position = getCameraController<PerspectiveCamera>().getCamera().getPosition();
	}

	void VisibilityRenderer::updateCameraProjectionView() const {
		cameraMappedRange->projectionView = getCameraController<PerspectiveCamera>().getCamera().getProjectionView();
	}

	void VisibilityRenderer::updateViewport() const {
		cameraMappedRange->viewport = getSwapchain()->getExtent();
	}

	void VisibilityRenderer::updateInstanceTransform(Entity entity, const mat4x4<float32>& transform) {
		instanceMappedRange[entity].transform = transform;
	}

	void VisibilityRenderer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	void VisibilityRenderer::setupRenderPass() {
		Attachment swapchainAttachment{};
		swapchainAttachment.format = getSurface()->getFormat();
		swapchainAttachment.sampleCount = 1;
		swapchainAttachment.loadOperation = LoadOperation::CLEAR;
		swapchainAttachment.storeOperation = StoreOperation::STORE;
		swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

		Attachment visibilityAttachment{};
		visibilityAttachment.format = ImageFormat::R32_UINT;
		visibilityAttachment.sampleCount = 1;
		visibilityAttachment.loadOperation = LoadOperation::CLEAR;
		visibilityAttachment.storeOperation = StoreOperation::UNDEFINED;
		visibilityAttachment.finalLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		Attachment depthStencilAttachment{};
		depthStencilAttachment.format = ImageFormat::D32_SFLOAT;
		depthStencilAttachment.sampleCount = 1;
		depthStencilAttachment.loadOperation = LoadOperation::CLEAR;
		depthStencilAttachment.storeOperation = StoreOperation::UNDEFINED;
		depthStencilAttachment.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		AttachmentReference writeSwapchainAttachmentReference{};
		writeSwapchainAttachmentReference.index = 0;
		writeSwapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		AttachmentReference writeVisibilityAttachmentReference{};
		writeVisibilityAttachmentReference.index = 1;
		writeVisibilityAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		AttachmentReference writeDepthStencilAttachmentReference{};
		writeDepthStencilAttachmentReference.index = 2;
		writeDepthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
 
		AttachmentReference readVisibilityAttachmentReference{};
		readVisibilityAttachmentReference.index = 1;
		readVisibilityAttachmentReference.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

		AttachmentReference readDepthStencilAttachmentReference{};
		readDepthStencilAttachmentReference.index = 2;
		readDepthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		SubpassDescriptor visibilitySubpassDescriptor{};
		visibilitySubpassDescriptor.colorAttachmentReferences = {writeVisibilityAttachmentReference};
		visibilitySubpassDescriptor.depthStencilAttachmentReference = writeDepthStencilAttachmentReference;

		SubpassDescriptor debugSubpassDescriptor{};
		debugSubpassDescriptor.colorAttachmentReferences = {writeSwapchainAttachmentReference};
		debugSubpassDescriptor.inputAttachmentReferences = {readVisibilityAttachmentReference, readDepthStencilAttachmentReference};

		const ref<Subpass> visibilitySubpass = createSubpass(visibilitySubpassDescriptor);
		const ref<Subpass> debugSubpass = createSubpass(debugSubpassDescriptor);

		SubpassDependency subpassDependency{};
		subpassDependency.sourceSubpassIndex = SubpassDependency::EXTERNAL_SUBPASS;
		subpassDependency.destinationSubpassIndex = 0;

		RenderPassDescriptor renderPassDescriptor{};
		renderPassDescriptor.attachments = {swapchainAttachment, visibilityAttachment, depthStencilAttachment};
		renderPassDescriptor.subpasses = {visibilitySubpass, debugSubpass};
		renderPassDescriptor.subpassDependencies = {subpassDependency};

		renderPass = createRenderPass(renderPassDescriptor);
	}

	void VisibilityRenderer::recreateVisibilityImage() {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		AttachmentImageDescriptor visibilityAttachmentImageDescriptor{};
		visibilityAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		visibilityAttachmentImageDescriptor.extent.width = swapchainExtent.width;
		visibilityAttachmentImageDescriptor.extent.height = swapchainExtent.height;
		visibilityAttachmentImageDescriptor.extent.depth = 1;
		visibilityAttachmentImageDescriptor.mipLevelCount = 1;
		visibilityAttachmentImageDescriptor.layerCount = 1;

		visibilityImage = renderPass->createInputAttachmentImage(visibilityAttachmentImageDescriptor, 1);
	}

	void VisibilityRenderer::recreateVisibilityImageView() {
		ImageViewDescriptor visibilityImageViewDescriptor{};
		visibilityImageViewDescriptor.mipLevelOffset = 0;
		visibilityImageViewDescriptor.mipLevelCount = visibilityImage->getMipLevelCount();
		visibilityImageViewDescriptor.layerOffset = 0;
		visibilityImageViewDescriptor.layerCount = visibilityImage->getLayerCount();

		visibilityImageView = visibilityImage->createView(visibilityImageViewDescriptor);
	}

	void VisibilityRenderer::recreateDepthStencilImage() {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		AttachmentImageDescriptor depthStencilAttachmentImageDescriptor{};
		depthStencilAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		depthStencilAttachmentImageDescriptor.extent.width = swapchainExtent.width;
		depthStencilAttachmentImageDescriptor.extent.height = swapchainExtent.height;
		depthStencilAttachmentImageDescriptor.extent.depth = 1;
		depthStencilAttachmentImageDescriptor.mipLevelCount = 1;
		depthStencilAttachmentImageDescriptor.layerCount = 1;

		depthStencilImage = renderPass->createDepthStencilAttachmentImage(depthStencilAttachmentImageDescriptor, VISIBILITY_SUBPASS_INDEX);
	}

	void VisibilityRenderer::recreateDepthStencilImageView() {
		ImageViewDescriptor depthStencilImageViewDescriptor{};
		depthStencilImageViewDescriptor.mipLevelOffset = 0;
		depthStencilImageViewDescriptor.mipLevelCount = depthStencilImage->getMipLevelCount();
		depthStencilImageViewDescriptor.layerOffset = 0;
		depthStencilImageViewDescriptor.layerCount = depthStencilImage->getLayerCount();

		depthStencilImageView = depthStencilImage->createView(depthStencilImageViewDescriptor);
	}

	void VisibilityRenderer::recreateFramebuffers() {
		const ref<Swapchain>& swapchain = getSwapchain();
		const Extent2D swapchainExtent = swapchain->getExtent();

		for (ref<Framebuffer>& framebuffer : framebuffers) {
			framebuffer.~shared_ptr();
		}

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor{};
		framebufferDescriptor.renderPass = renderPass;
		framebufferDescriptor.imageViews = {nullptr, visibilityImageView, depthStencilImageView};
		framebufferDescriptor.extent = swapchain->getExtent();
		framebufferDescriptor.layerCount = 1;

		for (uint32 framebufferIndex = 0; framebufferIndex < framebuffers.count; framebufferIndex += 1) {
			framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
		}
	}

	void VisibilityRenderer::createCameraDescriptorSetLayout() {
		DescriptorSetLayoutBinding cameraBinding{};
		cameraBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraBinding.descriptorCount = 1;
		cameraBinding.shaderStages = ShaderStage::VERTEX;

		cameraDescriptorSetLayout = createDescriptorSetLayout({cameraBinding});
	}

	void VisibilityRenderer::createClusterDescriptorSetLayout() {
		DescriptorSetLayoutBinding vertexPositionComponentBinding{};
		vertexPositionComponentBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		vertexPositionComponentBinding.descriptorCount = 1;
		vertexPositionComponentBinding.shaderStages = ShaderStage::VERTEX;

		DescriptorSetLayoutBinding vertexBinding{};
		vertexBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		vertexBinding.descriptorCount = 1;
		vertexBinding.shaderStages = ShaderStage::VERTEX;

		DescriptorSetLayoutBinding clusterBinding{};
		clusterBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		clusterBinding.descriptorCount = 1;
		clusterBinding.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT;

		clusterDescriptorSetLayout = createDescriptorSetLayout({vertexPositionComponentBinding, vertexBinding, clusterBinding});
	}

	void VisibilityRenderer::createInstanceDescriptorSetLayout() {
		DescriptorSetLayoutBinding instanceBinding{};
		instanceBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		instanceBinding.descriptorCount = 1;
		instanceBinding.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding geometryBinding{};
		geometryBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		geometryBinding.descriptorCount = 1;
		geometryBinding.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT;

		instanceDescriptorSetLayout = createDescriptorSetLayout({instanceBinding, geometryBinding});
	}

	void VisibilityRenderer::createDebugDescriptorSetLayout() {
		DescriptorSetLayoutBinding cameraBinding{};
		cameraBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraBinding.descriptorCount = 1;
		cameraBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding visibilityBinding{};
		visibilityBinding.descriptorType = DescriptorType::INPUT_ATTACHMENT;
		visibilityBinding.descriptorCount = 1;
		visibilityBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding depthStencilBinding{};
		depthStencilBinding.descriptorType = DescriptorType::INPUT_ATTACHMENT;
		depthStencilBinding.descriptorCount = 1;
		depthStencilBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexPositionComponentBinding{};
		vertexPositionComponentBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		vertexPositionComponentBinding.descriptorCount = 1;
		vertexPositionComponentBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexNormalComponentBinding{};
		vertexNormalComponentBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		vertexNormalComponentBinding.descriptorCount = 1;
		vertexNormalComponentBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexTextureCoordinateComponentBinding{};
		vertexTextureCoordinateComponentBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		vertexTextureCoordinateComponentBinding.descriptorCount = 1;
		vertexTextureCoordinateComponentBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexPositionIndexBinding{};
		vertexPositionIndexBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		vertexPositionIndexBinding.descriptorCount = 1;
		vertexPositionIndexBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexNormalIndexBinding{};
		vertexNormalIndexBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		vertexNormalIndexBinding.descriptorCount = 1;
		vertexNormalIndexBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexTextureCoordinateIndexBinding{};
		vertexTextureCoordinateIndexBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		vertexTextureCoordinateIndexBinding.descriptorCount = 1;
		vertexTextureCoordinateIndexBinding.shaderStages = ShaderStage::FRAGMENT;

		debugDescriptorSetLayout = createDescriptorSetLayout({
			cameraBinding,
			visibilityBinding,
			depthStencilBinding,
			vertexPositionComponentBinding,
			vertexNormalComponentBinding,
			vertexTextureCoordinateComponentBinding,
			vertexPositionIndexBinding,
			vertexNormalIndexBinding,
			vertexTextureCoordinateIndexBinding,
		});
	}

	void VisibilityRenderer::createVisibilityPipelineLayout() {
		PipelineLayoutDescriptor visibilityPipelineLayoutDescriptor{};
		visibilityPipelineLayoutDescriptor.descriptorSetLayouts = {cameraDescriptorSetLayout, clusterDescriptorSetLayout, instanceDescriptorSetLayout};

		visibilityPipelineLayout = createPipelineLayout(visibilityPipelineLayoutDescriptor);
	}

	void VisibilityRenderer::createDebugPipelineLayout() {
		PipelineLayoutDescriptor debugPipelineLayoutDescriptor{};
		debugPipelineLayoutDescriptor.descriptorSetLayouts = {debugDescriptorSetLayout, clusterDescriptorSetLayout, instanceDescriptorSetLayout};

		debugPipelineLayout = createPipelineLayout(debugPipelineLayoutDescriptor);
	}

	void VisibilityRenderer::createIndirectBuffer() {
		BufferDescriptor indirectBufferDescriptor{};
		indirectBufferDescriptor.size = sizeof(DrawIndirectCommand);
		indirectBufferDescriptor.usage = BufferUsage::INDIRECT;
		indirectBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		indirectBufferDescriptor.persistent = true;

		indirectBuffer = createBuffer(indirectBufferDescriptor);

		indirectMappedRange = indirectBuffer->map<DrawIndirectCommand>(0, indirectBuffer->size);
	}

	void VisibilityRenderer::createCameraBuffer() {
		BufferDescriptor cameraBufferDescriptor{};
		cameraBufferDescriptor.size = sizeof(Camera);
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		cameraBufferDescriptor.persistent = true;

		cameraBuffer = createBuffer(cameraBufferDescriptor);

		cameraMappedRange = cameraBuffer->map<Camera>(0, cameraBuffer->size);
	}

	void VisibilityRenderer::createVertexPositionComponentBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexPositionComponentBufferDescriptor{};
		vertexPositionComponentBufferDescriptor.size = scene->vertexPositionComponents.count * sizeof(float32);
		vertexPositionComponentBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		vertexPositionComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexPositionComponentBuffer = createBuffer(vertexPositionComponentBufferDescriptor);
	}

	void VisibilityRenderer::createVertexNormalComponentBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexNormalComponentBufferDescriptor{};
		vertexNormalComponentBufferDescriptor.size = scene->vertexNormalComponents.count * sizeof(float32);
		vertexNormalComponentBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		vertexNormalComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexNormalComponentBuffer = createBuffer(vertexNormalComponentBufferDescriptor);
	}

	void VisibilityRenderer::createVertexTextureCoordinateComponentBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexTextureCoordinateComponentBufferDescriptor{};
		vertexTextureCoordinateComponentBufferDescriptor.size = scene->vertexTextureCoordinateComponents.count * sizeof(float32);
		vertexTextureCoordinateComponentBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		vertexTextureCoordinateComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexTextureCoordinateComponentBuffer = createBuffer(vertexTextureCoordinateComponentBufferDescriptor);
	}

	void VisibilityRenderer::createVertexPositionIndexBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexPositionIndexBufferDescriptor{};
		vertexPositionIndexBufferDescriptor.size = scene->vertexPositionIndices.count * sizeof(uint32);
		vertexPositionIndexBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		vertexPositionIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexPositionIndexBuffer = createBuffer(vertexPositionIndexBufferDescriptor);
	}

	void VisibilityRenderer::createVertexNormalIndexBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexNormalIndexBufferDescriptor{};
		vertexNormalIndexBufferDescriptor.size = scene->vertexNormalIndices.count * sizeof(uint32);
		vertexNormalIndexBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		vertexNormalIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexNormalIndexBuffer = createBuffer(vertexNormalIndexBufferDescriptor);
	}

	void VisibilityRenderer::createVertexTextureCoordinateIndexBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexTextureCoordinateIndexBufferDescriptor{};
		vertexTextureCoordinateIndexBufferDescriptor.size = scene->vertexTextureCoordinateIndices.count * sizeof(uint32);
		vertexTextureCoordinateIndexBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		vertexTextureCoordinateIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexTextureCoordinateIndexBuffer = createBuffer(vertexTextureCoordinateIndexBufferDescriptor);
	}

	void VisibilityRenderer::createClusterBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor clusterBufferDescriptor{};
		clusterBufferDescriptor.size = scene->clusters.count * sizeof(Cluster);
		clusterBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		clusterBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		clusterBuffer = createBuffer(clusterBufferDescriptor);
	}

	void VisibilityRenderer::createInstanceBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor instanceBufferDescriptor{};
		instanceBufferDescriptor.size = scene->instances.count * sizeof(Instance);
		instanceBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		instanceBufferDescriptor.persistent = true;

		instanceBuffer = createBuffer(instanceBufferDescriptor);

		instanceMappedRange = instanceBuffer->map<Instance>(0, instanceBuffer->size);
	}

	void VisibilityRenderer::createGeometryBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor geometryBufferDescriptor{};
		geometryBufferDescriptor.size = scene->geometries.count * sizeof(Geometry);
		geometryBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		geometryBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		geometryBuffer = createBuffer(geometryBufferDescriptor);
	}

	void VisibilityRenderer::createVisibilityRenderPipeline() {
		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		ref<ShaderModule> vertexShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::OPENGL:
				vertexShaderModule = loadShaderModule("Visibility/VisibilityGL.vert", vertexShaderModuleDescriptor);

				break;
			case GraphicsAPI::VULKAN:
				vertexShaderModule = loadShaderModule("Visibility/VisibilityVK.vert", vertexShaderModuleDescriptor);

				break;
			default:
				ENGINE_ERROR_UNSUPPORTED();

				break;
		}

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Visibility/Visibility.frag", fragmentShaderModuleDescriptor);

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor visibilityRenderPipelineDescriptor{};
		visibilityRenderPipelineDescriptor.pipelineLayout = visibilityPipelineLayout;
		visibilityRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		visibilityRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		visibilityRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		visibilityRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		visibilityRenderPipelineDescriptor.cullMode = CullMode::BACK;
		visibilityRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		visibilityRenderPipelineDescriptor.lineWidth = 1.0f;
		visibilityRenderPipelineDescriptor.minSampleShading = 0.0f;
		visibilityRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		visibilityRenderPipelineDescriptor.depthStencilState.depthTest = true;
		visibilityRenderPipelineDescriptor.depthStencilState.depthWrite = true;
		visibilityRenderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::LESS;
		visibilityRenderPipelineDescriptor.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT;
		visibilityRenderPipelineDescriptor.renderPass = renderPass;
		visibilityRenderPipelineDescriptor.subpassIndex = VISIBILITY_SUBPASS_INDEX;

		visibilityRenderPipeline = createRenderPipeline(visibilityRenderPipelineDescriptor);
	}

	void VisibilityRenderer::createDebugRenderPipeline() {
		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		ref<ShaderModule> vertexShaderModule;
		ref<ShaderModule> fragmentShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::OPENGL:
				vertexShaderModule = loadShaderModule("Base/ScreenUV.vert", vertexShaderModuleDescriptor);
				fragmentShaderModule = loadShaderModule("Visibility/VisibilityDebugGL.frag", fragmentShaderModuleDescriptor);

				break;
			case GraphicsAPI::VULKAN:
				vertexShaderModule = loadShaderModule("Base/Screen.vert", vertexShaderModuleDescriptor);
				fragmentShaderModule = loadShaderModule("Visibility/VisibilityDebugVK.frag", fragmentShaderModuleDescriptor);

				break;
			default:
				ENGINE_ERROR_UNSUPPORTED();

				break;
		}

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor debugRenderPipelineDescriptor{};
		debugRenderPipelineDescriptor.pipelineLayout = debugPipelineLayout;
		debugRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		debugRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		debugRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		debugRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		debugRenderPipelineDescriptor.cullMode = CullMode::BACK;
		debugRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		debugRenderPipelineDescriptor.lineWidth = 1.0f;
		debugRenderPipelineDescriptor.minSampleShading = 0.0f;
		debugRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		debugRenderPipelineDescriptor.depthStencilState.depthTest = false;
		debugRenderPipelineDescriptor.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT;
		debugRenderPipelineDescriptor.renderPass = renderPass;
		debugRenderPipelineDescriptor.subpassIndex = DEBUG_SUBPASS_INDEX;

		debugRenderPipeline = createRenderPipeline(debugRenderPipelineDescriptor);
	}

	void VisibilityRenderer::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({
			cameraDescriptorSetLayout,
			clusterDescriptorSetLayout,
			instanceDescriptorSetLayout,
			debugDescriptorSetLayout,
		});

		cameraDescriptorSet = descriptorSets[0];
		clusterDescriptorSet = descriptorSets[1];
		instanceDescriptorSet = descriptorSets[2];
		debugDescriptorSet = descriptorSets[3];
	}

	void VisibilityRenderer::fillSceneBuffers() const {
		fillIndirectBuffer();

		const ref<Buffer> stagingBuffer = createStagingBuffer(
			vertexPositionComponentBuffer->size +
			vertexNormalComponentBuffer->size +
			vertexTextureCoordinateComponentBuffer->size +
			vertexPositionIndexBuffer->size +
			vertexNormalIndexBuffer->size +
			vertexTextureCoordinateIndexBuffer->size +
			clusterBuffer->size +
			instanceBuffer->size +
			geometryBuffer->size
		);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->size);
		uint64 sourceOffset = 0;

		float32* vertexPositionComponentMappedRange = pointer_cast<float32*>(mappedRange + sourceOffset);

		fillVertexPositionComponentBuffer(vertexPositionComponentMappedRange);

		BufferCopyRegion vertexPositionComponentBufferCopyRegion{};
		vertexPositionComponentBufferCopyRegion.sourceOffset = sourceOffset;
		vertexPositionComponentBufferCopyRegion.destinationOffset = 0;
		vertexPositionComponentBufferCopyRegion.size = vertexPositionComponentBuffer->size;

		sourceOffset += vertexPositionComponentBuffer->size;

		float32* vertexNormalComponentMappedRange = pointer_cast<float32*>(mappedRange + sourceOffset);

		fillVertexNormalComponentBuffer(vertexNormalComponentMappedRange);

		BufferCopyRegion vertexNormalComponentBufferCopyRegion{};
		vertexNormalComponentBufferCopyRegion.sourceOffset = sourceOffset;
		vertexNormalComponentBufferCopyRegion.destinationOffset = 0;
		vertexNormalComponentBufferCopyRegion.size = vertexNormalComponentBuffer->size;

		sourceOffset += vertexNormalComponentBuffer->size;

		float32* vertexTextureCoordinateComponentMappedRange = pointer_cast<float32*>(mappedRange + sourceOffset);

		fillVertexTextureCoordinateComponentBuffer(vertexTextureCoordinateComponentMappedRange);

		BufferCopyRegion vertexTextureCoordinateComponentBufferCopyRegion{};
		vertexTextureCoordinateComponentBufferCopyRegion.sourceOffset = sourceOffset;
		vertexTextureCoordinateComponentBufferCopyRegion.destinationOffset = 0;
		vertexTextureCoordinateComponentBufferCopyRegion.size = vertexTextureCoordinateComponentBuffer->size;

		sourceOffset += vertexTextureCoordinateComponentBuffer->size;

		uint32* vertexPositionIndexMappedRange = pointer_cast<uint32*>(mappedRange + sourceOffset);

		fillVertexPositionIndexBuffer(vertexPositionIndexMappedRange);

		BufferCopyRegion vertexPositionIndexBufferCopyRegion{};
		vertexPositionIndexBufferCopyRegion.sourceOffset = sourceOffset;
		vertexPositionIndexBufferCopyRegion.destinationOffset = 0;
		vertexPositionIndexBufferCopyRegion.size = vertexPositionIndexBuffer->size;

		sourceOffset += vertexPositionIndexBuffer->size;

		uint32* vertexNormalIndexMappedRange = pointer_cast<uint32*>(mappedRange + sourceOffset);

		fillVertexNormalIndexBuffer(vertexNormalIndexMappedRange);

		BufferCopyRegion vertexNormalIndexBufferCopyRegion{};
		vertexNormalIndexBufferCopyRegion.sourceOffset = sourceOffset;
		vertexNormalIndexBufferCopyRegion.destinationOffset = 0;
		vertexNormalIndexBufferCopyRegion.size = vertexNormalIndexBuffer->size;

		sourceOffset += vertexNormalIndexBuffer->size;

		uint32* vertexTextureCoordinateIndexMappedRange = pointer_cast<uint32*>(mappedRange + sourceOffset);

		fillVertexTextureCoordinateIndexBuffer(vertexTextureCoordinateIndexMappedRange);

		BufferCopyRegion vertexTextureCoordinateIndexBufferCopyRegion{};
		vertexTextureCoordinateIndexBufferCopyRegion.sourceOffset = sourceOffset;
		vertexTextureCoordinateIndexBufferCopyRegion.destinationOffset = 0;
		vertexTextureCoordinateIndexBufferCopyRegion.size = vertexTextureCoordinateIndexBuffer->size;

		sourceOffset += vertexTextureCoordinateIndexBuffer->size;

		Cluster* clusterMappedRange = pointer_cast<Cluster*>(mappedRange + sourceOffset);

		fillClusterBuffer(clusterMappedRange);

		BufferCopyRegion clusterBufferCopyRegion{};
		clusterBufferCopyRegion.sourceOffset = sourceOffset;
		clusterBufferCopyRegion.destinationOffset = 0;
		clusterBufferCopyRegion.size = clusterBuffer->size;

		sourceOffset += clusterBuffer->size;

		Instance* instanceMappedRange = pointer_cast<Instance*>(mappedRange + sourceOffset);

		fillInstanceBuffer(instanceMappedRange);

		BufferCopyRegion instanceBufferCopyRegion{};
		instanceBufferCopyRegion.sourceOffset = sourceOffset;
		instanceBufferCopyRegion.destinationOffset = 0;
		instanceBufferCopyRegion.size = instanceBuffer->size;

		sourceOffset += instanceBuffer->size;

		Geometry* geometryMappedRange = pointer_cast<Geometry*>(mappedRange + sourceOffset);

		fillGeometryBuffer(geometryMappedRange);

		BufferCopyRegion geometryBufferCopyRegion{};
		geometryBufferCopyRegion.sourceOffset = sourceOffset;
		geometryBufferCopyRegion.destinationOffset = 0;
		geometryBufferCopyRegion.size = geometryBuffer->size;

		stagingBuffer->unmap();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexPositionComponentBuffer, {vertexPositionComponentBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexNormalComponentBuffer, {vertexNormalComponentBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexTextureCoordinateComponentBuffer, {vertexTextureCoordinateComponentBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexPositionIndexBuffer, {vertexPositionIndexBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexNormalIndexBuffer, {vertexNormalIndexBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexTextureCoordinateIndexBuffer, {vertexTextureCoordinateIndexBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, clusterBuffer, {clusterBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, {instanceBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, geometryBuffer, {geometryBufferCopyRegion});

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		getDevice()->waitIdle();
	}

	void VisibilityRenderer::fillIndirectBuffer() const {
		indirectMappedRange[0].vertexCount = CLUSTER_INDEX_COUNT;
		indirectMappedRange[0].instanceCount = static_cast<uint32>(scene->clusters.count);
		indirectMappedRange[0].vertexOffset = 0;
		indirectMappedRange[0].instanceOffset = 0;
	}

	void VisibilityRenderer::fillVertexPositionComponentBuffer(float32* mappedRange) const {
		std::copy(scene->vertexPositionComponents.begin(), scene->vertexPositionComponents.end(), mappedRange);
	}

	void VisibilityRenderer::fillVertexNormalComponentBuffer(float32* mappedRange) const {
		std::copy(scene->vertexNormalComponents.begin(), scene->vertexNormalComponents.end(), mappedRange);
	}

	void VisibilityRenderer::fillVertexTextureCoordinateComponentBuffer(float32* mappedRange) const {
		std::copy(scene->vertexTextureCoordinateComponents.begin(), scene->vertexTextureCoordinateComponents.end(), mappedRange);
	}

	void VisibilityRenderer::fillVertexPositionIndexBuffer(uint32* mappedRange) const {
		std::copy(scene->vertexPositionIndices.begin(), scene->vertexPositionIndices.end(), mappedRange);
	}

	void VisibilityRenderer::fillVertexNormalIndexBuffer(uint32* mappedRange) const {
		std::copy(scene->vertexNormalIndices.begin(), scene->vertexNormalIndices.end(), mappedRange);
	}

	void VisibilityRenderer::fillVertexTextureCoordinateIndexBuffer(uint32* mappedRange) const {
		std::copy(scene->vertexTextureCoordinateIndices.begin(), scene->vertexTextureCoordinateIndices.end(), mappedRange);
	}

	void VisibilityRenderer::fillClusterBuffer(Cluster* mappedRange) const {
		for (uint32 clusterIndex = 0; clusterIndex < scene->clusters.count; clusterIndex += 1) {
			mappedRange[clusterIndex].instanceIndex = scene->clusters[clusterIndex].instanceIndex;
		}
	}

	void VisibilityRenderer::fillInstanceBuffer(Instance* mappedRange) const {
		for (uint32 instanceIndex = 0; instanceIndex < scene->instances.count; instanceIndex += 1) {
			const auto& instance = scene->instances[instanceIndex];
			const auto& geometry = scene->geometries[instance.geometryIndex];

			mappedRange[instanceIndex].transform = instance.transform;
			mappedRange[instanceIndex].geometryIndex = instance.geometryIndex;
			mappedRange[instanceIndex].clusterOffset = instance.clusterOffset;
		}
	}

	void VisibilityRenderer::fillGeometryBuffer(Geometry* mappedRange) const {
		for (uint32 geometryIndex = 0; geometryIndex < scene->geometries.count; geometryIndex += 1) {
			mappedRange[geometryIndex].vertexOffset = scene->geometries[geometryIndex].vertexOffset;
			mappedRange[geometryIndex].clusterCount = scene->geometries[geometryIndex].clusterCount;
		}
	}

	void VisibilityRenderer::updateBufferDescriptorSets() {
		DescriptorBuffer cameraDescriptorBuffer{};
		cameraDescriptorBuffer.buffer = cameraBuffer;
		cameraDescriptorBuffer.offset = 0;
		cameraDescriptorBuffer.size = cameraBuffer->size;

		DescriptorBuffer clusterDescriptorBuffer{};
		clusterDescriptorBuffer.buffer = clusterBuffer;
		clusterDescriptorBuffer.offset = 0;
		clusterDescriptorBuffer.size = clusterBuffer->size;

		DescriptorBuffer instanceDescriptorBuffer{};
		instanceDescriptorBuffer.buffer = instanceBuffer;
		instanceDescriptorBuffer.offset = 0;
		instanceDescriptorBuffer.size = instanceBuffer->size;

		DescriptorBuffer geometryDescriptorBuffer{};
		geometryDescriptorBuffer.buffer = geometryBuffer;
		geometryDescriptorBuffer.offset = 0;
		geometryDescriptorBuffer.size = geometryBuffer->size;

		DescriptorBuffer vertexPositionComponentDescriptorBuffer{};
		vertexPositionComponentDescriptorBuffer.buffer = vertexPositionComponentBuffer;
		vertexPositionComponentDescriptorBuffer.offset = 0;
		vertexPositionComponentDescriptorBuffer.size = vertexPositionComponentBuffer->size;

		DescriptorBuffer vertexNormalComponentDescriptorBuffer{};
		vertexNormalComponentDescriptorBuffer.buffer = vertexNormalComponentBuffer;
		vertexNormalComponentDescriptorBuffer.offset = 0;
		vertexNormalComponentDescriptorBuffer.size = vertexNormalComponentBuffer->size;

		DescriptorBuffer vertexTextureCoordinateComponentDescriptorBuffer{};
		vertexTextureCoordinateComponentDescriptorBuffer.buffer = vertexTextureCoordinateComponentBuffer;
		vertexTextureCoordinateComponentDescriptorBuffer.offset = 0;
		vertexTextureCoordinateComponentDescriptorBuffer.size = vertexTextureCoordinateComponentBuffer->size;

		DescriptorBuffer vertexPositionIndexDescriptorBuffer{};
		vertexPositionIndexDescriptorBuffer.buffer = vertexPositionIndexBuffer;
		vertexPositionIndexDescriptorBuffer.offset = 0;
		vertexPositionIndexDescriptorBuffer.size = vertexPositionIndexBuffer->size;

		DescriptorBuffer vertexNormalIndexDescriptorBuffer{};
		vertexNormalIndexDescriptorBuffer.buffer = vertexNormalIndexBuffer;
		vertexNormalIndexDescriptorBuffer.offset = 0;
		vertexNormalIndexDescriptorBuffer.size = vertexNormalIndexBuffer->size;

		DescriptorBuffer vertexTextureCoordinateIndexDescriptorBuffer{};
		vertexTextureCoordinateIndexDescriptorBuffer.buffer = vertexTextureCoordinateIndexBuffer;
		vertexTextureCoordinateIndexDescriptorBuffer.offset = 0;
		vertexTextureCoordinateIndexDescriptorBuffer.size = vertexTextureCoordinateIndexBuffer->size;

		WriteDescriptorSet cameraWriteDescriptorSet{};
		cameraWriteDescriptorSet.descriptorSet = cameraDescriptorSet;
		cameraWriteDescriptorSet.bindingIndex = 0;
		cameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet vertexPositionComponentWriteDescriptorSet{};
		vertexPositionComponentWriteDescriptorSet.descriptorSet = clusterDescriptorSet;
		vertexPositionComponentWriteDescriptorSet.bindingIndex = 0;
		vertexPositionComponentWriteDescriptorSet.descriptorBuffers = {vertexPositionComponentDescriptorBuffer};

		WriteDescriptorSet vertexPositionIndexWriteDescriptorSet{};
		vertexPositionIndexWriteDescriptorSet.descriptorSet = clusterDescriptorSet;
		vertexPositionIndexWriteDescriptorSet.bindingIndex = 1;
		vertexPositionIndexWriteDescriptorSet.descriptorBuffers = {vertexPositionIndexDescriptorBuffer};

		WriteDescriptorSet clusterWriteDescriptorSet{};
		clusterWriteDescriptorSet.descriptorSet = clusterDescriptorSet;
		clusterWriteDescriptorSet.bindingIndex = 2;
		clusterWriteDescriptorSet.descriptorBuffers = {clusterDescriptorBuffer};

		WriteDescriptorSet instanceWriteDescriptorSet{};
		instanceWriteDescriptorSet.descriptorSet = instanceDescriptorSet;
		instanceWriteDescriptorSet.bindingIndex = 0;
		instanceWriteDescriptorSet.descriptorBuffers = {instanceDescriptorBuffer};

		WriteDescriptorSet geometryWriteDescriptorSet{};
		geometryWriteDescriptorSet.descriptorSet = instanceDescriptorSet;
		geometryWriteDescriptorSet.bindingIndex = 1;
		geometryWriteDescriptorSet.descriptorBuffers = {geometryDescriptorBuffer};

		WriteDescriptorSet debugCameraWriteDescriptorSet{};
		debugCameraWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugCameraWriteDescriptorSet.bindingIndex = 0;
		debugCameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet debugVertexPositionComponentWriteDescriptorSet{};
		debugVertexPositionComponentWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexPositionComponentWriteDescriptorSet.bindingIndex = 3;
		debugVertexPositionComponentWriteDescriptorSet.descriptorBuffers = {vertexPositionComponentDescriptorBuffer};

		WriteDescriptorSet debugVertexNormalComponentWriteDescriptorSet{};
		debugVertexNormalComponentWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexNormalComponentWriteDescriptorSet.bindingIndex = 4;
		debugVertexNormalComponentWriteDescriptorSet.descriptorBuffers = {vertexNormalComponentDescriptorBuffer};

		WriteDescriptorSet debugVertexTextureCoordinateComponentWriteDescriptorSet{};
		debugVertexTextureCoordinateComponentWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexTextureCoordinateComponentWriteDescriptorSet.bindingIndex = 5;
		debugVertexTextureCoordinateComponentWriteDescriptorSet.descriptorBuffers = {vertexTextureCoordinateComponentDescriptorBuffer};

		WriteDescriptorSet debugVertexPositionIndexWriteDescriptorSet{};
		debugVertexPositionIndexWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexPositionIndexWriteDescriptorSet.bindingIndex = 6;
		debugVertexPositionIndexWriteDescriptorSet.descriptorBuffers = {vertexPositionIndexDescriptorBuffer};

		WriteDescriptorSet debugVertexNormalIndexWriteDescriptorSet{};
		debugVertexNormalIndexWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexNormalIndexWriteDescriptorSet.bindingIndex = 7;
		debugVertexNormalIndexWriteDescriptorSet.descriptorBuffers = {vertexNormalIndexDescriptorBuffer};

		WriteDescriptorSet debugVertexTextureCoordinateIndexWriteDescriptorSet{};
		debugVertexTextureCoordinateIndexWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVertexTextureCoordinateIndexWriteDescriptorSet.bindingIndex = 8;
		debugVertexTextureCoordinateIndexWriteDescriptorSet.descriptorBuffers = {vertexTextureCoordinateIndexDescriptorBuffer};

		getDevice()->updateDescriptorSets({
			cameraWriteDescriptorSet,
			vertexPositionComponentWriteDescriptorSet,
			vertexPositionIndexWriteDescriptorSet,
			clusterWriteDescriptorSet,
			instanceWriteDescriptorSet,
			geometryWriteDescriptorSet,
			debugCameraWriteDescriptorSet,
			debugVertexPositionComponentWriteDescriptorSet,
			debugVertexNormalComponentWriteDescriptorSet,
			debugVertexTextureCoordinateComponentWriteDescriptorSet,
			debugVertexPositionIndexWriteDescriptorSet,
			debugVertexNormalIndexWriteDescriptorSet,
			debugVertexTextureCoordinateIndexWriteDescriptorSet,
		});
	}

	void VisibilityRenderer::updateDebugInputAttachmentDescriptorSets() {
		DescriptorImage visibilityDescriptorImage{};
		visibilityDescriptorImage.imageView = visibilityImageView;
		visibilityDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
		visibilityDescriptorImage.sampler = nullptr;

		DescriptorImage depthStencilDescriptorImage{};
		depthStencilDescriptorImage.imageView = depthStencilImageView;
		depthStencilDescriptorImage.imageLayout = ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		depthStencilDescriptorImage.sampler = nullptr;

		WriteDescriptorSet debugVisibilityWriteDescriptorSet{};
		debugVisibilityWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugVisibilityWriteDescriptorSet.bindingIndex = 1;
		debugVisibilityWriteDescriptorSet.descriptorImages = {visibilityDescriptorImage};

		WriteDescriptorSet debugDepthStencilWriteDescriptorSet{};
		debugDepthStencilWriteDescriptorSet.descriptorSet = debugDescriptorSet;
		debugDepthStencilWriteDescriptorSet.bindingIndex = 2;
		debugDepthStencilWriteDescriptorSet.descriptorImages = {depthStencilDescriptorImage};

		getDevice()->updateDescriptorSets({
			debugVisibilityWriteDescriptorSet,
			debugDepthStencilWriteDescriptorSet,
		});
	}

	bool VisibilityRenderer::onWindowResize(WindowResizeEvent& event) {
		resize(event);

		return true;
	}
}