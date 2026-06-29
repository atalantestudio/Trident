// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/GUI/GUIRenderer.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	void GUIRenderer::build() {
		renderPass = createRenderPass();

		depthStencilImage = createDepthStencilImage();
		depthStencilImageView = createDepthStencilImageView();

		recreateFramebuffers();

		vertexBuffer = createVertexBuffer();
		indexBuffer = createIndexBuffer();

		cameraBuffer = createCameraBuffer();
		cameraBufferMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());

		subcomponentBuffer = createSubcomponentBuffer();
		subcomponentBufferMappedRange = subcomponentBuffer->map(0, subcomponentBuffer->getSize());

		glyphSubcomponentBuffer = createGlyphSubcomponentBuffer();
		glyphSubcomponentBufferMappedRange = glyphSubcomponentBuffer->map(0, glyphSubcomponentBuffer->getSize());

		userImageStagingBuffer = createUserImageStagingBuffer();
		userImageStagingBufferMappedRange = userImageStagingBuffer->map(0, userImageStagingBuffer->getSize());

		userImages = sequence<ref<Image>>(MAX_USER_IMAGE_COUNT);
		userImageViews = sequence<ref<ImageView>>(MAX_USER_IMAGE_COUNT);

		nearestSampler = createNearestSampler();
		linearSampler = createLinearSampler();

		subcomponentDescriptorSetLayout = createSubcomponentDescriptorSetLayout();

		const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(subcomponentDescriptorSetLayout);

		subcomponentDescriptorSet = descriptorSets[0];

		copyBuffers();

		subcomponentPipelineLayout = createSubcomponentPipelineLayout();

		subcomponentGraphicsPipeline = createSubcomponentGraphicsPipeline();
		glyphSubcomponentGraphicsPipeline = createGlyphSubcomponentGraphicsPipeline();

		updateCameraProjection();
	}

	ref<Image> GUIRenderer::getUserImage(uint32 imageIndex) const {
		DEBUG_ASSERT(imageIndex < userImageCount);

		return userImages[imageIndex];
	}

	void GUIRenderer::flushUserImageStagingBuffer() {
		DEBUG_ASSERT(!userImagesFlushed);

		userImageStagingBuffer->unmap();

		const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy user images",
			#endif
		});

		commandEncoder->begin();

		sequence<ImageMemoryBarrier> userImageMemoryBarriers(userImageCount);
		uint32 userImageIndex = 0;
		BufferImageCopyRegion userImageCopyRegion;
		uint64 bufferOffset = 0;

		while (userImageIndex < userImageCount) {
			userImageMemoryBarriers[userImageIndex] = {
				.image = userImages[userImageIndex],
				.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
				.newAccess = Access::TRANSFER_WRITE,
			};

			userImageIndex += 1;
		}

		commandEncoder->insertMemoryBarriers({}, userImageMemoryBarriers);

		userImageIndex = 0;

		while (userImageIndex < userImageCount) {
			const ref<Image> userImage = userImages[userImageIndex];

			userImageCopyRegion = {
				.bufferOffset = bufferOffset,
				.imageOffsetX = 0,
				.imageOffsetY = 0,
				.imageOffsetZ = 0,
				.imageExtent = userImage->getExtent(),
				.imageMipLevel = 0,
				.imageLayerOffset = 0,
				.imageLayerCount = 1,
			};

			userImageMemoryBarriers[userImageIndex].newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
			userImageMemoryBarriers[userImageIndex].newAccess = Access::SHADER_READ;

			commandEncoder->copyBufferToImage(userImageStagingBuffer, userImages[userImageIndex], userImageCopyRegion);

			bufferOffset += userImage->getSize();
			userImageIndex += 1;
		}

		commandEncoder->insertMemoryBarriers({}, userImageMemoryBarriers);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		graphicsQueue->submit(commandBuffer);
		graphicsQueue->waitIdle();

		userImageStagingBuffer.~shared_ptr();

		updateDescriptorSets();

		userImagesFlushed = true;
	}

	void GUIRenderer::registerDefaultUserImage() {
		const ImageID userImageIndex = userImageCount;

		DEBUG_ASSERT(userImageIndex < MAX_USER_IMAGE_COUNT);

		#if defined(DEBUG_OBJECT_NAME)
			const sequence<char8> name = Logger::format("User image [] (default)", userImageIndex);
		#endif

		userImages[userImageIndex] = createImage({
			.type = ImageType::TEXTURE_2D,
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {1, 1, 1},
			.mipLevelCount = 1,
			.layerCount = 1,
			.sampleCount = 1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = name,
			#endif
		});
		userImageViews[userImageIndex] = userImages[userImageIndex]->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = name,
			#endif
		});

		const sequence<uint8> data = {255, 255, 255, 255};

		std::copy(data.begin(), data.end(), userImageStagingBufferMappedRange);

		userImageStagingBufferMappedRange += data.count();
		userImageCount += 1;
	}

	ImageID GUIRenderer::registerUserImage(ref<ImageAsset> imageAsset) {
		const ImageID userImageIndex = userImageCount;

		DEBUG_ASSERT(userImageIndex < MAX_USER_IMAGE_COUNT);

		#if defined(DEBUG_OBJECT_NAME)
			const sequence<char8> name = Logger::format("User []", userImageIndex);
		#endif

		ImageFormat format = imageAsset->getFormat();

		if (format == ImageFormat::R8G8B8_UNORM) {
			format = ImageFormat::R8G8B8A8_UNORM;

			const uint64 size = imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * getChannelCount(ImageFormat::R8G8B8A8_UNORM);

			convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), userImageStagingBufferMappedRange, size);

			userImageStagingBufferMappedRange += size;
		}
		else {
			std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), userImageStagingBufferMappedRange);

			userImageStagingBufferMappedRange += imageAsset->getSize();
		}

		userImages[userImageIndex] = createImage({
			.type = ImageType::TEXTURE_2D,
			.format = format,
			.extent = imageAsset->getExtent(),
			.mipLevelCount = 1,
			.layerCount = 1,
			.sampleCount = 1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = name,
			#endif
		});
		userImageViews[userImageIndex] = userImages[userImageIndex]->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = name,
			#endif
		});

		userImageCount += 1;

		return userImageIndex;
	}

	void GUIRenderer::registerSubcomponent(const GUISubcomponent& subcomponent) {
		DEBUG_ASSERT(subcomponentCount < MAX_SUBCOMPONENT_COUNT);

		subcomponentBufferMappedRange[subcomponentCount] = subcomponent;
		subcomponentCount += 1;
	}

	void GUIRenderer::registerGlyphSubcomponent(const GUIGlyphSubcomponent& glyphSubcomponent) {
		DEBUG_ASSERT(glyphSubcomponentCount < MAX_GLYPH_SUBCOMPONENT_COUNT);

		glyphSubcomponentBufferMappedRange[glyphSubcomponentCount] = glyphSubcomponent;
		glyphSubcomponentCount += 1;
	}

	ref<RenderPass> GUIRenderer::createRenderPass() const {
		return Renderer::createRenderPass({
			.attachments = {
				{
					.format = getSurface()->getFormat(),
					.sampleCount = 1,

					// TODO(layer-superposition): Using LOAD when this layer is the only one registered will generate non-blocking errors from the Vulkan validation layers.
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
				},
			},
			.subpasses = {
				{
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
				},
			},
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

	ref<Buffer<GUIRenderer::Vertex>> GUIRenderer::createVertexBuffer() const {
		return createBuffer<Vertex>({
			.size = sizeof(Vertex) * 4,
			.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex",
			#endif
		});
	}

	ref<Buffer<uint16>> GUIRenderer::createIndexBuffer() const {
		return createBuffer<uint16>({
			.size = sizeof(uint16) * 6,
			.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Index",
			#endif
		});
	}

	ref<Buffer<GUIRenderer::Camera>> GUIRenderer::createCameraBuffer() const {
		return createBuffer<Camera>({
			.size = sizeof(Camera),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Camera",
			#endif
		});
	}

	ref<Buffer<GUISubcomponent>> GUIRenderer::createSubcomponentBuffer() const {
		return createBuffer<GUISubcomponent>({
			.size = sizeof(GUISubcomponent) * MAX_SUBCOMPONENT_COUNT,
			.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Subcomponent",
			#endif
		});
	}

	ref<Buffer<GUIGlyphSubcomponent>> GUIRenderer::createGlyphSubcomponentBuffer() const {
		return createBuffer<GUIGlyphSubcomponent>({
			.size = sizeof(GUIGlyphSubcomponent) * MAX_GLYPH_SUBCOMPONENT_COUNT,
			.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph subcomponent",
			#endif
		});
	}

	ref<Image> GUIRenderer::createDepthStencilImage() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		return renderPass->createDepthStencilAttachmentImage(
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

	ref<ImageView> GUIRenderer::createDepthStencilImageView() const {
		return depthStencilImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Depth stencil image view",
			#endif
		});
	}

	ref<Buffer<uint8>> GUIRenderer::createUserImageStagingBuffer() const {
		// Can accomodate for 8 2048x2048 RGBA textures.
		return createStagingBuffer<uint8>(134217728);
	}

	ref<Sampler> GUIRenderer::createNearestSampler() const {
		return createSampler({
			.magFilter = SamplerFilter::NEAREST,
			.minFilter = SamplerFilter::NEAREST,
		});
	}

	ref<Sampler> GUIRenderer::createLinearSampler() const {
		return createSampler({
			.magFilter = SamplerFilter::LINEAR,
			.minFilter = SamplerFilter::LINEAR,
		});
	}

	ref<DescriptorSetLayout> GUIRenderer::createSubcomponentDescriptorSetLayout() const {
		const sequence<DescriptorSetLayoutBinding> bindings = {
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX,
			},

			// User images
			{
				.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
				.descriptorCount = MAX_USER_IMAGE_COUNT,
				.shaderStages = ShaderStage::FRAGMENT,
			},
		};

		return createDescriptorSetLayout(bindings);
	}

	ref<PipelineLayout> GUIRenderer::createSubcomponentPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {subcomponentDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Subcomponent",
			#endif
		});
	}

	ref<GraphicsPipeline> GUIRenderer::createSubcomponentGraphicsPipeline() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec4<float32>>(); // Vertex position and texture coordinate

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // World transform
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // Texture transform
		instanceInputBinding.registerAttribute<vec4<float32>>(); // Color
		instanceInputBinding.registerAttribute<uint32>(); // Z-index
		instanceInputBinding.registerAttribute<uint32>(); // Texture array layer index

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = subcomponentPipelineLayout,
			.vertexInputBindings = {vertexInputBinding, instanceInputBinding},
			.vertexShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",

					#if defined(DEBUG_OBJECT_NAME)
						.name = "Subcomponent vertex",
					#endif
				},
				"GUI/Subcomponent.vert"
			),
			.fragmentShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",

					#if defined(DEBUG_OBJECT_NAME)
						.name = "Subcomponent fragment",
					#endif
				},
				"GUI/Subcomponent.frag"
			),
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

	ref<GraphicsPipeline> GUIRenderer::createGlyphSubcomponentGraphicsPipeline() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec4<float32>>(); // Vertex position and texture coordinate

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // World transform
		instanceInputBinding.registerAttribute<mat3x3<float32>>(); // Texture transform
		instanceInputBinding.registerAttribute<vec4<float32>>(); // Color
		instanceInputBinding.registerAttribute<uint32>(); // Z-index
		instanceInputBinding.registerAttribute<uint32>(); // Layer index
		instanceInputBinding.registerAttribute<float32>(); // Font size

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = subcomponentPipelineLayout,
			.vertexInputBindings = {vertexInputBinding, instanceInputBinding},
			.vertexShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",

					#if defined(DEBUG_OBJECT_NAME)
						.name = "Glyph subcomponent vertex",
					#endif
				},
				"GUI/GlyphSubcomponent.vert"
			),
			.fragmentShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",

					#if defined(DEBUG_OBJECT_NAME)
						.name = "Glyph subcomponent fragment",
					#endif
				},
				"GUI/GlyphSubcomponent.frag"
			),
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

	void GUIRenderer::copyBuffers() const {
		const ref<Buffer<uint8>> stagingBuffer = createStagingBuffer<uint8>(vertexBuffer->getSize() + indexBuffer->getSize());

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

		uint16* indexMappedRange = pointer_cast<uint16*>(mappedRange + vertexBuffer->getSize());

		static constexpr uint16 INDICES[] = {0, 1, 2, 0, 2, 3};

		std::copy_n(INDICES, 6, indexMappedRange);

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

		sequence<DescriptorImage> userDescriptorImages(MAX_USER_IMAGE_COUNT);

		uint32 userImageIndex = 0;

		while (userImageIndex < MAX_USER_IMAGE_COUNT) {
			userDescriptorImages[userImageIndex] = {
				.imageView = userImageViews[userImageIndex % userImageCount],
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,

				// TODO(gui/image/sampler): Let the user choose the sampler for each image.
				.sampler = linearSampler,
			};

			userImageIndex += 1;
		}

		const sequence<WriteDescriptorSet> writeDescriptorSets = {
			{
				.descriptorSet = subcomponentDescriptorSet,
				.bindingIndex = 0,
				.descriptorBuffers = cameraDescriptorBuffer,
			},
			{
				.descriptorSet = subcomponentDescriptorSet,
				.bindingIndex = 1,

				// TODO: Update all images in the descriptor (Vulkan and DX12).
				.descriptorImages = userDescriptorImages,
			},
		};

		getDevice()->updateDescriptorSets(writeDescriptorSets);
	}

	void GUIRenderer::updateCameraProjection() const {
		const Extent2D extent = getSwapchain()->getExtent();

		// TODO: lyah::mat<3, 3, float32>::orthographic
		cameraBufferMappedRange->projection = orthographicTopLeftOrigin({static_cast<float32>(extent.width), static_cast<float32>(extent.height)});
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

			// TODO: Remove vertex buffer.
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));

			commandEncoder->bindVertexBuffer(1, subcomponentBuffer, 0, sizeof(GUISubcomponent));
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
			commandEncoder->drawIndexed(6, subcomponentCount, 0, 0, 0);
		}

		if (glyphSubcomponentCount > 0) {
			commandEncoder->bindGraphicsPipeline(glyphSubcomponentGraphicsPipeline);
			commandEncoder->bindDescriptorSets(subcomponentPipelineLayout, 0, subcomponentDescriptorSet);

			// TODO: Remove vertex buffer.
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));

			commandEncoder->bindVertexBuffer(1, glyphSubcomponentBuffer, 0, sizeof(GUIGlyphSubcomponent));
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
			commandEncoder->drawIndexed(6, glyphSubcomponentCount, 0, 0, 0);
		}

		commandEncoder->endRenderPass();
	}

	void GUIRenderer::resize(WindowResizeEvent& event) {
		depthStencilImage = createDepthStencilImage();
		depthStencilImageView = createDepthStencilImageView();

		recreateFramebuffers();

		updateCameraProjection();
	}

	void GUIRenderer::resetCounters() {
		subcomponentCount = 0;
		glyphSubcomponentCount = 0;
	}
}