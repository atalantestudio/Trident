#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/GUIRenderer.hpp"

namespace ProjectA {
	void GUIRenderer::build() {
		const ref<Surface> surface = getSurface();

		ColorAttachment colorAttachment{};
		colorAttachment.format = surface->getFormat();
		colorAttachment.sampleCount = 1;

		// TODO: Use of CLEAR is debug-only.
		colorAttachment.loadOperation = LoadOperation::CLEAR;
		// colorAttachment.loadOperation = LoadOperation::LOAD;

		colorAttachment.storeOperation = StoreOperation::STORE;
		colorAttachment.finalImageLayout = ImageLayout::PRESENT_SRC;

		SubpassDescriptor subpassDescriptor{};
		subpassDescriptor.colorAttachments = {colorAttachment};
		subpassDescriptor.depthStencil = false;

		subpass = createSubpass(subpassDescriptor);

		SubpassDependency subpassDependency{};
		subpassDependency.sourceSubpassIndex = SubpassDependency::EXTERNAL_SUBPASS;
		subpassDependency.destinationSubpassIndex = 0;

		RenderPassDescriptor renderPassDescriptor{};
		renderPassDescriptor.subpasses = {subpass};
		renderPassDescriptor.subpassDependencies = {subpassDependency};

		renderPass = createRenderPass(renderPassDescriptor);

		recreateFramebuffers();

		initializeVertexBuffer();
		initializeIndexBuffer();
		initializeSubcomponentBuffer();
		initializeCameraBuffer();

		ImageDescriptor textureArrayDescriptor;
		textureArrayDescriptor.type = ImageType::TEXTURE_2D;
		textureArrayDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		textureArrayDescriptor.extent.width = TEXTURE_ARRAY_MAX_WIDTH;
		textureArrayDescriptor.extent.height = TEXTURE_ARRAY_MAX_HEIGHT;
		textureArrayDescriptor.extent.depth = 1;
		textureArrayDescriptor.mipLevelCount = 1;
		textureArrayDescriptor.layerCount = TEXTURE_ARRAY_MAX_LAYER_COUNT;
		textureArrayDescriptor.sampleCount = 1;
		textureArrayDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
		textureArrayDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		textureArray = createImage(textureArrayDescriptor);

		ImageViewDescriptor textureArrayViewDescriptor{};
		textureArrayViewDescriptor.mipLevelOffset = 0;
		textureArrayViewDescriptor.mipLevelCount = textureArray->getMipLevelCount();
		textureArrayViewDescriptor.layerOffset = 0;
		textureArrayViewDescriptor.layerCount = textureArray->getLayerCount();

		textureArrayView = textureArray->createView(textureArrayViewDescriptor);

		static constexpr uint64 textureArrayStagingBufferSize = TEXTURE_ARRAY_MAX_WIDTH * TEXTURE_ARRAY_MAX_HEIGHT * TEXTURE_ARRAY_MAX_LAYER_COUNT * TEXTURE_ARRAY_LAYER_BPP;

		textureArrayStagingBuffer = createStagingBuffer(textureArrayStagingBufferSize);
		textureArrayStagingBufferMappedRange = textureArrayStagingBuffer->map<uint8>(0, textureArrayStagingBufferSize);

		SamplerDescriptor nearestSamplerDescriptor{};
		nearestSamplerDescriptor.magFilter = SamplerFilter::NEAREST;
		nearestSamplerDescriptor.minFilter = SamplerFilter::NEAREST;

		nearestSampler = createSampler(nearestSamplerDescriptor);

		SamplerDescriptor linearSamplerDescriptor{};
		linearSamplerDescriptor.magFilter = SamplerFilter::NEAREST;
		linearSamplerDescriptor.minFilter = SamplerFilter::LINEAR;

		linearSampler = createSampler(linearSamplerDescriptor);

		initializeRenderPipeline();

		updateCamera();
	}

	void GUIRenderer::initializeVertexBuffer() {
		static constexpr uint64 size = 6 * sizeof(Vertex);

		const ref<Device> device = getDevice();

		const ref<Buffer> vertexStagingBuffer = createStagingBuffer(size);

		Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, size);

