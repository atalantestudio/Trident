#include "pch.hpp"

#include "Engine/Renderer/3D/VisibilityRenderer.hpp"

namespace ProjectA {
	void VisibilityRenderer::updateRenderPass() {
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

		AttachmentReference writeSwapchainAttachmentReference{};
		writeSwapchainAttachmentReference.index = 0;
		writeSwapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		AttachmentReference writeVisibilityAttachmentReference{};
		writeVisibilityAttachmentReference.index = 1;
		writeVisibilityAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
 
		AttachmentReference readVisibilityAttachmentReference{};
		readVisibilityAttachmentReference.index = 1;
		readVisibilityAttachmentReference.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

		SubpassDescriptor visibilitySubpassDescriptor{};
		visibilitySubpassDescriptor.colorAttachmentReferences = {writeVisibilityAttachmentReference};

		SubpassDescriptor presentSubpassDescriptor{};
		presentSubpassDescriptor.colorAttachmentReferences = {writeSwapchainAttachmentReference};
		presentSubpassDescriptor.inputAttachmentReferences = {readVisibilityAttachmentReference};

		const ref<Subpass> visibilitySubpass = createSubpass(visibilitySubpassDescriptor);
		const ref<Subpass> presentSubpass = createSubpass(presentSubpassDescriptor);

		RenderPassDescriptor renderPassDescriptor{};
		renderPassDescriptor.attachments = {swapchainAttachment, visibilityAttachment};
		renderPassDescriptor.subpasses = {visibilitySubpass, presentSubpass};

		renderPass = createRenderPass(renderPassDescriptor);
	}

