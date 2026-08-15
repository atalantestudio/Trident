// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/GUI/GUIRenderer.hpp"
#include "Engine/API/Window/Layer/RenderPassLayer.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	void GUIRenderer::build() {
		vertexBuffer = createVertexBuffer();
		indexBuffer = createIndexBuffer();
		cameraBuffer = createCameraBuffer();
		subcomponentBuffer = createSubcomponentBuffer();
		glyphSubcomponentBuffer = createGlyphSubcomponentBuffer();
		userImageStagingBuffer = createUserImageStagingBuffer();

		const BufferView<Camera> cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());
		const BufferView<GUISubcomponent> subcomponentView = subcomponentBuffer->createView<GUISubcomponent>(0, subcomponentBuffer->getSize());
		const BufferView<GUIGlyphSubcomponent> glyphSubcomponentView = glyphSubcomponentBuffer->createView<GUIGlyphSubcomponent>(0, glyphSubcomponentBuffer->getSize());
		const BufferView<uint8> userImageView = userImageStagingBuffer->createView<uint8>(0, userImageStagingBuffer->getSize());

		cameraBufferMappedRange = cameraView.map();
		subcomponentBufferMappedRange = subcomponentView.map();
		glyphSubcomponentBufferMappedRange = glyphSubcomponentView.map();
		userImageStagingBufferMappedRange = userImageView.map();

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

		userImageStagingBuffer.~ref();

		updateDescriptorSets();

		userImagesFlushed = true;
	}

	void GUIRenderer::registerDefaultUserImage() {
		const ImageID userImageIndex = userImageCount;

		DEBUG_ASSERT(userImageIndex < MAX_USER_IMAGE_COUNT);

		#if defined(DEBUG_OBJECT_NAME)
			const sequence<char8> name = Logger::format("User image [] (default)", userImageIndex);
		#endif

		userImages[userImageIndex] = createDynamicImage({
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

			const uint64 size = imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * getByteSize(ImageFormat::R8G8B8A8_UNORM);

			convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), userImageStagingBufferMappedRange, size);

			userImageStagingBufferMappedRange += size;
		}
		else {
			std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), userImageStagingBufferMappedRange);

			userImageStagingBufferMappedRange += imageAsset->getSize();
		}

		userImages[userImageIndex] = createDynamicImage({
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

	ref<Buffer> GUIRenderer::createVertexBuffer() const {
		return createDynamicBuffer({
			.size = 4 * sizeof(Vertex),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex",
			#endif
		});
	}

	ref<Buffer> GUIRenderer::createIndexBuffer() const {
		return createDynamicBuffer({
			.size = 6 * sizeof(uint16),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Index",
			#endif
		});
	}

	ref<Buffer> GUIRenderer::createCameraBuffer() const {
		const uint64 minUniformBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		return createDynamicBuffer({
			.size = ceil(sizeof(Camera), minUniformBufferOffsetAlignment),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Camera",
			#endif
		});
	}

	ref<Buffer> GUIRenderer::createSubcomponentBuffer() const {
		return createDynamicBuffer({
			.size = MAX_SUBCOMPONENT_COUNT * sizeof(GUISubcomponent),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Subcomponent",
			#endif
		});
	}

	ref<Buffer> GUIRenderer::createGlyphSubcomponentBuffer() const {
		return createDynamicBuffer({
			.size = MAX_GLYPH_SUBCOMPONENT_COUNT * sizeof(GUIGlyphSubcomponent),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph subcomponent",
			#endif
		});
	}

	ref<Buffer> GUIRenderer::createUserImageStagingBuffer() const {
		return createDynamicBuffer({
			// Can accomodate for 8 2048x2048 RGBA textures.
			.size = 134217728,

			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "User image staging",
			#endif
		});
	}

	ref<Sampler> GUIRenderer::createNearestSampler() const {
		return createSampler({
			.magFilter = Filter::NEAREST,
			.minFilter = Filter::NEAREST,
			.addressModeU = AddressMode::REPEAT,
			.addressModeV = AddressMode::REPEAT,
			.addressModeW = AddressMode::REPEAT,
		});
	}

	ref<Sampler> GUIRenderer::createLinearSampler() const {
		return createSampler({
			.magFilter = Filter::LINEAR,
			.minFilter = Filter::LINEAR,
			.addressModeU = AddressMode::REPEAT,
			.addressModeV = AddressMode::REPEAT,
			.addressModeW = AddressMode::REPEAT,
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
		VertexInputBinding vertexInputBindings[2] = {
			VertexInputBinding(InputRate::VERTEX),
			VertexInputBinding(InputRate::INSTANCE),
		};

		vertexInputBindings[0].registerAttribute<vec4<float32>>(); // Vertex position and texture coordinate

		vertexInputBindings[1].registerAttribute<mat3x3<float32>>(); // World transform
		vertexInputBindings[1].registerAttribute<mat3x3<float32>>(); // Texture transform
		vertexInputBindings[1].registerAttribute<vec4<float32>>(); // Color
		vertexInputBindings[1].registerAttribute<uint32>(); // Z-index
		vertexInputBindings[1].registerAttribute<uint32>(); // Texture array layer index

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = subcomponentPipelineLayout,
			.vertexInputBindings = view<VertexInputBinding>(vertexInputBindings, 2),
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
			.renderPass = pointer_cast<const RenderPassLayer*>(getLayer())->renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Subcomponent",
			#endif
		});
	}

	ref<GraphicsPipeline> GUIRenderer::createGlyphSubcomponentGraphicsPipeline() const {
		VertexInputBinding vertexInputBindings[2] = {
			VertexInputBinding(InputRate::VERTEX),
			VertexInputBinding(InputRate::INSTANCE),
		};

		vertexInputBindings[0].registerAttribute<vec4<float32>>(); // Vertex position and texture coordinate

		vertexInputBindings[1].registerAttribute<mat3x3<float32>>(); // World transform
		vertexInputBindings[1].registerAttribute<mat3x3<float32>>(); // Texture transform
		vertexInputBindings[1].registerAttribute<vec4<float32>>(); // Color
		vertexInputBindings[1].registerAttribute<uint32>(); // Z-index
		vertexInputBindings[1].registerAttribute<uint32>(); // Layer index
		vertexInputBindings[1].registerAttribute<float32>(); // Font size

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = subcomponentPipelineLayout,
			.vertexInputBindings = view<VertexInputBinding>(vertexInputBindings, 2),
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
			.renderPass = pointer_cast<const RenderPassLayer*>(getLayer())->renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph subcomponent",
			#endif
		});
	}

	void GUIRenderer::copyBuffers() const {
		const ref<Buffer> stagingBuffer = createDynamicBuffer({
			.size = vertexBuffer->getSize() + indexBuffer->getSize(),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const BufferView<Vertex> vertexView = stagingBuffer->createView<Vertex>(0, vertexBuffer->getSize());
		const BufferView<uint16> indexView = stagingBuffer->createView<uint16>(vertexBuffer->getSize(), indexBuffer->getSize());

		Vertex* vertexMappedRange = vertexView.map();
		vertexMappedRange[0].position = {0.0f, 0.0f};
		vertexMappedRange[0].textureCoordinate = {0.0f, 0.0f};
		vertexMappedRange[1].position = {1.0f, 0.0f};
		vertexMappedRange[1].textureCoordinate = {1.0f, 0.0f};
		vertexMappedRange[2].position = {1.0f, 1.0f};
		vertexMappedRange[2].textureCoordinate = {1.0f, 1.0f};
		vertexMappedRange[3].position = {0.0f, 1.0f};
		vertexMappedRange[3].textureCoordinate = {0.0f, 1.0f};

		static constexpr uint16 INDICES[] = {0, 1, 2, 0, 2, 3};

		uint16* indexMappedRange = indexView.map();

		std::copy_n(INDICES, 6, indexMappedRange);

		stagingBuffer->unmap();

		const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy buffers",
			#endif
		});

		commandEncoder->begin();

		BufferCopyRegion bufferCopyRegion = {
			.sourceOffset = vertexView.getOffset(),
			.destinationOffset = 0,
			.size = vertexView.getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegion);

		bufferCopyRegion = {
			.sourceOffset = indexView.getOffset(),
			.destinationOffset = 0,
			.size = indexView.getSize(),
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, bufferCopyRegion);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();
	}

	void GUIRenderer::updateDescriptorSets() const {
		const BufferView<Camera> cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());

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

		const WriteDescriptorSet writeDescriptorSets[] = {
			{
				.descriptorSet = subcomponentDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = cameraView,
			},
			{
				.descriptorSet = subcomponentDescriptorSet,
				.bindingIndex = 1,

				// TODO(gui/user-images): Update all images in the descriptor (Vulkan and DX12).
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

		const Extent2D extent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(extent.width);
		const float32 height = static_cast<float32>(extent.height);

		commandEncoder->setScissor(0, 0, extent);
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);

		if (subcomponentCount > 0) {
			commandEncoder->bindGraphicsPipeline(subcomponentGraphicsPipeline);
			commandEncoder->bindDescriptorSets(subcomponentPipelineLayout, 0, subcomponentDescriptorSet);

			// TODO: Remove vertex buffer.
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);

			commandEncoder->bindVertexBuffer(1, subcomponentBuffer, 0);
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
			commandEncoder->drawIndexed(6, subcomponentCount, 0, 0, 0);
		}

		if (glyphSubcomponentCount > 0) {
			commandEncoder->bindGraphicsPipeline(glyphSubcomponentGraphicsPipeline);
			commandEncoder->bindDescriptorSets(subcomponentPipelineLayout, 0, subcomponentDescriptorSet);

			// TODO: Remove vertex buffer.
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);

			commandEncoder->bindVertexBuffer(1, glyphSubcomponentBuffer, 0);
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
			commandEncoder->drawIndexed(6, glyphSubcomponentCount, 0, 0, 0);
		}
	}

	void GUIRenderer::resize(WindowResizeEvent& event) {
		updateCameraProjection();
	}

	void GUIRenderer::resetCounters() {
		subcomponentCount = 0;
		glyphSubcomponentCount = 0;
	}
}