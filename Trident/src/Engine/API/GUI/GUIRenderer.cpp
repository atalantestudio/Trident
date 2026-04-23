// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/API/GUI/GUIRenderer.hpp"

namespace Trident {
	void GUIRenderer::build() {
		setupRenderPass();

		createDepthStencilImage();
		createDepthStencilImageView();

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

		createSubcomponentGraphicsPipeline();
		createGlyphSubcomponentGraphicsPipeline();

		updateDescriptorSets();
		updateCameraProjection();
	}

	Extent2D GUIRenderer::getTextureArrayLayerExtent(uint32 layerIndex) const {
		DEBUG_ASSERT(layerIndex < textureArrayLayerCount);

		return textureArrayLayerExtents[layerIndex];
	}

	void GUIRenderer::flushTextureArrayStagingBuffer() const {
		DEBUG_ASSERT(textureArrayLayerCount > 0);

		textureArrayStagingBuffer->unmap();

		sequence<BufferImageCopyRegion> copyRegions(textureArrayLayerCount);
		uint64 bufferOffset = 0;
		uint32 layerIndex = 0;

		while (layerIndex < textureArrayLayerCount) {
			const Extent2D& extent = textureArrayLayerExtents[layerIndex];

			BufferImageCopyRegion& copyRegion = copyRegions[layerIndex];
			copyRegion.bufferOffset = bufferOffset;
			copyRegion.imageOffsetX = 0;
			copyRegion.imageOffsetY = 0;
			copyRegion.imageOffsetZ = 0;
			copyRegion.imageExtent.width = extent.width;
			copyRegion.imageExtent.height = extent.height;
			copyRegion.imageExtent.depth = 1;
			copyRegion.imageMipLevel = 0;
			copyRegion.imageLayerOffset = layerIndex;
			copyRegion.imageLayerCount = 1;

			bufferOffset += extent.width * extent.height * getChannelCount(ImageFormat::R8G8B8A8_UNORM);
			layerIndex += 1;
		}

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		#if defined(DEBUG_OBJECT_NAME)
			commandBufferDescriptor.name = "Copy texture array layers";
		#endif

		const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

		ImageMemoryBarrier textureArrayImageMemoryBarrier = {
			.image = textureArrayImage,
			.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
			.newAccess = Access::TRANSFER_WRITE,
		};

		commandEncoder->begin();
		commandEncoder->insertMemoryBarriers({}, textureArrayImageMemoryBarrier);
		commandEncoder->copyBufferToImage(textureArrayStagingBuffer, textureArrayImage, copyRegions);

		textureArrayImageMemoryBarrier = {
			.image = textureArrayImage,
			.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
			.newAccess = Access::SHADER_READ,
		};

		commandEncoder->insertMemoryBarriers({}, textureArrayImageMemoryBarrier);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		graphicsQueue->submit(commandBuffer);
		graphicsQueue->waitIdle();

		textureArrayStagingBuffer.~shared_ptr();
	}

