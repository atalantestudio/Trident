// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/GUI/GUIRenderer.hpp"
#include "Engine/API/Window/Layer/RenderPassLayer.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	void GUIRenderer::build() {
		subcomponentDescriptorSetLayout = createSubcomponentDescriptorSetLayout();

		// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
		sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(subcomponentDescriptorSetLayout);

		subcomponentDescriptorSet = descriptorSets[0];

		subcomponentPipelineLayout = createSubcomponentPipelineLayout();

		subcomponentGraphicsPipeline = createSubcomponentGraphicsPipeline();
		glyphSubcomponentGraphicsPipeline = createGlyphSubcomponentGraphicsPipeline();
		vertexBuffer = createVertexBuffer();
		indexBuffer = createIndexBuffer();
		cameraBuffer = createCameraBuffer();
		subcomponentBuffer = createSubcomponentBuffer();
		glyphSubcomponentBuffer = createGlyphSubcomponentBuffer();
		userImageStagingBuffer = createUserImageStagingBuffer();
		nearestSampler = createNearestSampler();
		linearSampler = createLinearSampler();

		cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());

		const BufferView<GUISubcomponent> subcomponentView = subcomponentBuffer->createView<GUISubcomponent>(0, subcomponentBuffer->getSize());
		const BufferView<GUIGlyphSubcomponent> glyphSubcomponentView = glyphSubcomponentBuffer->createView<GUIGlyphSubcomponent>(0, glyphSubcomponentBuffer->getSize());
		const BufferView<uint8> userImageView = userImageStagingBuffer->createView<uint8>(0, userImageStagingBuffer->getSize());

		cameraMappedRange = cameraView.map();
		subcomponentMappedRange = subcomponentView.map();
		glyphSubcomponentMappedRange = glyphSubcomponentView.map();
		userImageStagingMappedRange = userImageView.map();

		updateCameraProjection();
	}

	void GUIRenderer::encodeOneTimeCommands() const {
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
		vertexMappedRange[0] = {{0.0f, 0.0f, 0.0f, 0.0f}};
		vertexMappedRange[1] = {{1.0f, 0.0f, 1.0f, 0.0f}};
		vertexMappedRange[2] = {{1.0f, 1.0f, 1.0f, 1.0f}};
		vertexMappedRange[3] = {{0.0f, 1.0f, 0.0f, 1.0f}};

		uint16* indexMappedRange = indexView.map();
		indexMappedRange[0] = 0;
		indexMappedRange[1] = 1;
		indexMappedRange[2] = 2;
		indexMappedRange[3] = 0;
		indexMappedRange[4] = 2;
		indexMappedRange[5] = 3;

		stagingBuffer->unmap();

		const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "One-time commands",
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

	void GUIRenderer::render(ref<CommandEncoder> commandEncoder) const {
		if (subcomponentCount + glyphSubcomponentCount == 0) {
			return;
		}

		const Extent2D extent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(extent.x);
		const float32 height = static_cast<float32>(extent.y);

		commandEncoder->setScissor(0, 0, extent);
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);

		if (subcomponentCount > 0) {
			commandEncoder->bindGraphicsPipeline(subcomponentGraphicsPipeline);
			commandEncoder->bindDescriptorSets(subcomponentPipelineLayout, 0, subcomponentDescriptorSet);

			// TODO(gui/renderer): Remove vertex buffer.
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);

			commandEncoder->bindVertexBuffer(1, subcomponentBuffer, 0);
			commandEncoder->bindIndexBuffer<IndexFormat::R16_UINT>(indexBuffer, 0);
			commandEncoder->drawIndexed(6, subcomponentCount, 0, 0, 0);
		}

		if (glyphSubcomponentCount > 0) {
			commandEncoder->bindGraphicsPipeline(glyphSubcomponentGraphicsPipeline);
			commandEncoder->bindDescriptorSets(subcomponentPipelineLayout, 0, subcomponentDescriptorSet);

			// TODO(gui/renderer): Remove vertex buffer.
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);

			commandEncoder->bindVertexBuffer(1, glyphSubcomponentBuffer, 0);
			commandEncoder->bindIndexBuffer<IndexFormat::R16_UINT>(indexBuffer, 0);
			commandEncoder->drawIndexed(6, glyphSubcomponentCount, 0, 0, 0);
		}
	}

	void GUIRenderer::resize(WindowResizeEvent& event) {
		updateCameraProjection();
	}

	bool GUIRenderer::doneFlushing() const {
		return userImagesFlushed;
	}

	ref<Image> GUIRenderer::getUserImage(uint32 imageIndex) const {
		DEBUG_ASSERT(imageIndex < userImageCount);

		return userImages[imageIndex];
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

			const uint64 size = imageAsset->getExtent().x * imageAsset->getExtent().y * imageAsset->getExtent().z * imageAsset->getLayerCount() * getByteSize(ImageFormat::R8G8B8A8_UNORM);

			convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), userImageStagingMappedRange, size);

			userImageStagingMappedRange += size;
		}
		else {
			std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), userImageStagingMappedRange);

			userImageStagingMappedRange += imageAsset->getSize();
		}

		userImages[userImageIndex] = createDynamicImage({
			.type = ImageType::TYPE_2D,
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

	void GUIRenderer::registerDefaultUserImage() {
		const ImageID userImageIndex = userImageCount;

		DEBUG_ASSERT(userImageIndex < MAX_USER_IMAGE_COUNT);

		#if defined(DEBUG_OBJECT_NAME)
			const sequence<char8> name = Logger::format("User image [] (default)", userImageIndex);
		#endif

		userImages[userImageIndex] = createDynamicImage({
			.type = ImageType::TYPE_2D,
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

		static constexpr uint8 data[] = {255, 255, 255, 255};
		static constexpr view<uint8> dataView = data;

		std::copy(dataView.begin(), dataView.end(), userImageStagingMappedRange);

		userImageStagingMappedRange += dataView.count();
		userImageCount += 1;
	}

	void GUIRenderer::flushUserImageStagingBuffer() {
		DEBUG_ASSERT(!userImagesFlushed);

		userImageStagingBuffer->unmap();

		ImageMemoryBarrier userImageMemoryBarriers[MAX_USER_IMAGE_COUNT];
		view<ImageMemoryBarrier> imageMemoryBarriers(userImageMemoryBarriers, userImageCount);
		BufferImageCopyRegion userImageCopyRegion = {
			.bufferOffset = 0,
			.imageOffset = {0, 0, 0},
			.imageMipLevel = 0,
			.imageLayerOffset = 0,
			.imageLayerCount = 1,
		};
		uint32 userImageIndex = 0;

		while (userImageIndex < userImageCount) {
			userImageMemoryBarriers[userImageIndex] = {
				.image = userImages[userImageIndex],
				.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
				.newAccess = Access::TRANSFER_WRITE,
			};

			userImageIndex += 1;
		}

		const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy user images",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

		userImageIndex = 0;

		while (userImageIndex < userImageCount) {
			const ref<Image> userImage = userImages[userImageIndex];

			userImageCopyRegion.imageExtent = userImage->getExtent();

			userImageMemoryBarriers[userImageIndex] = {
				.image = userImage,
				.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.newAccess = Access::SHADER_READ,
			};

			commandEncoder->copyBufferToImage(userImageStagingBuffer, userImages[userImageIndex], userImageCopyRegion);

			userImageCopyRegion.bufferOffset += userImage->getSize();

			userImageIndex += 1;
		}

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		updateDescriptorSets();

		userImagesFlushed = true;

		// TODO(gui/renderer): Destroy the user image staging buffer here.
	}

	void GUIRenderer::registerSubcomponent(const GUISubcomponent& subcomponent) {
		DEBUG_ASSERT(subcomponentCount < MAX_SUBCOMPONENT_COUNT);

		subcomponentMappedRange[subcomponentCount] = subcomponent;
		subcomponentCount += 1;
	}

	void GUIRenderer::registerGlyphSubcomponent(const GUIGlyphSubcomponent& glyphSubcomponent) {
		DEBUG_ASSERT(glyphSubcomponentCount < MAX_GLYPH_SUBCOMPONENT_COUNT);

		glyphSubcomponentMappedRange[glyphSubcomponentCount] = glyphSubcomponent;
		glyphSubcomponentCount += 1;
	}

	void GUIRenderer::resetCounters() {
		subcomponentCount = 0;
		glyphSubcomponentCount = 0;
	}

	ref<DescriptorSetLayout> GUIRenderer::createSubcomponentDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::VERTEX,
			},

			// User images
			{
				.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
				.descriptorCount = MAX_USER_IMAGE_COUNT,
				.stageFlags = ShaderStage::FRAGMENT,
			},
		};

		return createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Subcomponent",
			#endif
		});
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

		vertexInputBindings[0].registerAttribute<decltype(Vertex::positionAndTextureCoordinate)>();
		vertexInputBindings[1].registerAttribute<decltype(GUISubcomponent::worldTransform)>();
		vertexInputBindings[1].registerAttribute<decltype(GUISubcomponent::textureTransform)>();
		vertexInputBindings[1].registerAttribute<decltype(GUISubcomponent::color)>();
		vertexInputBindings[1].registerAttribute<decltype(GUISubcomponent::zIndex)>();
		vertexInputBindings[1].registerAttribute<decltype(GUISubcomponent::textureArrayLayerIndex)>();

		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = subcomponentPipelineLayout,
			.vertexInputBindings = vertexInputBindings,
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

		vertexInputBindings[0].registerAttribute<decltype(Vertex::positionAndTextureCoordinate)>();
		vertexInputBindings[1].registerAttribute<decltype(GUIGlyphSubcomponent::world)>();
		vertexInputBindings[1].registerAttribute<decltype(GUIGlyphSubcomponent::textureWorld)>();
		vertexInputBindings[1].registerAttribute<decltype(GUIGlyphSubcomponent::color)>();
		vertexInputBindings[1].registerAttribute<decltype(GUIGlyphSubcomponent::zIndex)>();
		vertexInputBindings[1].registerAttribute<decltype(GUIGlyphSubcomponent::textureArrayLayerIndex)>();
		vertexInputBindings[1].registerAttribute<decltype(GUIGlyphSubcomponent::fontSize)>();

		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = subcomponentPipelineLayout,
			.vertexInputBindings = vertexInputBindings,
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

	ref<Buffer> GUIRenderer::createCameraBuffer() const {
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		return createDynamicBuffer({
			.size = ceilPowerOf2(sizeof(Camera), alignment),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Camera",
			#endif
		});
	}

	ref<Buffer> GUIRenderer::createVertexBuffer() const {
		return createDynamicBuffer({
			.size = sizeof(Vertex) * 4,
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
			.size = sizeof(uint16) * 6,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Index",
			#endif
		});
	}

	ref<Buffer> GUIRenderer::createSubcomponentBuffer() const {
		return createDynamicBuffer({
			.size = sizeof(GUISubcomponent) * MAX_SUBCOMPONENT_COUNT,
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
			.size = sizeof(GUIGlyphSubcomponent) * MAX_GLYPH_SUBCOMPONENT_COUNT,
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
			.size = MAX_USER_IMAGE_COUNT * 2048 * 2048 * getByteSize(ImageFormat::R8G8B8A8_UNORM),
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
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Nearest",
			#endif
		});
	}

	ref<Sampler> GUIRenderer::createLinearSampler() const {
		return createSampler({
			.magFilter = Filter::LINEAR,
			.minFilter = Filter::LINEAR,
			.addressModeU = AddressMode::REPEAT,
			.addressModeV = AddressMode::REPEAT,
			.addressModeW = AddressMode::REPEAT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Linear",
			#endif
		});
	}

	void GUIRenderer::updateDescriptorSets() const {
		DescriptorImage userDescriptorImages[MAX_USER_IMAGE_COUNT];
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

				// TODO(graphics/device/update-descriptor-sets): All images in the descriptor must be provided (requirement from Vulkan and DX12).
				.descriptorImages = userDescriptorImages,
			},
		};

		getDevice()->updateDescriptorSets(writeDescriptorSets);
	}

	void GUIRenderer::updateCameraProjection() const {
		const Extent2D extent = getSwapchain()->getExtent();

		// TODO(lyah/projection/orthographic-3x3)
		const mat3x3<float32> projection = orthographicTopLeftOrigin(vec2<float32>(extent));

		cameraMappedRange->projection = {{
			{projection[0].x, projection[0].y, projection[0].z, 1.0f},
			{projection[1].x, projection[1].y, projection[1].z, 1.0f},
			{projection[2].x, projection[2].y, projection[2].z, 1.0f},
		}};
	}
}