		vertexMappedRange[0].vertex = {0.0f, 0.0f};
		vertexMappedRange[0].textureCoordinate = {0.0f, 0.0f};
		vertexMappedRange[1].vertex = {1.0f, 0.0f};
		vertexMappedRange[1].textureCoordinate = {1.0f, 0.0f};
		vertexMappedRange[2].vertex = {1.0f, 1.0f};
		vertexMappedRange[2].textureCoordinate = {1.0f, 1.0f};
		vertexMappedRange[3].vertex = {0.0f, 1.0f};
		vertexMappedRange[3].textureCoordinate = {0.0f, 1.0f};

		vertexStagingBuffer->unmap();

		BufferDescriptor vertexBufferDescriptor;
		vertexBufferDescriptor.size = size;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexBuffer = createBuffer(vertexBufferDescriptor);

		sequence<BufferCopyRegion> vertexBufferCopyRegions(1);

		BufferCopyRegion& vertexBufferCopyRegion = vertexBufferCopyRegions[0];
		vertexBufferCopyRegion.sourceOffset = 0;
		vertexBufferCopyRegion.destinationOffset = 0;
		vertexBufferCopyRegion.size = size;

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, vertexBufferCopyRegions);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		device->waitIdle();
	}

	void GUIRenderer::initializeIndexBuffer() {
		const ref<Device> device = getDevice();

		const std::vector<uint16> indices = {
			0, 1, 2,
			0, 2, 3,
		};

		const uint64 size = indices.size() * sizeof(uint16);

		const ref<Buffer> indexStagingBuffer = createStagingBuffer(size);

		uint16* mappedRange = indexStagingBuffer->map<uint16>(0, size);

		std::copy(indices.begin(), indices.end(), mappedRange);

		indexStagingBuffer->unmap();

		BufferDescriptor indexBufferDescriptor;
		indexBufferDescriptor.size = size;
		indexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
		indexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		indexBuffer = createBuffer(indexBufferDescriptor);

		sequence<BufferCopyRegion> indexBufferCopyRegions(1);

		BufferCopyRegion& indexBufferCopyRegion = indexBufferCopyRegions[0];
		indexBufferCopyRegion.sourceOffset = 0;
		indexBufferCopyRegion.destinationOffset = 0;
		indexBufferCopyRegion.size = size;

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> transferQueue = device->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(indexStagingBuffer, indexBuffer, indexBufferCopyRegions);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		device->waitIdle();
	}

	void GUIRenderer::initializeSubcomponentBuffer() {
		constexpr uint64 subcomponentBufferSize = MAX_SUBCOMPONENTS * sizeof(GUISubcomponent);

		BufferDescriptor subcomponentBufferDescriptor;
		subcomponentBufferDescriptor.size = subcomponentBufferSize;
		subcomponentBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		subcomponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		subcomponentBufferDescriptor.persistent = true;

		subcomponentBuffer = createBuffer(subcomponentBufferDescriptor);
		subcomponentMappedRange = subcomponentBuffer->map<GUISubcomponent>(0, subcomponentBufferSize);

		constexpr uint64 glyphSubcomponentBufferSize = MAX_GLYPH_SUBCOMPONENTS * sizeof(GUIGlyphSubcomponent);

		BufferDescriptor glyphSubcomponentBufferDescriptor;
		glyphSubcomponentBufferDescriptor.size = glyphSubcomponentBufferSize;
		glyphSubcomponentBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		glyphSubcomponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphSubcomponentBufferDescriptor.persistent = true;

		glyphSubcomponentBuffer = createBuffer(glyphSubcomponentBufferDescriptor);
		glyphSubcomponentMappedRange = glyphSubcomponentBuffer->map<GUIGlyphSubcomponent>(0, glyphSubcomponentBufferSize);
	}

	void GUIRenderer::initializeCameraBuffer() {
		BufferDescriptor cameraBufferDescriptor;
		cameraBufferDescriptor.size = sizeof(mat3x3<float32>);
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		cameraBufferDescriptor.persistent = true;

		cameraBuffer = createBuffer(cameraBufferDescriptor);
		cameraMappedRange = cameraBuffer->map<mat3x3<float32>>(0, cameraBufferDescriptor.size);
	}

	void GUIRenderer::initializeRenderPipeline() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding{};
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

		DescriptorSetLayoutBinding nearestTextureArrayDescriptorSetLayoutBinding{};
		nearestTextureArrayDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		nearestTextureArrayDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		nearestTextureArrayDescriptorSetLayoutBinding.imageViews = {textureArrayView};
		nearestTextureArrayDescriptorSetLayoutBinding.samplers = {nearestSampler};

		DescriptorSetLayoutBinding linearTextureArrayDescriptorSetLayoutBinding{};
		linearTextureArrayDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		linearTextureArrayDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		linearTextureArrayDescriptorSetLayoutBinding.imageViews = {textureArrayView};
		linearTextureArrayDescriptorSetLayoutBinding.samplers = {linearSampler};

		DescriptorSetLayout nearestDescriptorSetLayout;
		nearestDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, nearestTextureArrayDescriptorSetLayoutBinding};

		DescriptorSetLayout linearDescriptorSetLayout;
		linearDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, linearTextureArrayDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor nearestPipelineLayoutDescriptor;
		nearestPipelineLayoutDescriptor.layouts = {nearestDescriptorSetLayout};

		PipelineLayoutDescriptor linearPipelineLayoutDescriptor;
		linearPipelineLayoutDescriptor.layouts = {linearDescriptorSetLayout};

		subcomponentPipelineLayout = createPipelineLayout(nearestPipelineLayoutDescriptor);

		glyphSubcomponentPipelineLayout = createPipelineLayout(linearPipelineLayoutDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec4<float32>>(); // Vertex position and texture coordinate

		VertexInputBinding subcomponentInputBinding(InputRate::INSTANCE);
		subcomponentInputBinding.registerAttribute<vec4<float32>>(); // Bounds
		subcomponentInputBinding.registerAttribute<mat3x3<float32>>(); // World transform
		subcomponentInputBinding.registerAttribute<mat3x3<float32>>(); // Texture transform
		subcomponentInputBinding.registerAttribute<vec4<float32>>(); // Color
		subcomponentInputBinding.registerAttribute<vec4<float32>>(); // Outline color
		subcomponentInputBinding.registerAttribute<uint32>(); // Texture array layer index
		subcomponentInputBinding.registerAttribute<float32>(); // Outline size
		subcomponentInputBinding.registerAttribute<float32>(); // Border radius

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		ColorBlendAttachment transparentColorBlendAttachment{};
		transparentColorBlendAttachment.blend = true;
		transparentColorBlendAttachment.srcColorBlendFactor = BlendFactor::SRC_ALPHA;
		transparentColorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		RenderPipelineDescriptor subcomponentRenderPipelineDescriptor;
		subcomponentRenderPipelineDescriptor.pipelineLayout = subcomponentPipelineLayout;
		subcomponentRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, subcomponentInputBinding};
		subcomponentRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("../../../ProjectA/src/Asset/Shader/GUI/Subcomponent.vert", vertexShaderModuleDescriptor);
		subcomponentRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("../../../ProjectA/src/Asset/Shader/GUI/Subcomponent.frag", fragmentShaderModuleDescriptor);
		subcomponentRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		subcomponentRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		subcomponentRenderPipelineDescriptor.cullMode = CullMode::BACK;
		subcomponentRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		subcomponentRenderPipelineDescriptor.lineWidth = 1.0f;
		subcomponentRenderPipelineDescriptor.minSampleShading = 0.5f;
		subcomponentRenderPipelineDescriptor.colorBlendState.attachments = {transparentColorBlendAttachment};
		subcomponentRenderPipelineDescriptor.depthStencilState.depthTest = false;
		subcomponentRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		subcomponentRenderPipelineDescriptor.renderPass = renderPass;
		subcomponentRenderPipelineDescriptor.subpassIndex = 0;

		subcomponentRenderPipeline = createRenderPipeline(subcomponentRenderPipelineDescriptor);

		VertexInputBinding glyphSubcomponentVertexInputBinding(InputRate::INSTANCE);
		glyphSubcomponentVertexInputBinding.registerAttribute<mat3x3<float32>>();
		glyphSubcomponentVertexInputBinding.registerAttribute<mat3x3<float32>>();
		glyphSubcomponentVertexInputBinding.registerAttribute<uint32>();
		glyphSubcomponentVertexInputBinding.registerAttribute<vec4<float32>>();

		RenderPipelineDescriptor glyphSubcomponentRenderPipelineDescriptor;
		glyphSubcomponentRenderPipelineDescriptor.pipelineLayout = glyphSubcomponentPipelineLayout;
		glyphSubcomponentRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, glyphSubcomponentVertexInputBinding};
		glyphSubcomponentRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("../../../ProjectA/src/Asset/Shader/GUI/GlyphSubcomponent.vert", vertexShaderModuleDescriptor);
		glyphSubcomponentRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("../../../ProjectA/src/Asset/Shader/GUI/GlyphSubcomponent.frag", fragmentShaderModuleDescriptor);
		glyphSubcomponentRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		glyphSubcomponentRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		glyphSubcomponentRenderPipelineDescriptor.cullMode = CullMode::BACK;
		glyphSubcomponentRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		glyphSubcomponentRenderPipelineDescriptor.lineWidth = 1.0f;
		glyphSubcomponentRenderPipelineDescriptor.minSampleShading = 1.0f;
		glyphSubcomponentRenderPipelineDescriptor.colorBlendState.attachments = {transparentColorBlendAttachment};
		glyphSubcomponentRenderPipelineDescriptor.depthStencilState.depthTest = false;
		glyphSubcomponentRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		glyphSubcomponentRenderPipelineDescriptor.renderPass = renderPass;
		glyphSubcomponentRenderPipelineDescriptor.subpassIndex = 0;

		glyphSubcomponentRenderPipeline = createRenderPipeline(glyphSubcomponentRenderPipelineDescriptor);
	}

	uint32 GUIRenderer::registerTextureArrayLayer(const TextureArrayLayer& layer) {
		ENGINE_ASSERT(textureArrayLayerCount + 1 <= TEXTURE_ARRAY_MAX_LAYER_COUNT, "Too many client-side images.");
		ENGINE_ASSERT(layer.width <= TEXTURE_ARRAY_MAX_WIDTH, "Image width ([]) is larger than the maximum width ([]).", layer.width, TEXTURE_ARRAY_MAX_WIDTH);
		ENGINE_ASSERT(layer.height <= TEXTURE_ARRAY_MAX_HEIGHT, "Image height ([]) is larger than the maximum height ([]).", layer.height, TEXTURE_ARRAY_MAX_HEIGHT);

		const uint32 layerIndex = textureArrayLayerCount;

		std::copy(layer.data.begin(), layer.data.end(), textureArrayStagingBufferMappedRange);

		textureArrayStagingBufferMappedRange += layer.data.count;

		textureArrayLayerExtents[textureArrayLayerCount].width = layer.width;
		textureArrayLayerExtents[textureArrayLayerCount].height = layer.height;

		textureArrayLayerCount += 1;

		return layerIndex;
	}

	void GUIRenderer::flushTextureArrayStagingBuffer() const {
		DEBUG_ASSERT(textureArrayLayerCount > 0);

		textureArrayStagingBuffer->unmap();

		sequence<BufferImageCopyRegion> copyRegions(textureArrayLayerCount);
		uint64 bufferOffset = 0;

		for (uint32 layerIndex = 0; layerIndex < textureArrayLayerCount; layerIndex += 1) {
			const Extent2D extent = textureArrayLayerExtents[layerIndex];

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

			bufferOffset += extent.width * extent.height * TEXTURE_ARRAY_LAYER_BPP;
		}

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor;
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();

		commandEncoder->setImageMemoryBarrier(textureArray, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
		commandEncoder->copyBufferToImage(textureArrayStagingBuffer, textureArray, copyRegions);
		commandEncoder->setImageMemoryBarrier(textureArray, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		graphicsQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		device->waitIdle();

		textureArrayStagingBuffer.~shared_ptr();
	}

	Extent2D GUIRenderer::getTextureArrayLayerExtent(uint32 layerIndex) const {
		ENGINE_ASSERT(layerIndex < textureArrayLayerCount, "Layer index overflow.");

		return textureArrayLayerExtents[layerIndex];
	}

	void GUIRenderer::registerSubcomponent(const GUISubcomponent& subcomponent) {
		DEBUG_ASSERT(subcomponentCount < MAX_SUBCOMPONENTS);

		subcomponentMappedRange[subcomponentCount] = subcomponent;
		subcomponentCount += 1;
	}

	void GUIRenderer::registerGlyphSubcomponent(const GUIGlyphSubcomponent& glyphSubcomponent) {
		DEBUG_ASSERT(glyphSubcomponentCount < MAX_GLYPH_SUBCOMPONENTS);

		glyphSubcomponentMappedRange[glyphSubcomponentCount] = glyphSubcomponent;
		glyphSubcomponentCount += 1;
	}

	void GUIRenderer::updateCamera() const {
		const ref<Swapchain>& swapchain = getSwapchain();
		const float32 width = static_cast<float32>(swapchain->getExtent().width);
		const float32 height = static_cast<float32>(swapchain->getExtent().height);

		const mat3x3<float32> cameraProjection = {
			2.0f / width, 0.0f, 0.0f,
			0.0f, -2.0f / height, 0.0f,
			-1.0f, 1.0f, 1.0f,
		};

		*cameraMappedRange = cameraProjection;
	}

	void GUIRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		if (subcomponentCount == 0 && glyphSubcomponentCount == 0) {
			ENGINE_WARN("No GUI components to render.");

			// TODO: If the command encoder is left empty, the attachments will not be transitioned into the final layout.
			BREAKPOINT();

			return;
		}

		const ref<Swapchain>& swapchain = getSwapchain();
		const float32 width = static_cast<float32>(swapchain->getExtent().width);
		const float32 height = static_cast<float32>(swapchain->getExtent().height);

		const ClearValue clearValue = {1.0f, 1.0f, 1.0f, 1.0f};

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
		beginRenderPassDescriptor.clearValues = {clearValue};

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->setScissor(0, 0, swapchain->getExtent());
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);

		if (subcomponentCount > 0) {
			commandEncoder->bindRenderPipeline(subcomponentRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
			commandEncoder->bindVertexBuffer(1, subcomponentBuffer, 0, sizeof(GUISubcomponent));
			commandEncoder->drawIndexed(6, subcomponentCount, 0, 0, 0);
		}

		if (glyphSubcomponentCount > 0) {
			commandEncoder->bindRenderPipeline(glyphSubcomponentRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
			commandEncoder->bindVertexBuffer(1, glyphSubcomponentBuffer, 0, sizeof(GUIGlyphSubcomponent));
			commandEncoder->drawIndexed(6, glyphSubcomponentCount, 0, 0, 0);
		}

		commandEncoder->endRenderPass();
	}

	void GUIRenderer::resize(WindowResizeEvent& event) {
		recreateFramebuffers();
		updateCamera();
	}

	void GUIRenderer::resetCounters() {
		subcomponentCount = 0;
		glyphSubcomponentCount = 0;
	}

	void GUIRenderer::recreateFramebuffers() {
		const ref<Swapchain>& swapchain = getSwapchain();

		if (framebuffers.count > 0) {
			for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
				framebuffers[framebufferIndex].~shared_ptr();
			}
		}

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor{};
		framebufferDescriptor.renderPass = renderPass;
		framebufferDescriptor.imageViews = sequence<ref<ImageView>>(1);
		framebufferDescriptor.extent = swapchain->getExtent();
		framebufferDescriptor.layerCount = 1;

		for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
			framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
		}
	}
}