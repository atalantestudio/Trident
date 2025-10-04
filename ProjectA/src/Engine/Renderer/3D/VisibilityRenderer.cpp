#include "pch.hpp"

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
		createVisibilityDescriptorSetLayout();

		allocateDescriptorSets();

		createVisibilityPipelineLayout();
		createScreenPipelineLayout();

		createVisibilityRenderPipeline();
		createScreenRenderPipeline();

		createIndirectBuffer();
		createCameraBuffer();
	}

	void VisibilityRenderer::setScene(const ClusteredScene* scene) {
		SceneRenderer::setScene(scene);

		createVertexPositionComponentBuffer();
		createVertexBuffer();
		createClusterBuffer();
		createInstanceBuffer();
		createGeometryBuffer();

		fillSceneBuffers();

		updateAllDescriptorSets();
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
 
		AttachmentReference readVisibilityAttachmentReference{};
		readVisibilityAttachmentReference.index = 1;
		readVisibilityAttachmentReference.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

		AttachmentReference writeDepthStencilAttachmentReference{};
		writeDepthStencilAttachmentReference.index = 2;
		writeDepthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		SubpassDescriptor visibilitySubpassDescriptor{};
		visibilitySubpassDescriptor.colorAttachmentReferences = {writeVisibilityAttachmentReference};
		visibilitySubpassDescriptor.depthStencilAttachmentReference = writeDepthStencilAttachmentReference;

		SubpassDescriptor screenSubpassDescriptor{};
		screenSubpassDescriptor.colorAttachmentReferences = {writeSwapchainAttachmentReference};
		screenSubpassDescriptor.inputAttachmentReferences = {readVisibilityAttachmentReference};

		const ref<Subpass> visibilitySubpass = createSubpass(visibilitySubpassDescriptor);
		const ref<Subpass> screenSubpass = createSubpass(screenSubpassDescriptor);

		RenderPassDescriptor renderPassDescriptor{};
		renderPassDescriptor.attachments = {swapchainAttachment, visibilityAttachment, depthStencilAttachment};
		renderPassDescriptor.subpasses = {visibilitySubpass, screenSubpass};

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

	void VisibilityRenderer::createVisibilityDescriptorSetLayout() {
		DescriptorSetLayoutBinding visibilityBinding{};
		visibilityBinding.descriptorType = DescriptorType::INPUT_ATTACHMENT;
		visibilityBinding.descriptorCount = 1;
		visibilityBinding.shaderStages = ShaderStage::FRAGMENT;

		visibilityDescriptorSetLayout = createDescriptorSetLayout({visibilityBinding});
	}

	void VisibilityRenderer::createVisibilityPipelineLayout() {
		PipelineLayoutDescriptor visibilityPipelineLayoutDescriptor{};
		visibilityPipelineLayoutDescriptor.descriptorSetLayouts = {cameraDescriptorSetLayout, clusterDescriptorSetLayout, instanceDescriptorSetLayout};

		visibilityPipelineLayout = createPipelineLayout(visibilityPipelineLayoutDescriptor);
	}

	void VisibilityRenderer::createScreenPipelineLayout() {
		PipelineLayoutDescriptor screenPipelineLayoutDescriptor{};
		screenPipelineLayoutDescriptor.descriptorSetLayouts = {visibilityDescriptorSetLayout};

		screenPipelineLayout = createPipelineLayout(screenPipelineLayoutDescriptor);
	}

	void VisibilityRenderer::createIndirectBuffer() {
		BufferDescriptor indirectBufferDescriptor{};
		indirectBufferDescriptor.size = 4 * sizeof(uint32);
		indirectBufferDescriptor.usage = BufferUsage::INDIRECT;
		indirectBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		indirectBufferDescriptor.persistent = true;

		indirectBuffer = createBuffer(indirectBufferDescriptor);

		indirectMappedRange = indirectBuffer->map<uint32>(0, indirectBuffer->size);
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
		vertexPositionComponentBufferDescriptor.size = scene->clusters.count * CLUSTER_VERTEX_COUNT * 3 * sizeof(float32);
		vertexPositionComponentBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		vertexPositionComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexPositionComponentBuffer = createBuffer(vertexPositionComponentBufferDescriptor);
	}

	void VisibilityRenderer::createVertexBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = scene->clusters.count * CLUSTER_VERTEX_COUNT * sizeof(Vertex);
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
		visibilityRenderPipelineDescriptor.depthStencilState.depthTest = false;
		// visibilityRenderPipelineDescriptor.depthStencilState.depthWrite = true;
		// visibilityRenderPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER; // TODO: Use LESS.
		visibilityRenderPipelineDescriptor.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT;
		visibilityRenderPipelineDescriptor.renderPass = renderPass;
		visibilityRenderPipelineDescriptor.subpassIndex = VISIBILITY_SUBPASS_INDEX;

		visibilityRenderPipeline = createRenderPipeline(visibilityRenderPipelineDescriptor);
	}

	void VisibilityRenderer::createScreenRenderPipeline() {
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
				fragmentShaderModule = loadShaderModule("Visibility/DebugTextureSampler.frag", fragmentShaderModuleDescriptor);

				break;
			case GraphicsAPI::VULKAN:
				vertexShaderModule = loadShaderModule("Base/Screen.vert", vertexShaderModuleDescriptor);
				fragmentShaderModule = loadShaderModule("Visibility/DebugInputAttachment.frag", fragmentShaderModuleDescriptor);

				break;
			default:
				ENGINE_ERROR_UNSUPPORTED();

				break;
		}

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor screenRenderPipelineDescriptor{};
		screenRenderPipelineDescriptor.pipelineLayout = screenPipelineLayout;
		screenRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		screenRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		screenRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		screenRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		screenRenderPipelineDescriptor.cullMode = CullMode::BACK;
		screenRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		screenRenderPipelineDescriptor.lineWidth = 1.0f;
		screenRenderPipelineDescriptor.minSampleShading = 0.0f;
		screenRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		screenRenderPipelineDescriptor.depthStencilState.depthTest = false;
		screenRenderPipelineDescriptor.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT;
		screenRenderPipelineDescriptor.renderPass = renderPass;
		screenRenderPipelineDescriptor.subpassIndex = SCREEN_SUBPASS_INDEX;

		screenRenderPipeline = createRenderPipeline(screenRenderPipelineDescriptor);
	}

	void VisibilityRenderer::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({
			cameraDescriptorSetLayout,
			clusterDescriptorSetLayout,
			instanceDescriptorSetLayout,
			visibilityDescriptorSetLayout,
		});

		cameraDescriptorSet = descriptorSets[0];
		clusterDescriptorSet = descriptorSets[1];
		instanceDescriptorSet = descriptorSets[2];
		visibilityDescriptorSet = descriptorSets[3];
	}

	void VisibilityRenderer::fillSceneBuffers() {
		indirectMappedRange[0] = CLUSTER_VERTEX_COUNT;
		indirectMappedRange[1] = static_cast<uint32>(scene->clusters.count);
		indirectMappedRange[2] = 0;
		indirectMappedRange[3] = 0;

		const uint64 stagingBufferSize = vertexPositionComponentBuffer->size + vertexBuffer->size + clusterBuffer->size + instanceBuffer->size /* + geometryBuffer->size */;

		const ref<Buffer> stagingBuffer = createStagingBuffer(stagingBufferSize);

		uint64 stagingBufferOffset = 0;

		float32* vertexPositionComponentMappedRange = stagingBuffer->map<float32>(stagingBufferOffset, vertexPositionComponentBuffer->size);

		std::copy(scene->vertexPositionComponents.begin(), scene->vertexPositionComponents.end(), vertexPositionComponentMappedRange);

		BufferCopyRegion vertexPositionComponentBufferCopyRegion{};
		vertexPositionComponentBufferCopyRegion.sourceOffset = stagingBufferOffset;
		vertexPositionComponentBufferCopyRegion.destinationOffset = 0;
		vertexPositionComponentBufferCopyRegion.size = vertexPositionComponentBuffer->size;

		stagingBufferOffset += vertexPositionComponentBuffer->size;

		stagingBuffer->unmap();

		Vertex* vertexMappedRange = stagingBuffer->map<Vertex>(stagingBufferOffset, vertexBuffer->size);

		for (uint32 vertexIndex = 0; vertexIndex < scene->vertexPositionIndices.count; vertexIndex += 1) {
			vertexMappedRange[vertexIndex].positionIndex = scene->vertexPositionIndices[vertexIndex];
		}

		BufferCopyRegion vertexBufferCopyRegion{};
		vertexBufferCopyRegion.sourceOffset = stagingBufferOffset;
		vertexBufferCopyRegion.destinationOffset = 0;
		vertexBufferCopyRegion.size = vertexBuffer->size;

		stagingBuffer->unmap();

		stagingBufferOffset += vertexBuffer->size;

		Cluster* clusterMappedRange = stagingBuffer->map<Cluster>(stagingBufferOffset, clusterBuffer->size);

		for (uint32 clusterIndex = 0; clusterIndex < scene->clusters.count; clusterIndex += 1) {
			clusterMappedRange[clusterIndex].instanceIndex = scene->clusters[clusterIndex].instanceIndex;
		}

		BufferCopyRegion clusterBufferCopyRegion{};
		clusterBufferCopyRegion.sourceOffset = stagingBufferOffset;
		clusterBufferCopyRegion.destinationOffset = 0;
		clusterBufferCopyRegion.size = clusterBuffer->size;

		stagingBufferOffset += clusterBuffer->size;

		stagingBuffer->unmap();

		Instance* instanceMappedRange = stagingBuffer->map<Instance>(stagingBufferOffset, instanceBuffer->size);

		for (uint32 instanceIndex = 0; instanceIndex < scene->instances.count; instanceIndex += 1) {
			// TODO
			instanceMappedRange[instanceIndex].transform = mat4x4<float32>::identity();
			instanceMappedRange[instanceIndex].geometryIndex = scene->instances[instanceIndex].geometryIndex;
		}

		BufferCopyRegion instanceBufferCopyRegion{};
		instanceBufferCopyRegion.sourceOffset = stagingBufferOffset;
		instanceBufferCopyRegion.destinationOffset = 0;
		instanceBufferCopyRegion.size = instanceBuffer->size;

		stagingBufferOffset += instanceBuffer->size;

		stagingBuffer->unmap();

		/* Geometry* geometryMappedRange = stagingBuffer->map<Geometry>(stagingBufferOffset, geometryBuffer->size);

		for (uint32 geometryIndex = 0; geometryIndex < scene->geometries.count; geometryIndex += 1) {
			geometryMappedRange[geometryIndex].vertexBufferOffset = 0;
		}

		BufferCopyRegion geometryBufferCopyRegion{};
		geometryBufferCopyRegion.sourceOffset = stagingBufferOffset;
		geometryBufferCopyRegion.destinationOffset = 0;
		geometryBufferCopyRegion.size = geometryBuffer->size;

		stagingBuffer->unmap(); */

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexPositionComponentBuffer, {vertexPositionComponentBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, clusterBuffer, {clusterBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, instanceBuffer, {instanceBufferCopyRegion});
		// commandEncoder->copyBufferToBuffer(stagingBuffer, geometryBuffer, {geometryBufferCopyRegion});

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		getDevice()->waitIdle();
	}

	void VisibilityRenderer::updateAllDescriptorSets() {
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

		DescriptorImage visibilityDescriptorImage{};
		visibilityDescriptorImage.imageView = visibilityImageView;
		visibilityDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
		visibilityDescriptorImage.sampler = nullptr;

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

		WriteDescriptorSet visibilityWriteDescriptorSet{};
		visibilityWriteDescriptorSet.descriptorSet = visibilityDescriptorSet;
		visibilityWriteDescriptorSet.bindingIndex = 0;
		visibilityWriteDescriptorSet.descriptorImages = {visibilityDescriptorImage};

		getDevice()->updateDescriptorSets({
			cameraWriteDescriptorSet,
			vertexPositionComponentWriteDescriptorSet,
			vertexWriteDescriptorSet,
			clusterWriteDescriptorSet,
			instanceWriteDescriptorSet,
			geometryWriteDescriptorSet,
			visibilityWriteDescriptorSet,
		});
	}

	void VisibilityRenderer::updateVisibilityDescriptorSet() {
		DescriptorImage visibilityDescriptorImage{};
		visibilityDescriptorImage.imageView = visibilityImageView;
		visibilityDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
		visibilityDescriptorImage.sampler = nullptr;

		WriteDescriptorSet visibilityWriteDescriptorSet{};
		visibilityWriteDescriptorSet.descriptorSet = visibilityDescriptorSet;
		visibilityWriteDescriptorSet.bindingIndex = 0;
		visibilityWriteDescriptorSet.descriptorImages = {visibilityDescriptorImage};

		getDevice()->updateDescriptorSets({
			visibilityWriteDescriptorSet,
		});
	}

	void VisibilityRenderer::updateCamera() {
		cameraMappedRange->projectionView = camera->projection;
	}

	void VisibilityRenderer::updateInstanceTransform(Entity entity, const mat4x4<float32>& transform) {
		instanceMappedRange[entity].transform = transform;
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

		// Screen subpass
		{
			commandEncoder->bindRenderPipeline(screenRenderPipeline);
			commandEncoder->bindDescriptorSets(screenPipelineLayout, {visibilityDescriptorSet});
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

		updateVisibilityDescriptorSet();

		updateCamera();
	}
}