	uint32 GUIRenderer::registerTextureArrayLayer(const TextureArrayLayer& layer) {
		ENGINE_ASSERT(textureArrayLayerCount + 1 <= TEXTURE_ARRAY_MAX_LAYER_COUNT, "Too many client-side images.");
		ENGINE_ASSERT(layer.extent.width <= TEXTURE_ARRAY_MAX_WIDTH, "Image width ([]) is larger than the maximum width ([]).", layer.extent.width, TEXTURE_ARRAY_MAX_WIDTH);
		ENGINE_ASSERT(layer.extent.height <= TEXTURE_ARRAY_MAX_HEIGHT, "Image height ([]) is larger than the maximum height ([]).", layer.extent.height, TEXTURE_ARRAY_MAX_HEIGHT);

		const uint32 layerIndex = textureArrayLayerCount;

		std::copy(layer.data.begin(), layer.data.end(), textureArrayStagingBufferMappedRange);

		textureArrayStagingBufferMappedRange += layer.data.count();
		textureArrayLayerExtents[textureArrayLayerCount] = layer.extent;
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
		// swapchainAttachment.loadOperation = LoadOperation::LOAD;
		swapchainAttachment.loadOperation = LoadOperation::CLEAR;

		swapchainAttachment.storeOperation = StoreOperation::STORE;
		swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

		Attachment depthStencilAttachment{};
		depthStencilAttachment.format = ImageFormat::D32_SFLOAT;
		depthStencilAttachment.sampleCount = 1;
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

	void GUIRenderer::createDepthStencilImage() {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		AttachmentImageDescriptor depthStencilAttachmentImageDescriptor{};
		depthStencilAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		depthStencilAttachmentImageDescriptor.extent.width = swapchainExtent.width;
		depthStencilAttachmentImageDescriptor.extent.height = swapchainExtent.height;
		depthStencilAttachmentImageDescriptor.extent.depth = 1;
		depthStencilAttachmentImageDescriptor.mipLevelCount = 1;
		depthStencilAttachmentImageDescriptor.layerCount = 1;

		#if defined(DEBUG_OBJECT_NAME)
			depthStencilAttachmentImageDescriptor.name = "Depth stencil image";
		#endif

		depthStencilImage = renderPass->createDepthStencilAttachmentImage(depthStencilAttachmentImageDescriptor, 0);
	}

	void GUIRenderer::createDepthStencilImageView() {
		ImageViewDescriptor depthStencilImageViewDescriptor{};
		depthStencilImageViewDescriptor.mipLevelOffset = 0;
		depthStencilImageViewDescriptor.mipLevelCount = 1;
		depthStencilImageViewDescriptor.layerOffset = 0;
		depthStencilImageViewDescriptor.layerCount = 1;

		#if defined(DEBUG_OBJECT_NAME)
			depthStencilImageViewDescriptor.name = "Depth stencil image view";
		#endif

		depthStencilImageView = depthStencilImage->createView(depthStencilImageViewDescriptor);
	}

	void GUIRenderer::recreateFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor{};
		framebufferDescriptor.renderPass = renderPass;
		framebufferDescriptor.imageViews = {nullptr, depthStencilImageView};
		framebufferDescriptor.extent = swapchain->getExtent();

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 framebufferIndex = 0;

		while (framebufferIndex < framebuffers.count()) {
			#if defined(DEBUG_OBJECT_NAME)
				name = Logger::format("In flight framebuffer []", framebufferIndex);

				framebufferDescriptor.name = name;
			#endif

			framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

			framebufferIndex += 1;
		}
	}

