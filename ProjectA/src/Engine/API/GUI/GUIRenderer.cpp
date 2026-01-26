#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/API/GUI/GUIRenderer.hpp"

namespace ProjectA {
	void GUIRenderer::build() {
		setupRenderPass();

		recreateFramebuffers();

		createCameraBuffer();
		createVertexBuffer();
		createIndexBuffer();
		createSubcomponentBuffer();
		createGlyphSubcomponentBuffer();
		createTextureArrayStagingBuffer();

		createTextureArrayImage();
		createTextureArrayImageView();

		createNearestSampler();
		createLinearSampler();

		createDescriptorSetLayouts();

		allocateDescriptorSets();

		copyBuffers();

		createSubcomponentPipelineLayout();
		createGlyphSubcomponentPipelineLayout();

		createSubcomponentRenderPipeline();
		createGlyphSubcomponentRenderPipeline();

		updateDescriptorSets();
		updateCameraProjection();
	}

	Extent2D GUIRenderer::getTextureArrayLayerExtent(uint32 layerIndex) const {
		ENGINE_ASSERT(layerIndex < textureArrayLayerCount, "Layer index overflow.");

		return textureArrayLayerExtents[layerIndex];
	}

	void GUIRenderer::flushTextureArrayStagingBuffer() const {
		DEBUG_ASSERT(textureArrayLayerCount > 0);

		textureArrayStagingBuffer->unmap();

		sequence<BufferImageCopyRegion> bufferImageCopyRegions(textureArrayLayerCount);
		uint64 bufferOffset = 0;

		for (uint32 layerIndex = 0; layerIndex < textureArrayLayerCount; layerIndex += 1) {
			const Extent2D extent = textureArrayLayerExtents[layerIndex];

			BufferImageCopyRegion& bufferImageCopyRegion = bufferImageCopyRegions[layerIndex];
			bufferImageCopyRegion.bufferOffset = bufferOffset;
			bufferImageCopyRegion.imageOffsetX = 0;
			bufferImageCopyRegion.imageOffsetY = 0;
			bufferImageCopyRegion.imageOffsetZ = 0;
			bufferImageCopyRegion.imageExtent.width = extent.width;
			bufferImageCopyRegion.imageExtent.height = extent.height;
			bufferImageCopyRegion.imageExtent.depth = 1;
			bufferImageCopyRegion.imageMipLevel = 0;
			bufferImageCopyRegion.imageLayerOffset = layerIndex;
			bufferImageCopyRegion.imageLayerCount = 1;

			bufferOffset += extent.width * extent.height * TEXTURE_ARRAY_LAYER_BPP;
		}

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(textureArrayImage, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
		commandEncoder->copyBufferToImage(textureArrayStagingBuffer, textureArrayImage, bufferImageCopyRegions);
		commandEncoder->setImageMemoryBarrier(textureArrayImage, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		graphicsQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
		graphicsQueue->waitIdle();

		textureArrayStagingBuffer.~shared_ptr();
	}

	uint32 GUIRenderer::registerTextureArrayLayer(const TextureArrayLayer& layer) {
		ENGINE_ASSERT(textureArrayLayerCount + 1 <= TEXTURE_ARRAY_MAX_LAYER_COUNT, "Too many client-side images.");
		ENGINE_ASSERT(layer.width <= TEXTURE_ARRAY_MAX_WIDTH, "Image width ([]) is larger than the maximum width ([]).", layer.width, TEXTURE_ARRAY_MAX_WIDTH);
		ENGINE_ASSERT(layer.height <= TEXTURE_ARRAY_MAX_HEIGHT, "Image height ([]) is larger than the maximum height ([]).", layer.height, TEXTURE_ARRAY_MAX_HEIGHT);

		const uint32 layerIndex = textureArrayLayerCount;

		std::copy(layer.data.begin(), layer.data.end(), textureArrayStagingBufferMappedRange);

		textureArrayStagingBufferMappedRange += layer.data.count();
		textureArrayLayerExtents[textureArrayLayerCount].width = layer.width;
		textureArrayLayerExtents[textureArrayLayerCount].height = layer.height;
		textureArrayLayerCount += 1;

		return layerIndex;
	}

	void GUIRenderer::registerSubcomponent(const GUISubcomponent& subcomponent) {
		DEBUG_ASSERT(subcomponentCount < MAX_SUBCOMPONENTS);

		subcomponentBufferMappedRange[subcomponentCount] = subcomponent;
		subcomponentCount += 1;
	}

	void GUIRenderer::registerGlyphSubcomponent(const GUIGlyphSubcomponent& glyphSubcomponent) {
		DEBUG_ASSERT(glyphSubcomponentCount < MAX_GLYPH_SUBCOMPONENTS);

		glyphSubcomponentBufferMappedRange[glyphSubcomponentCount] = glyphSubcomponent;
		glyphSubcomponentCount += 1;
	}

	void GUIRenderer::setupRenderPass() {
		Attachment swapchainAttachment{};
		swapchainAttachment.format = getSurface()->getFormat();
		swapchainAttachment.sampleCount = 1;

		// TODO(layer-superposition)
		// TODO: Using LOAD when this layer is the only one registered will generate non-blocking errors from the Vulkan validation layers.
		swapchainAttachment.loadOperation = LoadOperation::LOAD;
		// swapchainAttachment.loadOperation = LoadOperation::CLEAR;

		swapchainAttachment.storeOperation = StoreOperation::STORE;
		swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

		AttachmentReference swapchainAttachmentReference{};
		swapchainAttachmentReference.index = 0;
		swapchainAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

		SubpassDescriptor subpassDescriptor{};
		subpassDescriptor.colorAttachmentReferences = {swapchainAttachmentReference};

		const ref<Subpass> subpass = createSubpass(subpassDescriptor);

		RenderPassDescriptor renderPassDescriptor{};
		renderPassDescriptor.attachments = {swapchainAttachment};
		renderPassDescriptor.subpasses = {subpass};

		renderPass = createRenderPass(renderPassDescriptor);
	}

	void GUIRenderer::recreateFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor{};
		framebufferDescriptor.renderPass = renderPass;
		framebufferDescriptor.imageViews = {nullptr};
		framebufferDescriptor.extent = swapchain->getExtent();
		framebufferDescriptor.layerCount = 1;

		for (uint32 framebufferIndex = 0; framebufferIndex < framebuffers.count(); framebufferIndex += 1) {
			framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
		}
	}

	void GUIRenderer::createCameraBuffer() {
		BufferDescriptor cameraBufferDescriptor{};
		cameraBufferDescriptor.size = sizeof(Camera);
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		cameraBufferDescriptor.persistent = true;

		cameraBuffer = createBuffer(cameraBufferDescriptor);

		cameraBufferMappedRange = cameraBuffer->map<Camera>(0, cameraBuffer->size);
	}

	void GUIRenderer::createVertexBuffer() {
		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexBuffer = createBuffer(vertexBufferDescriptor);
	}

	void GUIRenderer::createIndexBuffer() {
		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = INDEX_BUFFER_SIZE;
		indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
		indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		indexBuffer = createBuffer(indexBufferDescriptor);
	}

	void GUIRenderer::createSubcomponentBuffer() {
		BufferDescriptor subcomponentBufferDescriptor{};
		subcomponentBufferDescriptor.size = SUBCOMPONENT_BUFFER_SIZE;
		subcomponentBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		subcomponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		subcomponentBufferDescriptor.persistent = true;

		subcomponentBuffer = createBuffer(subcomponentBufferDescriptor);

		subcomponentBufferMappedRange = subcomponentBuffer->map<GUISubcomponent>(0, subcomponentBuffer->size);
	}

	void GUIRenderer::createGlyphSubcomponentBuffer() {
		BufferDescriptor glyphSubcomponentBufferDescriptor{};
		glyphSubcomponentBufferDescriptor.size = GLYPH_SUBCOMPONENT_BUFFER_SIZE;
		glyphSubcomponentBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		glyphSubcomponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphSubcomponentBufferDescriptor.persistent = true;

		glyphSubcomponentBuffer = createBuffer(glyphSubcomponentBufferDescriptor);

		glyphSubcomponentBufferMappedRange = glyphSubcomponentBuffer->map<GUIGlyphSubcomponent>(0, glyphSubcomponentBuffer->size);
	}

	void GUIRenderer::createTextureArrayStagingBuffer() {
		textureArrayStagingBuffer = createStagingBuffer(TEXTURE_ARRAY_MAX_WIDTH * TEXTURE_ARRAY_MAX_HEIGHT * TEXTURE_ARRAY_MAX_LAYER_COUNT * TEXTURE_ARRAY_LAYER_BPP);

		textureArrayStagingBufferMappedRange = textureArrayStagingBuffer->map<uint8>(0, textureArrayStagingBuffer->size);
	}

	void GUIRenderer::createTextureArrayImage() {
		ImageDescriptor textureArrayImageDescriptor{};
		textureArrayImageDescriptor.type = ImageType::TEXTURE_2D;
		textureArrayImageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		textureArrayImageDescriptor.extent.width = TEXTURE_ARRAY_MAX_WIDTH;
		textureArrayImageDescriptor.extent.height = TEXTURE_ARRAY_MAX_HEIGHT;
		textureArrayImageDescriptor.extent.depth = 1;
		textureArrayImageDescriptor.mipLevelCount = 1;
		textureArrayImageDescriptor.layerCount = TEXTURE_ARRAY_MAX_LAYER_COUNT;
		textureArrayImageDescriptor.sampleCount = 1;
		textureArrayImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
		textureArrayImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		textureArrayImage = createImage(textureArrayImageDescriptor);
	}

	void GUIRenderer::createTextureArrayImageView() {
		ImageViewDescriptor textureArrayImageViewDescriptor{};
		textureArrayImageViewDescriptor.mipLevelOffset = 0;
		textureArrayImageViewDescriptor.mipLevelCount = textureArrayImage->getMipLevelCount();
		textureArrayImageViewDescriptor.layerOffset = 0;
		textureArrayImageViewDescriptor.layerCount = textureArrayImage->getLayerCount();

		textureArrayImageView = textureArrayImage->createView(textureArrayImageViewDescriptor);
	}

	void GUIRenderer::createNearestSampler() {
		SamplerDescriptor nearestSamplerDescriptor{};
		nearestSamplerDescriptor.magFilter = SamplerFilter::NEAREST;
		nearestSamplerDescriptor.minFilter = SamplerFilter::NEAREST;

		nearestSampler = createSampler(nearestSamplerDescriptor);
	}

	void GUIRenderer::createLinearSampler() {
		SamplerDescriptor linearSamplerDescriptor{};
		linearSamplerDescriptor.magFilter = SamplerFilter::LINEAR;
		linearSamplerDescriptor.minFilter = SamplerFilter::LINEAR;

		linearSampler = createSampler(linearSamplerDescriptor);
	}

	void GUIRenderer::createDescriptorSetLayouts() {
		DescriptorSetLayoutBinding cameraBinding{};
		cameraBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraBinding.descriptorCount = 1;
		cameraBinding.shaderStages = ShaderStage::VERTEX;

		DescriptorSetLayoutBinding nearestTextureArrayBinding{};
		nearestTextureArrayBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		nearestTextureArrayBinding.descriptorCount = 1;
		nearestTextureArrayBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding linearTextureArrayBinding{};
		linearTextureArrayBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		linearTextureArrayBinding.descriptorCount = 1;
		linearTextureArrayBinding.shaderStages = ShaderStage::FRAGMENT;

		subcomponentDescriptorSetLayout = createDescriptorSetLayout({cameraBinding, nearestTextureArrayBinding});
		glyphSubcomponentDescriptorSetLayout = createDescriptorSetLayout({cameraBinding, linearTextureArrayBinding});
	}

	void GUIRenderer::createSubcomponentPipelineLayout() {
		PipelineLayoutDescriptor subcomponentPipelineLayoutDescriptor{};
		subcomponentPipelineLayoutDescriptor.descriptorSetLayouts = {subcomponentDescriptorSetLayout};

		subcomponentPipelineLayout = createPipelineLayout(subcomponentPipelineLayoutDescriptor);
	}

	void GUIRenderer::createGlyphSubcomponentPipelineLayout() {
		PipelineLayoutDescriptor glyphSubcomponentPipelineLayoutDescriptor{};
		glyphSubcomponentPipelineLayoutDescriptor.descriptorSetLayouts = {glyphSubcomponentDescriptorSetLayout};

		glyphSubcomponentPipelineLayout = createPipelineLayout(glyphSubcomponentPipelineLayoutDescriptor);
	}

	void GUIRenderer::createSubcomponentRenderPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec4<float32>>(); // Vertex position and texture coordinate

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<vec4<float32>>(); // Bounds
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // World transform
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // Texture transform
		instanceInputBinding.registerAttribute<vec4<float32>>(); // Color
		instanceInputBinding.registerAttribute<vec4<float32>>(); // Outline color
		instanceInputBinding.registerAttribute<uint32>(); // Texture array layer index
		instanceInputBinding.registerAttribute<float32>(); // Outline size
		instanceInputBinding.registerAttribute<float32>(); // Border radius

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "../../../../../ProjectA/src/Asset/Shader/GUI/Subcomponent.vert");
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "../../../../../ProjectA/src/Asset/Shader/GUI/Subcomponent.frag");

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = true;
		colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		RenderPipelineDescriptor subcomponentRenderPipelineDescriptor{};
		subcomponentRenderPipelineDescriptor.pipelineLayout = subcomponentPipelineLayout;
		subcomponentRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		subcomponentRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		subcomponentRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		subcomponentRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		subcomponentRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		subcomponentRenderPipelineDescriptor.cullMode = CullMode::BACK;
		subcomponentRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		subcomponentRenderPipelineDescriptor.lineWidth = 1.0f;
		subcomponentRenderPipelineDescriptor.minSampleShading = 0.0f;
		subcomponentRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		subcomponentRenderPipelineDescriptor.depthStencilState.depthTest = false;
		subcomponentRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		subcomponentRenderPipelineDescriptor.renderPass = renderPass;
		subcomponentRenderPipelineDescriptor.subpassIndex = 0;

		subcomponentRenderPipeline = createRenderPipeline(subcomponentRenderPipelineDescriptor);
	}

	void GUIRenderer::createGlyphSubcomponentRenderPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec4<float32>>(); // Vertex position and texture coordinate

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // World transform
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // Texture transform
		instanceInputBinding.registerAttribute<uint32>(); // Layer index
		instanceInputBinding.registerAttribute<vec4<float32>>(); // Color

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "../../../../../ProjectA/src/Asset/Shader/GUI/GlyphSubcomponent.vert");
		const ref<ShaderModule> fragmentShaderModule = loadShaderModule(fragmentShaderModuleDescriptor, "../../../../../ProjectA/src/Asset/Shader/GUI/GlyphSubcomponent.frag");

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.blend = true;
		colorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		RenderPipelineDescriptor glyphSubcomponentRenderPipelineDescriptor{};
		glyphSubcomponentRenderPipelineDescriptor.pipelineLayout = glyphSubcomponentPipelineLayout;
		glyphSubcomponentRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		glyphSubcomponentRenderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		glyphSubcomponentRenderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		glyphSubcomponentRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		glyphSubcomponentRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		glyphSubcomponentRenderPipelineDescriptor.cullMode = CullMode::BACK;
		glyphSubcomponentRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		glyphSubcomponentRenderPipelineDescriptor.lineWidth = 1.0f;
		glyphSubcomponentRenderPipelineDescriptor.minSampleShading = 0.0f;
		glyphSubcomponentRenderPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
		glyphSubcomponentRenderPipelineDescriptor.depthStencilState.depthTest = false;
		glyphSubcomponentRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		glyphSubcomponentRenderPipelineDescriptor.renderPass = renderPass;
		glyphSubcomponentRenderPipelineDescriptor.subpassIndex = 0;

		glyphSubcomponentRenderPipeline = createRenderPipeline(glyphSubcomponentRenderPipelineDescriptor);
	}

	void GUIRenderer::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({subcomponentDescriptorSetLayout, glyphSubcomponentDescriptorSetLayout});

		subcomponentDescriptorSet = descriptorSets[0];
		glyphSubcomponentDescriptorSet = descriptorSets[1];
	}

	void GUIRenderer::copyBuffers() const {
		const ref<Buffer> stagingBuffer = createStagingBuffer(VERTEX_BUFFER_SIZE + INDEX_BUFFER_SIZE);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->size);

		Vertex* vertexMappedRange = pointer_cast<Vertex*>(mappedRange);
		vertexMappedRange[0].position = {0.0f, 0.0f};
		vertexMappedRange[0].textureCoordinate = {0.0f, 0.0f};
		vertexMappedRange[1].position = {1.0f, 0.0f};
		vertexMappedRange[1].textureCoordinate = {1.0f, 0.0f};
		vertexMappedRange[2].position = {1.0f, 1.0f};
		vertexMappedRange[2].textureCoordinate = {1.0f, 1.0f};
		vertexMappedRange[3].position = {0.0f, 1.0f};
		vertexMappedRange[3].textureCoordinate = {0.0f, 1.0f};

		uint16* indexMappedRange = pointer_cast<uint16*>(mappedRange + VERTEX_BUFFER_SIZE);
		indexMappedRange[0] = 0;
		indexMappedRange[1] = 1;
		indexMappedRange[2] = 2;
		indexMappedRange[3] = 0;
		indexMappedRange[4] = 2;
		indexMappedRange[5] = 3;

		stagingBuffer->unmap();

		BufferCopyRegion vertexBufferCopyRegion{};
		vertexBufferCopyRegion.sourceOffset = 0;
		vertexBufferCopyRegion.destinationOffset = 0;
		vertexBufferCopyRegion.size = vertexBuffer->size;

		BufferCopyRegion indexBufferCopyRegion{};
		indexBufferCopyRegion.sourceOffset = vertexBuffer->size;
		indexBufferCopyRegion.destinationOffset = 0;
		indexBufferCopyRegion.size = indexBuffer->size;

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {vertexBufferCopyRegion});
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, {indexBufferCopyRegion});

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
		transferQueue->waitIdle();
	}

	void GUIRenderer::updateDescriptorSets() const {
		DescriptorBuffer cameraDescriptorBuffer{};
		cameraDescriptorBuffer.buffer = cameraBuffer;
		cameraDescriptorBuffer.offset = 0;
		cameraDescriptorBuffer.size = cameraBuffer->size;

		DescriptorImage nearestTextureArrayDescriptorImage{};
		nearestTextureArrayDescriptorImage.imageView = textureArrayImageView;
		nearestTextureArrayDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
		nearestTextureArrayDescriptorImage.sampler = nearestSampler;

		DescriptorImage linearTextureArrayDescriptorImage{};
		linearTextureArrayDescriptorImage.imageView = textureArrayImageView;
		linearTextureArrayDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
		linearTextureArrayDescriptorImage.sampler = linearSampler;

		WriteDescriptorSet subcomponentCameraWriteDescriptorSet{};
		subcomponentCameraWriteDescriptorSet.descriptorSet = subcomponentDescriptorSet;
		subcomponentCameraWriteDescriptorSet.bindingIndex = 0;
		subcomponentCameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet subcomponentNearestTextureArrayWriteDescriptorSet{};
		subcomponentNearestTextureArrayWriteDescriptorSet.descriptorSet = subcomponentDescriptorSet;
		subcomponentNearestTextureArrayWriteDescriptorSet.bindingIndex = 1;
		subcomponentNearestTextureArrayWriteDescriptorSet.descriptorImages = {nearestTextureArrayDescriptorImage};

		WriteDescriptorSet glyphSubcomponentCameraWriteDescriptorSet{};
		glyphSubcomponentCameraWriteDescriptorSet.descriptorSet = glyphSubcomponentDescriptorSet;
		glyphSubcomponentCameraWriteDescriptorSet.bindingIndex = 0;
		glyphSubcomponentCameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

		WriteDescriptorSet glyphSubcomponentLinearTextureArrayWriteDescriptorSet{};
		glyphSubcomponentLinearTextureArrayWriteDescriptorSet.descriptorSet = glyphSubcomponentDescriptorSet;
		glyphSubcomponentLinearTextureArrayWriteDescriptorSet.bindingIndex = 1;
		glyphSubcomponentLinearTextureArrayWriteDescriptorSet.descriptorImages = {linearTextureArrayDescriptorImage};

		getDevice()->updateDescriptorSets({
			subcomponentCameraWriteDescriptorSet,
			subcomponentNearestTextureArrayWriteDescriptorSet,
			glyphSubcomponentCameraWriteDescriptorSet,
			glyphSubcomponentLinearTextureArrayWriteDescriptorSet,
		});
	}

	void GUIRenderer::updateCameraProjection() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		cameraBufferMappedRange->projection = {
			 2.0f / swapchainExtent.width,  0.0f,                           0.0f,
			 0.0f,                         -2.0f / swapchainExtent.height,  0.0f,
			-1.0f,                          1.0f,                           1.0f,
		};
	}

	void GUIRenderer::render(ref<CommandEncoder> commandEncoder) const {
		if (subcomponentCount == 0 && glyphSubcomponentCount == 0) {
			// TODO
			// If no commands have been encoded, the attachments will not be transitioned into the final layout.
			// Handle properly the case where the command buffer is empty.
			ENGINE_ASSERT(false, "No GUI components to render.");

			return;
		}

		BeginRenderPassDescriptor beginRenderPassDescriptor{};
		beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
		beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 0.0f}};

		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(swapchainExtent.width);
		const float32 height = static_cast<float32>(swapchainExtent.height);

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->setScissor(0, 0, swapchainExtent);
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);

		if (subcomponentCount > 0) {
			commandEncoder->bindRenderPipeline(subcomponentRenderPipeline);
			commandEncoder->bindDescriptorSets(subcomponentPipelineLayout, {subcomponentDescriptorSet});
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
			commandEncoder->bindVertexBuffer(1, subcomponentBuffer, 0, sizeof(GUISubcomponent));
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
			commandEncoder->drawIndexed(6, subcomponentCount, 0, 0, 0);
		}

		if (glyphSubcomponentCount > 0) {
			commandEncoder->bindRenderPipeline(glyphSubcomponentRenderPipeline);
			commandEncoder->bindDescriptorSets(glyphSubcomponentPipelineLayout, {glyphSubcomponentDescriptorSet});
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
			commandEncoder->bindVertexBuffer(1, glyphSubcomponentBuffer, 0, sizeof(GUIGlyphSubcomponent));
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
			commandEncoder->drawIndexed(6, glyphSubcomponentCount, 0, 0, 0);
		}

		commandEncoder->endRenderPass();
	}

	void GUIRenderer::resize(WindowResizeEvent& event) {
		recreateFramebuffers();

		updateCameraProjection();
	}

	void GUIRenderer::resetCounters() {
		subcomponentCount = 0;
		glyphSubcomponentCount = 0;
	}
}