	void VisibilityRenderer::updateVisibilityImage() {
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

	void VisibilityRenderer::updateVisibilityImageView() {
		ImageViewDescriptor visibilityImageViewDescriptor{};
		visibilityImageViewDescriptor.mipLevelOffset = 0;
		visibilityImageViewDescriptor.mipLevelCount = visibilityImage->getMipLevelCount();
		visibilityImageViewDescriptor.layerOffset = 0;
		visibilityImageViewDescriptor.layerCount = visibilityImage->getLayerCount();

		visibilityImageView = visibilityImage->createView(visibilityImageViewDescriptor);
	}

	void VisibilityRenderer::updateSceneBuffers() {
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

	void VisibilityRenderer::recreateFramebuffers() {
		const ref<Swapchain>& swapchain = getSwapchain();
		const Extent2D swapchainExtent = swapchain->getExtent();

		for (ref<Framebuffer>& framebuffer : framebuffers) {
			framebuffer.~shared_ptr();
		}

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor{};
		framebufferDescriptor.renderPass = renderPass;
		framebufferDescriptor.imageViews = sequence<ref<ImageView>>(2);
		framebufferDescriptor.imageViews[1] = visibilityImageView;
		framebufferDescriptor.extent = swapchain->getExtent();
		framebufferDescriptor.layerCount = 1;

		for (uint32 framebufferIndex = 0; framebufferIndex < framebuffers.count; framebufferIndex += 1) {
			framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
		}
	}

	void VisibilityRenderer::createCameraBuffer() {
		BufferDescriptor cameraBufferDescriptor{};
		cameraBufferDescriptor.size = sizeof(Camera);
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		cameraBufferDescriptor.persistent = true;

		cameraBuffer = createBuffer(cameraBufferDescriptor);

		cameraBufferMappedRange = cameraBuffer->map<Camera>(0, cameraBuffer->size);
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

		instanceBufferMappedRange = instanceBuffer->map<Instance>(0, instanceBuffer->size);
	}

	void VisibilityRenderer::createGeometryBuffer() {
		DEBUG_ASSERT(scene != nullptr);

		BufferDescriptor geometryBufferDescriptor{};
		geometryBufferDescriptor.size = scene->geometries.count * sizeof(Geometry);
		geometryBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		geometryBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		geometryBuffer = createBuffer(geometryBufferDescriptor);
	}

	void VisibilityRenderer::createVisibilityPipelineLayout() {
		DescriptorSetLayoutBinding cameraBufferBinding{};
		cameraBufferBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraBufferBinding.stageFlags = ShaderStage::VERTEX;
		cameraBufferBinding.buffers = {cameraBuffer};

		DescriptorSetLayoutBinding vertexPositionComponentBufferBinding{};
		vertexPositionComponentBufferBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		vertexPositionComponentBufferBinding.stageFlags = ShaderStage::VERTEX;
		vertexPositionComponentBufferBinding.buffers = {vertexPositionComponentBuffer};

		DescriptorSetLayoutBinding vertexBufferBinding{};
		vertexBufferBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		vertexBufferBinding.stageFlags = ShaderStage::VERTEX;
		vertexBufferBinding.buffers = {vertexBuffer};

		DescriptorSetLayoutBinding clusterBufferBinding{};
		clusterBufferBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		clusterBufferBinding.stageFlags = ShaderStage::VERTEX;
		clusterBufferBinding.buffers = {clusterBuffer};

		DescriptorSetLayoutBinding instanceBufferBinding{};
		instanceBufferBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		instanceBufferBinding.stageFlags = ShaderStage::VERTEX;
		instanceBufferBinding.buffers = {instanceBuffer};

		DescriptorSetLayoutBinding geometryBufferBinding{};
		geometryBufferBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		geometryBufferBinding.stageFlags = ShaderStage::VERTEX;
		geometryBufferBinding.buffers = {geometryBuffer};

		DescriptorSetLayout cameraDescriptorSetLayout{};
		cameraDescriptorSetLayout.bindings = {cameraBufferBinding};

		DescriptorSetLayout clusterDescriptorSetLayout{};
		clusterDescriptorSetLayout.bindings = {vertexPositionComponentBufferBinding, vertexBufferBinding, clusterBufferBinding};

		DescriptorSetLayout instanceDescriptorSetLayout{};
		instanceDescriptorSetLayout.bindings = {instanceBufferBinding, geometryBufferBinding};

		PipelineLayoutDescriptor visibilityPipelineLayoutDescriptor{};
		visibilityPipelineLayoutDescriptor.layouts = {cameraDescriptorSetLayout, clusterDescriptorSetLayout, instanceDescriptorSetLayout};

		visibilityPipelineLayout = createPipelineLayout(visibilityPipelineLayoutDescriptor);
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
				vertexShaderModule = loadShaderModule("Asset/Shader/Visibility/VisibilitySingleSet.vert", vertexShaderModuleDescriptor);

				break;
			case GraphicsAPI::VULKAN:
				vertexShaderModule = loadShaderModule("Asset/Shader/Visibility/Visibility.vert", vertexShaderModuleDescriptor);

				break;
			default:
				ENGINE_ERROR_UNSUPPORTED();

				break;
		}

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Asset/Shader/Visibility/Visibility.frag", fragmentShaderModuleDescriptor);

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
		visibilityRenderPipelineDescriptor.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT;
		visibilityRenderPipelineDescriptor.renderPass = renderPass;
		visibilityRenderPipelineDescriptor.subpassIndex = VISIBILITY_SUBPASS_INDEX;

		visibilityRenderPipeline = createRenderPipeline(visibilityRenderPipelineDescriptor);
	}

	void VisibilityRenderer::createPresentPipelineLayout() {
		DescriptorSetLayoutBinding visibilityImageViewBinding{};
		visibilityImageViewBinding.descriptorType = DescriptorType::INPUT_ATTACHMENT;
		visibilityImageViewBinding.stageFlags = ShaderStage::FRAGMENT;
		visibilityImageViewBinding.imageViews = {visibilityImageView};

		DescriptorSetLayout visibilityDescriptorSetLayout{};
		visibilityDescriptorSetLayout.bindings = {visibilityImageViewBinding};

		PipelineLayoutDescriptor presentPipelineLayoutDescriptor{};
		presentPipelineLayoutDescriptor.layouts = {visibilityDescriptorSetLayout};

		presentPipelineLayout = createPipelineLayout(presentPipelineLayoutDescriptor);
	}

	void VisibilityRenderer::createPresentRenderPipeline() {
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
				vertexShaderModule = loadShaderModule("Asset/Shader/Base/ScreenUV.vert", vertexShaderModuleDescriptor);
				fragmentShaderModule = loadShaderModule("Asset/Shader/Visibility/DebugTextureSampler.frag", fragmentShaderModuleDescriptor);

				break;
			case GraphicsAPI::VULKAN:
				vertexShaderModule = loadShaderModule("Asset/Shader/Base/Screen.vert", vertexShaderModuleDescriptor);
				fragmentShaderModule = loadShaderModule("Asset/Shader/Visibility/DebugInputAttachment.frag", fragmentShaderModuleDescriptor);

				break;
			default:
				ENGINE_ERROR_UNSUPPORTED();

				break;
		}

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = false;

		RenderPipelineDescriptor presentRenderPipelineDescriptor{};
		presentRenderPipelineDescriptor.pipelineLayout = presentPipelineLayout;
		presentRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		presentRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		presentRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		presentRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		presentRenderPipelineDescriptor.cullMode = CullMode::BACK;
		presentRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		presentRenderPipelineDescriptor.lineWidth = 1.0f;
		presentRenderPipelineDescriptor.minSampleShading = 0.0f;
		presentRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		presentRenderPipelineDescriptor.depthStencilState.depthTest = false;
		presentRenderPipelineDescriptor.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT;
		presentRenderPipelineDescriptor.renderPass = renderPass;
		presentRenderPipelineDescriptor.subpassIndex = PRESENT_SUBPASS_INDEX;

		presentRenderPipeline = createRenderPipeline(presentRenderPipelineDescriptor);
	}

	void VisibilityRenderer::updateCamera() {
		cameraBufferMappedRange->projectionView = camera->projection;
	}

	void VisibilityRenderer::updateInstanceTransform(Entity entity, const mat4x4<float32>& transform) {
		instanceBufferMappedRange[entity].transform = transform;
	}

	void VisibilityRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		BeginRenderPassDescriptor beginRenderPassDescriptor{};
		beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
		beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};

		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(swapchainExtent.width);
		const float32 height = static_cast<float32>(swapchainExtent.height);

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchainExtent);

		// Visibility subpass
		{
			commandEncoder->bindRenderPipeline(visibilityRenderPipeline);
			commandEncoder->bindAllDescriptorSets();

			const uint32 instanceCount = static_cast<uint32>(scene->clusters.count);

			// TODO: Use indirect drawing.
			commandEncoder->draw(CLUSTER_INDEX_COUNT, instanceCount, 0, 0);
		}

		commandEncoder->nextSubpass();

		// Present subpass
		{
			commandEncoder->bindRenderPipeline(presentRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->draw(3, 1, 0, 0);
		}

		commandEncoder->endRenderPass();
	}
}