	void GUIRenderer::createCameraBuffer() {
		BufferDescriptor cameraBufferDescriptor{};
		cameraBufferDescriptor.size = sizeof(Camera);
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		cameraBufferDescriptor.persistent = true;

		#if defined(DEBUG_OBJECT_NAME)
			cameraBufferDescriptor.name = "Camera";
		#endif

		cameraBuffer = createBuffer<Camera>(cameraBufferDescriptor);

		cameraBufferMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());
	}

	void GUIRenderer::createVertexBuffer() {
		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = VERTEX_BUFFER_SIZE;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		#if defined(DEBUG_OBJECT_NAME)
			vertexBufferDescriptor.name = "Vertex";
		#endif

		vertexBuffer = createBuffer<Vertex>(vertexBufferDescriptor);
	}

	void GUIRenderer::createIndexBuffer() {
		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = INDEX_BUFFER_SIZE;
		indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
		indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		#if defined(DEBUG_OBJECT_NAME)
			indexBufferDescriptor.name = "Index";
		#endif

		indexBuffer = createBuffer<uint16>(indexBufferDescriptor);
	}

	void GUIRenderer::createSubcomponentBuffer() {
		BufferDescriptor subcomponentBufferDescriptor{};
		subcomponentBufferDescriptor.size = SUBCOMPONENT_BUFFER_SIZE;
		subcomponentBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		subcomponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		subcomponentBufferDescriptor.persistent = true;

		#if defined(DEBUG_OBJECT_NAME)
			subcomponentBufferDescriptor.name = "Subcomponent";
		#endif

		subcomponentBuffer = createBuffer<GUISubcomponent>(subcomponentBufferDescriptor);

		subcomponentBufferMappedRange = subcomponentBuffer->map(0, subcomponentBuffer->getSize());
	}

	void GUIRenderer::createGlyphSubcomponentBuffer() {
		BufferDescriptor glyphSubcomponentBufferDescriptor{};
		glyphSubcomponentBufferDescriptor.size = GLYPH_SUBCOMPONENT_BUFFER_SIZE;
		glyphSubcomponentBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		glyphSubcomponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphSubcomponentBufferDescriptor.persistent = true;

		#if defined(DEBUG_OBJECT_NAME)
			glyphSubcomponentBufferDescriptor.name = "Glyph subcomponent";
		#endif

		glyphSubcomponentBuffer = createBuffer<GUIGlyphSubcomponent>(glyphSubcomponentBufferDescriptor);

		glyphSubcomponentBufferMappedRange = glyphSubcomponentBuffer->map(0, glyphSubcomponentBuffer->getSize());
	}

	void GUIRenderer::createTextureArrayStagingBuffer() {
		textureArrayStagingBuffer = createStagingBuffer<uint8>(TEXTURE_ARRAY_MAX_WIDTH * TEXTURE_ARRAY_MAX_HEIGHT * TEXTURE_ARRAY_MAX_LAYER_COUNT * TEXTURE_ARRAY_LAYER_BPP);

		textureArrayStagingBufferMappedRange = textureArrayStagingBuffer->map(0, textureArrayStagingBuffer->getSize());
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

		#if defined(DEBUG_OBJECT_NAME)
			textureArrayImageDescriptor.name = "Texture array";
		#endif

		textureArrayImage = createImage(textureArrayImageDescriptor);
	}

	void GUIRenderer::createTextureArrayImageView() {
		ImageViewDescriptor textureArrayImageViewDescriptor{};
		textureArrayImageViewDescriptor.mipLevelOffset = 0;
		textureArrayImageViewDescriptor.mipLevelCount = 1;
		textureArrayImageViewDescriptor.layerOffset = 0;
		textureArrayImageViewDescriptor.layerCount = textureArrayImage->getLayerCount();

		#if defined(DEBUG_OBJECT_NAME)
			textureArrayImageViewDescriptor.name = "Texture array";
		#endif

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

		DescriptorSetLayoutBinding textureArrayBinding{};
		textureArrayBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		textureArrayBinding.descriptorCount = 1;
		textureArrayBinding.shaderStages = ShaderStage::FRAGMENT;

		subcomponentDescriptorSetLayout = createDescriptorSetLayout({cameraBinding, textureArrayBinding});
	}

	void GUIRenderer::createSubcomponentPipelineLayout() {
		PipelineLayoutDescriptor subcomponentPipelineLayoutDescriptor{};
		subcomponentPipelineLayoutDescriptor.descriptorSetLayouts = {subcomponentDescriptorSetLayout};

		#if defined(DEBUG_OBJECT_NAME)
			subcomponentPipelineLayoutDescriptor.name = "Subcomponent";
		#endif

		subcomponentPipelineLayout = createPipelineLayout(subcomponentPipelineLayoutDescriptor);
	}

	void GUIRenderer::createSubcomponentGraphicsPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec4<float32>>(); // Vertex position and texture coordinate

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // World transform
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // Texture transform
		instanceInputBinding.registerAttribute<vec4<float32>>(); // Color
		instanceInputBinding.registerAttribute<uint32>(); // Z-index
		instanceInputBinding.registerAttribute<uint32>(); // Texture array layer index

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		#if defined(DEBUG_OBJECT_NAME)
			vertexShaderModuleDescriptor.name = "Subcomponent vertex";
		#endif

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		#if defined(DEBUG_OBJECT_NAME)
			fragmentShaderModuleDescriptor.name = "Subcomponent fragment";
		#endif

		const ref<ShaderModule> vertexShaderModule = loadInternalShaderModule(vertexShaderModuleDescriptor, "GUI/Subcomponent.vert");
		const ref<ShaderModule> fragmentShaderModule = loadInternalShaderModule(fragmentShaderModuleDescriptor, "GUI/Subcomponent.frag");

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

		GraphicsPipelineDescriptor subcomponentGraphicsPipelineDescriptor{};
		subcomponentGraphicsPipelineDescriptor.pipelineLayout = subcomponentPipelineLayout;
		subcomponentGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		subcomponentGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		subcomponentGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		subcomponentGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		subcomponentGraphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
		subcomponentGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
		subcomponentGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		subcomponentGraphicsPipelineDescriptor.lineWidth = 1.0f;
		subcomponentGraphicsPipelineDescriptor.minSampleShading = 0.0f;
		subcomponentGraphicsPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
		subcomponentGraphicsPipelineDescriptor.depthStencilState.depthTest = true;
		subcomponentGraphicsPipelineDescriptor.depthStencilState.depthWrite = true;
		subcomponentGraphicsPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER_OR_EQUAL;
		subcomponentGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		subcomponentGraphicsPipelineDescriptor.renderPass = renderPass;
		subcomponentGraphicsPipelineDescriptor.subpassIndex = 0;

		#if defined(DEBUG_OBJECT_NAME)
			subcomponentGraphicsPipelineDescriptor.name = "Subcomponent";
		#endif

		subcomponentGraphicsPipeline = createGraphicsPipeline(subcomponentGraphicsPipelineDescriptor);
	}

	void GUIRenderer::createGlyphSubcomponentGraphicsPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec4<float32>>(); // Vertex position and texture coordinate

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // World transform
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // Texture transform
		instanceInputBinding.registerAttribute<vec4<float32>>(); // Color
		instanceInputBinding.registerAttribute<uint32>(); // Layer index
		instanceInputBinding.registerAttribute<float32>(); // Font size

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		#if defined(DEBUG_OBJECT_NAME)
			vertexShaderModuleDescriptor.name = "Glyph subcomponent vertex";
		#endif

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		#if defined(DEBUG_OBJECT_NAME)
			fragmentShaderModuleDescriptor.name = "Glyph subcomponent fragment";
		#endif

		const ref<ShaderModule> vertexShaderModule = loadInternalShaderModule(vertexShaderModuleDescriptor, "GUI/GlyphSubcomponent.vert");
		const ref<ShaderModule> fragmentShaderModule = loadInternalShaderModule(fragmentShaderModuleDescriptor, "GUI/GlyphSubcomponent.frag");

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

		GraphicsPipelineDescriptor glyphSubcomponentGraphicsPipelineDescriptor{};
		glyphSubcomponentGraphicsPipelineDescriptor.pipelineLayout = subcomponentPipelineLayout;
		glyphSubcomponentGraphicsPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		glyphSubcomponentGraphicsPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		glyphSubcomponentGraphicsPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		glyphSubcomponentGraphicsPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		glyphSubcomponentGraphicsPipelineDescriptor.polygonMode = PolygonMode::FILL;
		glyphSubcomponentGraphicsPipelineDescriptor.cullMode = CullMode::BACK;
		glyphSubcomponentGraphicsPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		glyphSubcomponentGraphicsPipelineDescriptor.lineWidth = 1.0f;
		glyphSubcomponentGraphicsPipelineDescriptor.minSampleShading = 0.0f;
		glyphSubcomponentGraphicsPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
		glyphSubcomponentGraphicsPipelineDescriptor.depthStencilState.depthTest = false;
		glyphSubcomponentGraphicsPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		glyphSubcomponentGraphicsPipelineDescriptor.renderPass = renderPass;
		glyphSubcomponentGraphicsPipelineDescriptor.subpassIndex = 0;

		#if defined(DEBUG_OBJECT_NAME)
			glyphSubcomponentGraphicsPipelineDescriptor.name = "Glyph subcomponent";
		#endif

		glyphSubcomponentGraphicsPipeline = createGraphicsPipeline(glyphSubcomponentGraphicsPipelineDescriptor);
	}

	void GUIRenderer::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(sequence<ref<DescriptorSetLayout>>{
			subcomponentDescriptorSetLayout,
			subcomponentDescriptorSetLayout,
		});

		subcomponentDescriptorSet = descriptorSets[0];
		glyphSubcomponentDescriptorSet = descriptorSets[1];
	}

	void GUIRenderer::copyBuffers() const {
		const ref<Buffer<uint8>> stagingBuffer = createStagingBuffer<uint8>(VERTEX_BUFFER_SIZE + INDEX_BUFFER_SIZE);

		uint8* mappedRange = stagingBuffer->map(0, stagingBuffer->getSize());

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

		static constexpr uint16 INDICES[] = {0, 1, 2, 0, 2, 3};

		std::copy_n(INDICES, INDEX_BUFFER_SIZE, indexMappedRange);

		stagingBuffer->unmap();

		const sequence<BufferCopyRegion> copyRegions = {
			// Vertex
			{
				.sourceOffset = 0,
				.destinationOffset = 0,
				.size = vertexBuffer->getSize(),
			},

			// Index
			{
				.sourceOffset = vertexBuffer->getSize(),
				.destinationOffset = 0,
				.size = indexBuffer->getSize(),
			},
		};

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy buffers",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, copyRegions[0]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, copyRegions[1]);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit(commandBuffer);
		transferQueue->waitIdle();
	}

	void GUIRenderer::updateDescriptorSets() const {
		const DescriptorBuffer cameraDescriptorBuffer = {
			.buffer = cameraBuffer,
			.offset = 0,
			.size = cameraBuffer->getSize(),
		};
		const DescriptorImage nearestTextureArrayDescriptorImage = {
			.imageView = textureArrayImageView,
			.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
			.sampler = nearestSampler,
		};
		const DescriptorImage linearTextureArrayDescriptorImage = {
			.imageView = textureArrayImageView,
			.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
			.sampler = linearSampler,
		};

		const sequence<WriteDescriptorSet> writeDescriptorSets = {
			{
				.descriptorSet = subcomponentDescriptorSet,
				.bindingIndex = 0,
				.descriptorBuffers = cameraDescriptorBuffer,
			},
			{
				.descriptorSet = subcomponentDescriptorSet,
				.bindingIndex = 1,
				.descriptorImages = nearestTextureArrayDescriptorImage,
			},
			{
				.descriptorSet = glyphSubcomponentDescriptorSet,
				.bindingIndex = 0,
				.descriptorBuffers = cameraDescriptorBuffer,
			},
			{
				.descriptorSet = glyphSubcomponentDescriptorSet,
				.bindingIndex = 1,
				.descriptorImages = linearTextureArrayDescriptorImage,
			},
		};

		getDevice()->updateDescriptorSets(writeDescriptorSets);
	}

	void GUIRenderer::updateCameraProjection() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		// TODO: lyah::mat<3, 3, float32>::orthographic
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

		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(swapchainExtent.width);
		const float32 height = static_cast<float32>(swapchainExtent.height);

		commandEncoder->beginRenderPass({
			.framebuffer = framebuffers[getSwapchainImageIndex()],
			.clearValues = {
				{0.0f, 0.0f, 0.0f, 1.0f},
				{
					.depth = 0.0f,
					.stencil = 0,
				},
			},
		});
		commandEncoder->setScissor(0, 0, swapchainExtent);
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);

		if (subcomponentCount > 0) {
			commandEncoder->bindGraphicsPipeline(subcomponentGraphicsPipeline);
			commandEncoder->bindDescriptorSets(subcomponentPipelineLayout, 0, subcomponentDescriptorSet);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
			commandEncoder->bindVertexBuffer(1, subcomponentBuffer, 0, sizeof(GUISubcomponent));
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
			commandEncoder->drawIndexed(6, subcomponentCount, 0, 0, 0);
		}

		if (glyphSubcomponentCount > 0) {
			commandEncoder->bindGraphicsPipeline(glyphSubcomponentGraphicsPipeline);
			commandEncoder->bindDescriptorSets(subcomponentPipelineLayout, 0, glyphSubcomponentDescriptorSet);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
			commandEncoder->bindVertexBuffer(1, glyphSubcomponentBuffer, 0, sizeof(GUIGlyphSubcomponent));
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
			commandEncoder->drawIndexed(6, glyphSubcomponentCount, 0, 0, 0);
		}

		commandEncoder->endRenderPass();
	}

	void GUIRenderer::resize(WindowResizeEvent& event) {
		createDepthStencilImage();
		createDepthStencilImageView();
		recreateFramebuffers();
		updateCameraProjection();
	}

	void GUIRenderer::resetCounters() {
		subcomponentCount = 0;
		glyphSubcomponentCount = 0;
	}
}