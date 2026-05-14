// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/GUI/GUIRenderer.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

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

	void GUIRenderer::flushTextureArrayStagingBuffer() {
		DEBUG_ASSERT(!textureArrayFlushed);
		DEBUG_ASSERT(textureArrayLayerCount > 0);

		textureArrayStagingBuffer->unmap();

		sequence<BufferImageCopyRegion> copyRegions(textureArrayLayerCount);
		uint64 bufferOffset = 0;
		uint32 layerIndex = 0;

		while (layerIndex < textureArrayLayerCount) {
			const Extent2D& extent = textureArrayLayerExtents[layerIndex];

			copyRegions[layerIndex] = {
				.bufferOffset = bufferOffset,
				.imageOffsetX = 0,
				.imageOffsetY = 0,
				.imageOffsetZ = 0,
				.imageExtent = {extent.width, extent.height, 1},
				.imageMipLevel = 0,
				.imageLayerOffset = layerIndex,
				.imageLayerCount = 1,
			};

			bufferOffset += extent.width * extent.height * getChannelCount(ImageFormat::R8G8B8A8_UNORM);
			layerIndex += 1;
		}

		const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy texture array layers",
			#endif
		});

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

		textureArrayFlushed = true;
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
		renderPass = createRenderPass({
			.attachments = {
				{
					.format = getSurface()->getFormat(),
					.sampleCount = 1,

					// TODO(layer-superposition)
					// TODO: Using LOAD when this layer is the only one registered will generate non-blocking errors from the Vulkan validation layers.
					.loadOperation = LoadOperation::LOAD,
					// .loadOperation = LoadOperation::CLEAR,

					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::PRESENT_SRC,
				},
				{
					.format = ImageFormat::D32_SFLOAT,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::UNDEFINED,
					.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				}
			},
			.subpasses = {
				createSubpass({
					.colorAttachmentReferences = {
						{
							.index = 0,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
					.depthStencilAttachmentReference = {
						.index = 1,
						.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					},
				}),
			},
		});
	}

	void GUIRenderer::createDepthStencilImage() {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		depthStencilImage = renderPass->createDepthStencilAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = {swapchainExtent.width, swapchainExtent.height, 1},
				.mipLevelCount = 1,
				.layerCount = 1,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Depth stencil image",
				#endif
			},
			0
		);
	}

	void GUIRenderer::createDepthStencilImageView() {
		depthStencilImageView = depthStencilImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Depth stencil image view",
			#endif
		});
	}

	void GUIRenderer::recreateFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor = {
			.renderPass = renderPass,
			.imageViews = {nullptr, depthStencilImageView},
			.extent = swapchain->getExtent(),
		};

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
		cameraBuffer = createBuffer<Camera>({
			.size = sizeof(Camera),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Camera",
			#endif
		});
		cameraBufferMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());
	}

	void GUIRenderer::createVertexBuffer() {
		vertexBuffer = createBuffer<Vertex>({
			.size = VERTEX_BUFFER_SIZE,
			.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex",
			#endif
		});
	}

	void GUIRenderer::createIndexBuffer() {
		indexBuffer = createBuffer<uint16>({
			.size = INDEX_BUFFER_SIZE,
			.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Index",
			#endif
		});
	}

	void GUIRenderer::createSubcomponentBuffer() {
		subcomponentBuffer = createBuffer<GUISubcomponent>({
			.size = SUBCOMPONENT_BUFFER_SIZE,
			.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Subcomponent",
			#endif
		});
		subcomponentBufferMappedRange = subcomponentBuffer->map(0, subcomponentBuffer->getSize());
	}

	void GUIRenderer::createGlyphSubcomponentBuffer() {
		glyphSubcomponentBuffer = createBuffer<GUIGlyphSubcomponent>({
			.size = GLYPH_SUBCOMPONENT_BUFFER_SIZE,
			.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph subcomponent",
			#endif
		});
		glyphSubcomponentBufferMappedRange = glyphSubcomponentBuffer->map(0, glyphSubcomponentBuffer->getSize());
	}

	void GUIRenderer::createTextureArrayStagingBuffer() {
		textureArrayStagingBuffer = createStagingBuffer<uint8>(TEXTURE_ARRAY_MAX_WIDTH * TEXTURE_ARRAY_MAX_HEIGHT * TEXTURE_ARRAY_MAX_LAYER_COUNT * TEXTURE_ARRAY_LAYER_BPP);
		textureArrayStagingBufferMappedRange = textureArrayStagingBuffer->map(0, textureArrayStagingBuffer->getSize());
	}

	void GUIRenderer::createTextureArrayImage() {
		textureArrayImage = createImage({
			.type = ImageType::TEXTURE_2D,
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {TEXTURE_ARRAY_MAX_WIDTH, TEXTURE_ARRAY_MAX_HEIGHT, 1},
			.mipLevelCount = 1,
			.layerCount = TEXTURE_ARRAY_MAX_LAYER_COUNT,
			.sampleCount = 1,
			.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Texture array",
			#endif
		});
	}

	void GUIRenderer::createTextureArrayImageView() {
		textureArrayImageView = textureArrayImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = textureArrayImage->getLayerCount(),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Texture array",
			#endif
		});
	}

	void GUIRenderer::createNearestSampler() {
		nearestSampler = createSampler({
			.magFilter = SamplerFilter::NEAREST,
			.minFilter = SamplerFilter::NEAREST,
		});
	}

	void GUIRenderer::createLinearSampler() {
		linearSampler = createSampler({
			.magFilter = SamplerFilter::LINEAR,
			.minFilter = SamplerFilter::LINEAR,
		});
	}

	void GUIRenderer::createDescriptorSetLayouts() {
		const DescriptorSetLayoutBinding cameraBinding = {
			.descriptorType = DescriptorType::UNIFORM_BUFFER,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::VERTEX,
		};

		const DescriptorSetLayoutBinding textureArrayBinding = {
			.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::FRAGMENT,
		};

		subcomponentDescriptorSetLayout = createDescriptorSetLayout({cameraBinding, textureArrayBinding});
	}

	void GUIRenderer::createSubcomponentPipelineLayout() {
		subcomponentPipelineLayout = createPipelineLayout({
			.descriptorSetLayouts = {subcomponentDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Subcomponent",
			#endif
		});
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

		subcomponentGraphicsPipeline = createGraphicsPipeline({
			.pipelineLayout = subcomponentPipelineLayout,
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
				.attachments = colorBlendAttachments,
			},
			.depthStencilState = {
				.depthTest = true,
				.depthWrite = true,
				.compareOperation = CompareOperation::GREATER_OR_EQUAL,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Subcomponent",
			#endif
		});
	}

	void GUIRenderer::createGlyphSubcomponentGraphicsPipeline() {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec4<float32>>(); // Vertex position and texture coordinate

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // World transform
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // Texture transform
		instanceInputBinding.registerAttribute<vec4<float32>>(); // Color
		instanceInputBinding.registerAttribute<uint32>(); // Z-index
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

		glyphSubcomponentGraphicsPipeline = createGraphicsPipeline({
			.pipelineLayout = subcomponentPipelineLayout,
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
				.attachments = colorBlendAttachments,
			},
			.depthStencilState = {
				.depthTest = true,
				.depthWrite = true,
				.compareOperation = CompareOperation::GREATER_OR_EQUAL,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph subcomponent",
			#endif
		});
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
		if (subcomponentCount + glyphSubcomponentCount == 0) {
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