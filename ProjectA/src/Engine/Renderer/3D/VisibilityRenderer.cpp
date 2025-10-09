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
	}

	void VisibilityRenderer::postSetScene() {
		createVertexPositionComponentBuffer();
		createVertexBuffer();
		createClusterBuffer();
		createInstanceBuffer();
		createGeometryBuffer();

		fillSceneBuffers();

		updateBufferDescriptorSets();
		updateDebugDescriptorSet();
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
			commandEncoder->bindDescriptorSets(debugPipelineLayout, {debugDescriptorSet});
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

		updateDebugDescriptorSet();
	}

	void VisibilityRenderer::updateCameraProjectionView() {
		cameraMappedRange->projectionView = getCameraController<PerspectiveCamera>().getCamera().getProjectionView();
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
		clusterBinding.shaderStages = ShaderStage::VERTEX;

		clusterDescriptorSetLayout = createDescriptorSetLayout({vertexPositionComponentBinding, vertexBinding, clusterBinding});
	}

	void VisibilityRenderer::createInstanceDescriptorSetLayout() {
		DescriptorSetLayoutBinding instanceBinding{};
		instanceBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		instanceBinding.descriptorCount = 1;
		instanceBinding.shaderStages = ShaderStage::VERTEX;

		DescriptorSetLayoutBinding geometryBinding{};
		geometryBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		geometryBinding.descriptorCount = 1;
		geometryBinding.shaderStages = ShaderStage::VERTEX;

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

		debugDescriptorSetLayout = createDescriptorSetLayout({cameraBinding, visibilityBinding, depthStencilBinding});
	}

	void VisibilityRenderer::createVisibilityPipelineLayout() {
		PipelineLayoutDescriptor visibilityPipelineLayoutDescriptor{};
		visibilityPipelineLayoutDescriptor.descriptorSetLayouts = {cameraDescriptorSetLayout, clusterDescriptorSetLayout, instanceDescriptorSetLayout};

		visibilityPipelineLayout = createPipelineLayout(visibilityPipelineLayoutDescriptor);
	}

	void VisibilityRenderer::createDebugPipelineLayout() {
		PipelineLayoutDescriptor debugPipelineLayoutDescriptor{};
		debugPipelineLayoutDescriptor.descriptorSetLayouts = {debugDescriptorSetLayout};

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
		vertexPositionComponentBufferDescriptor.size = scene->clusters.count * CLUSTER_INDEX_COUNT * 3 * sizeof(float32);
		vertexPositionComponentBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		vertexPositionComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexPositionComponentBuffer = createBuffer(vertexPositionComponentBufferDescriptor);
	}

	void VisibilityRenderer::createVertexBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = scene->clusters.count * CLUSTER_INDEX_COUNT * sizeof(Vertex);
		vertexBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexBuffer = createBuffer(vertexBufferDescriptor);
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
		ref<ShaderModule> fragmentShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::OPENGL:
				vertexShaderModule = loadShaderModule("Visibility/VisibilitySingleSet.vert", vertexShaderModuleDescriptor);
				fragmentShaderModule = loadShaderModule("Visibility/VisibilityGL.frag", fragmentShaderModuleDescriptor);

				break;
			case GraphicsAPI::VULKAN:
				vertexShaderModule = loadShaderModule("Visibility/Visibility.vert", vertexShaderModuleDescriptor);
				fragmentShaderModule = loadShaderModule("Visibility/VisibilitySPV.frag", fragmentShaderModuleDescriptor);

				break;
			default:
				ENGINE_ERROR_UNSUPPORTED();

				break;
		}

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
				fragmentShaderModule = loadShaderModule("Visibility/DebugInputAttachment.frag", fragmentShaderModuleDescriptor);

				break;
			case GraphicsAPI::VULKAN:
				vertexShaderModule = loadShaderModule("Base/ScreenUVFlipY.vert", vertexShaderModuleDescriptor);
				fragmentShaderModule = loadShaderModule("Visibility/DebugInputAttachment.frag", fragmentShaderModuleDescriptor);

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

		const ref<Buffer> stagingBuffer = createStagingBuffer(vertexPositionComponentBuffer->size + vertexBuffer->size + clusterBuffer->size + instanceBuffer->size + geometryBuffer->size);

		uint64 stagingBufferOffset = 0;

		float32* vertexPositionComponentMappedRange = stagingBuffer->map<float32>(stagingBufferOffset, vertexPositionComponentBuffer->size);

		fillVertexPositionComponentBuffer(vertexPositionComponentMappedRange);

		BufferCopyRegion vertexPositionComponentBufferCopyRegion{};
		vertexPositionComponentBufferCopyRegion.sourceOffset = stagingBufferOffset;
		vertexPositionComponentBufferCopyRegion.destinationOffset = 0;
		vertexPositionComponentBufferCopyRegion.size = vertexPositionComponentBuffer->size;

		stagingBufferOffset += vertexPositionComponentBuffer->size;

		stagingBuffer->unmap();

		Vertex* vertexMappedRange = stagingBuffer->map<Vertex>(stagingBufferOffset, vertexBuffer->size);

		fillVertexBuffer(vertexMappedRange);

		BufferCopyRegion vertexBufferCopyRegion{};
		vertexBufferCopyRegion.sourceOffset = stagingBufferOffset;
		vertexBufferCopyRegion.destinationOffset = 0;
		vertexBufferCopyRegion.size = vertexBuffer->size;

		stagingBuffer->unmap();

		stagingBufferOffset += vertexBuffer->size;

		Cluster* clusterMappedRange = stagingBuffer->map<Cluster>(stagingBufferOffset, clusterBuffer->size);

		fillClusterBuffer(clusterMappedRange);

		BufferCopyRegion clusterBufferCopyRegion{};
		clusterBufferCopyRegion.sourceOffset = stagingBufferOffset;
		clusterBufferCopyRegion.destinationOffset = 0;
		clusterBufferCopyRegion.size = clusterBuffer->size;

		stagingBufferOffset += clusterBuffer->size;

		stagingBuffer->unmap();

		Instance* instanceMappedRange = stagingBuffer->map<Instance>(stagingBufferOffset, instanceBuffer->size);

		fillInstanceBuffer(instanceMappedRange);

		BufferCopyRegion instanceBufferCopyRegion{};
		instanceBufferCopyRegion.sourceOffset = stagingBufferOffset;
		instanceBufferCopyRegion.destinationOffset = 0;
		instanceBufferCopyRegion.size = instanceBuffer->size;

		stagingBufferOffset += instanceBuffer->size;

		stagingBuffer->unmap();

		Geometry* geometryMappedRange = stagingBuffer->map<Geometry>(stagingBufferOffset, geometryBuffer->size);

		fillGeometryBuffer(geometryMappedRange);

		BufferCopyRegion geometryBufferCopyRegion{};
		geometryBufferCopyRegion.sourceOffset = stagingBufferOffset;
		geometryBufferCopyRegion.destinationOffset = 0;
		geometryBufferCopyRegion.size = geometryBuffer->size;

		stagingBuffer->unmap();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexPositionComponentBuffer, {vertexPositionComponentBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
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

	void VisibilityRenderer::fillVertexBuffer(Vertex* mappedRange) const {
		for (uint32 vertexIndex = 0; vertexIndex < scene->vertexPositionIndices.count; vertexIndex += 1) {
			mappedRange[vertexIndex].positionIndex = scene->vertexPositionIndices[vertexIndex];
		}
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

		DescriptorBuffer vertexPositionComponentDescriptorBuffer{};
		vertexPositionComponentDescriptorBuffer.buffer = vertexPositionComponentBuffer;
		vertexPositionComponentDescriptorBuffer.offset = 0;
		vertexPositionComponentDescriptorBuffer.size = vertexPositionComponentBuffer->size;

		DescriptorBuffer vertexDescriptorBuffer{};
		vertexDescriptorBuffer.buffer = vertexBuffer;
		vertexDescriptorBuffer.offset = 0;
		vertexDescriptorBuffer.size = vertexBuffer->size;

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

		DescriptorBuffer debugCameraDescriptorBuffer{};
		debugCameraDescriptorBuffer.buffer = cameraBuffer;
		debugCameraDescriptorBuffer.offset = sizeof(mat4x4<float32>);
		debugCameraDescriptorBuffer.size = 2 * sizeof(float32);

		WriteDescriptorSet cameraWriteDescriptorSet{};
		cameraWriteDescriptorSet.descriptorSet = cameraDescriptorSet;
		cameraWriteDescriptorSet.bindingIndex = 0;
		cameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet vertexPositionComponentWriteDescriptorSet{};
		vertexPositionComponentWriteDescriptorSet.descriptorSet = clusterDescriptorSet;
		vertexPositionComponentWriteDescriptorSet.bindingIndex = 0;
		vertexPositionComponentWriteDescriptorSet.descriptorBuffers = {vertexPositionComponentDescriptorBuffer};

		WriteDescriptorSet vertexWriteDescriptorSet{};
		vertexWriteDescriptorSet.descriptorSet = clusterDescriptorSet;
		vertexWriteDescriptorSet.bindingIndex = 1;
		vertexWriteDescriptorSet.descriptorBuffers = {vertexDescriptorBuffer};

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
		debugCameraWriteDescriptorSet.descriptorBuffers = {debugCameraDescriptorBuffer};

		getDevice()->updateDescriptorSets({
			cameraWriteDescriptorSet,
			vertexPositionComponentWriteDescriptorSet,
			vertexWriteDescriptorSet,
			clusterWriteDescriptorSet,
			instanceWriteDescriptorSet,
			geometryWriteDescriptorSet,
			debugCameraWriteDescriptorSet,
		});
	}

	void VisibilityRenderer::updateDebugDescriptorSet() {
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