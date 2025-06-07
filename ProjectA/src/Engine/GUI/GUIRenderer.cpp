#include "pch.hpp"

#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/GUIRenderer.hpp"

namespace ProjectA {
	void GUIRenderer::build() {
		const ref<Surface> surface = getSurface();

		ColorAttachment colorAttachment{};
		colorAttachment.format = surface->getFormat();
		colorAttachment.sampleCount = 4;
		colorAttachment.loadOperation = LoadOperation::CLEAR;
		colorAttachment.storeOperation = StoreOperation::STORE;

		RenderPassDescriptor renderPassDescriptor;
		renderPassDescriptor.colorAttachments = {colorAttachment};
		renderPassDescriptor.resolve = true;
		renderPassDescriptor.depthStencil = false;
		renderPassDescriptor.presentAttachmentIndex = 0;

		renderPass = createRenderPass(renderPassDescriptor);

		updateColorAttachment();

		const ref<Swapchain>& swapchain = getSwapchain();

		swapchain->getImages(renderPass);

		initializeVertexBuffer();
		initializeIndexBuffer();
		initializeSubcomponentBuffer();
		initializeCameraBuffer();

		SamplerDescriptor samplerDescriptor{};
		samplerDescriptor.magFilter = SamplerFilter::NEAREST;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		sampler = createSampler(samplerDescriptor);

		SamplerDescriptor bilinearSamplerDescriptor{};
		bilinearSamplerDescriptor.magFilter = SamplerFilter::LINEAR;
		bilinearSamplerDescriptor.minFilter = SamplerFilter::LINEAR;

		bilinearSampler = createSampler(bilinearSamplerDescriptor);

		initializeRenderPipeline();

		updateCamera();
	}

	void GUIRenderer::updateColorAttachment() {
		const ref<Swapchain>& swapchain = getSwapchain();

		AttachmentImageDescriptor colorAttachmentImageDescriptor{};
		colorAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		colorAttachmentImageDescriptor.width = swapchain->getExtentWidth();
		colorAttachmentImageDescriptor.height = swapchain->getExtentHeight();
		colorAttachmentImageDescriptor.depth = 1;
		colorAttachmentImageDescriptor.mipLevelCount = 1;
		colorAttachmentImageDescriptor.arrayLayerCount = 1;

		colorImage = renderPass->createColorAttachmentImage(colorAttachmentImageDescriptor, 0);

		constexpr ImageViewDescriptor colorImageViewDescriptor;

		colorImageView = colorImage->createView(colorImageViewDescriptor);

		renderPass->setColorAttachmentImageView(colorImageView, 0);
	}

	void GUIRenderer::initializeVertexBuffer() {
		constexpr uint64 size = 6 * sizeof(Vertex);

		const ref<Buffer> vertexStagingBuffer = createStagingBuffer(size);

		Vertex* vertexMappedRange = vertexStagingBuffer->map<Vertex>(0, size);

		// Image coordinates are reversed manually.
		vertexMappedRange[0].vertex = {0.0f, 0.0f, 1.0f, 0.0f};
		vertexMappedRange[0].imageCoordinate = {0.0f, 0.0f};
		vertexMappedRange[1].vertex = {1.0f, 0.0f, 1.0f, 0.0f};
		vertexMappedRange[1].imageCoordinate = {1.0f, 0.0f};
		vertexMappedRange[2].vertex = {1.0f, 1.0f, 1.0f, 0.0f};
		vertexMappedRange[2].imageCoordinate = {1.0f, 1.0f};
		vertexMappedRange[3].vertex = {0.0f, 1.0f, 1.0f, 0.0f};
		vertexMappedRange[3].imageCoordinate = {0.0f, 1.0f};

		vertexStagingBuffer->unmap();

		BufferDescriptor vertexBufferDescriptor;
		vertexBufferDescriptor.size = size;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		vertexBuffer = createBuffer(vertexBufferDescriptor);

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, 0, 0, size);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
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

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(indexStagingBuffer, indexBuffer, 0, 0, size);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void GUIRenderer::initializeSubcomponentBuffer() {
		constexpr uint64 subcomponentBufferSize = MAX_SUBCOMPONENTS * sizeof(Subcomponent);

		BufferDescriptor subcomponentBufferDescriptor;
		subcomponentBufferDescriptor.size = subcomponentBufferSize;
		subcomponentBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		subcomponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		subcomponentBufferDescriptor.persistent = true;

		subcomponentBuffer = createBuffer(subcomponentBufferDescriptor);
		subcomponentMappedRange = subcomponentBuffer->map<Subcomponent>(0, subcomponentBufferSize);

		constexpr uint64 glyphSubcomponentBufferSize = MAX_GLYPH_SUBCOMPONENTS * sizeof(GlyphSubcomponent);

		BufferDescriptor glyphSubcomponentBufferDescriptor;
		glyphSubcomponentBufferDescriptor.size = glyphSubcomponentBufferSize;
		glyphSubcomponentBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
		glyphSubcomponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphSubcomponentBufferDescriptor.persistent = true;

		glyphSubcomponentBuffer = createBuffer(glyphSubcomponentBufferDescriptor);
		glyphSubcomponentMappedRange = glyphSubcomponentBuffer->map<GlyphSubcomponent>(0, glyphSubcomponentBufferSize);
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
		constexpr ImageViewDescriptor imageViewDescriptor;

		imageViews.resize(images.size());

		for (uint32 imageIndex = 0; imageIndex < images.size(); imageIndex += 1) {
			const ref<Image>& image = images[imageIndex];

			imageViews[imageIndex] = image->createView(imageViewDescriptor);
		}

		std::vector<ref<Sampler>> samplers(images.size(), sampler);

		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding{};
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

		DescriptorSetLayoutBinding imageDescriptorSetLayoutBinding{};
		imageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		imageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		imageDescriptorSetLayoutBinding.imageViews = imageViews;
		imageDescriptorSetLayoutBinding.samplers = samplers;

		DescriptorSetLayoutBinding glyphMapDescriptorSetLayoutBinding{};
		glyphMapDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		glyphMapDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		glyphMapDescriptorSetLayoutBinding.imageViews = {imageViews[1]};
		glyphMapDescriptorSetLayoutBinding.samplers = {bilinearSampler};

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, imageDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor subcomponentPipelineLayoutDescriptor;
		subcomponentPipelineLayoutDescriptor.layouts = {descriptorSetLayout};

		subcomponentPipelineLayout = createPipelineLayout(subcomponentPipelineLayoutDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);

		// TODO: Remove padding.
		vertexInputBinding.registerAttribute<vec4<float32>>();

		vertexInputBinding.registerAttribute<svec2<float32>>();

		VertexInputBinding subcomponentInputBinding(InputRate::INSTANCE);
		subcomponentInputBinding.registerAttribute<mat3x3<float32>>();
		subcomponentInputBinding.registerAttribute<mat3x3<float32>>();
		subcomponentInputBinding.registerAttribute<uint32>();
		subcomponentInputBinding.registerAttribute<vec4<float32>>();

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		ColorBlendAttachment opaqueColorBlendAttachment{};
		opaqueColorBlendAttachment.blend = false;

		RenderPipelineDescriptor subcomponentRenderPipelineDescriptor;
		subcomponentRenderPipelineDescriptor.renderPass = renderPass;
		subcomponentRenderPipelineDescriptor.pipelineLayout = subcomponentPipelineLayout;
		subcomponentRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, subcomponentInputBinding};
		subcomponentRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("../ProjectA/src/Asset/Shader/GUI/Subcomponent.vert.spv", vertexShaderModuleDescriptor);
		subcomponentRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("../ProjectA/src/Asset/Shader/GUI/Subcomponent.frag.spv", fragmentShaderModuleDescriptor);
		subcomponentRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		subcomponentRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		subcomponentRenderPipelineDescriptor.cullMode = CullMode::BACK;
		subcomponentRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		subcomponentRenderPipelineDescriptor.lineWidth = 1.0f;
		subcomponentRenderPipelineDescriptor.minSampleShading = 0.5f;
		subcomponentRenderPipelineDescriptor.colorBlendState.attachments = {opaqueColorBlendAttachment};
		subcomponentRenderPipelineDescriptor.depthStencilState.depthTest = false;
		subcomponentRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		subcomponentRenderPipeline = createRenderPipeline(subcomponentRenderPipelineDescriptor);

		///

		DescriptorSetLayout glyphSubcomponentDescriptorSetLayout;
		glyphSubcomponentDescriptorSetLayout.bindings = {cameraDescriptorSetLayoutBinding, glyphMapDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor glyphSubcomponentPipelineLayoutDescriptor;
		glyphSubcomponentPipelineLayoutDescriptor.layouts = {glyphSubcomponentDescriptorSetLayout};

		glyphSubcomponentPipelineLayout = createPipelineLayout(glyphSubcomponentPipelineLayoutDescriptor);

		VertexInputBinding glyphSubcomponentVertexInputBinding(InputRate::INSTANCE);
		glyphSubcomponentVertexInputBinding.registerAttribute<mat3x3<float32>>();
		glyphSubcomponentVertexInputBinding.registerAttribute<mat3x3<float32>>();
		glyphSubcomponentVertexInputBinding.registerAttribute<uint32>();
		glyphSubcomponentVertexInputBinding.registerAttribute<float32>();
		glyphSubcomponentVertexInputBinding.registerAttribute<vec4<float32>>();
		glyphSubcomponentVertexInputBinding.registerAttribute<vec4<float32>>();

		ColorBlendAttachment transparentColorBlendAttachment{};
		transparentColorBlendAttachment.blend = true;
		transparentColorBlendAttachment.srcColorBlendFactor = BlendFactor::ONE;
		transparentColorBlendAttachment.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

		RenderPipelineDescriptor glyphSubcomponentRenderPipelineDescriptor;
		glyphSubcomponentRenderPipelineDescriptor.renderPass = renderPass;
		glyphSubcomponentRenderPipelineDescriptor.pipelineLayout = glyphSubcomponentPipelineLayout;
		glyphSubcomponentRenderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, glyphSubcomponentVertexInputBinding};
		glyphSubcomponentRenderPipelineDescriptor.vertexShaderModule = loadShaderModule("../ProjectA/src/Asset/Shader/GUI/GlyphSubcomponent.vert.spv", vertexShaderModuleDescriptor);
		glyphSubcomponentRenderPipelineDescriptor.fragmentShaderModule = loadShaderModule("../ProjectA/src/Asset/Shader/GUI/GlyphSubcomponent.frag.spv", fragmentShaderModuleDescriptor);
		glyphSubcomponentRenderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		glyphSubcomponentRenderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		glyphSubcomponentRenderPipelineDescriptor.cullMode = CullMode::BACK;
		glyphSubcomponentRenderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		glyphSubcomponentRenderPipelineDescriptor.lineWidth = 1.0f;
		glyphSubcomponentRenderPipelineDescriptor.minSampleShading = 1.0f;
		glyphSubcomponentRenderPipelineDescriptor.colorBlendState.attachments = {transparentColorBlendAttachment};
		glyphSubcomponentRenderPipelineDescriptor.depthStencilState.depthTest = false;
		glyphSubcomponentRenderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		glyphSubcomponentRenderPipeline = createRenderPipeline(glyphSubcomponentRenderPipelineDescriptor);
	}

	uint32 GUIRenderer::registerDefaultImage() {
		const uint32 imageIndex = static_cast<uint32>(images.size());

		ENGINE_ASSERT(imageIndex < MAX_IMAGES, "Image registration overflow.");

		constexpr uint32 width = 1;
		constexpr uint32 height = 1;
		constexpr uint64 channelCount = 4;
		constexpr uint64 size = channelCount * sizeof(uint8);

		const ref<Buffer> stagingBuffer = createStagingBuffer(size);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, size);

		std::fill_n(mappedRange, channelCount, 255);

		stagingBuffer->unmap();

		registerImage(stagingBuffer, width, height);

		return imageIndex;
	}

	uint32 GUIRenderer::registerImage(const ref<Image>& image) {
		const uint32 imageIndex = static_cast<uint32>(images.size());

		ENGINE_ASSERT(imageIndex < MAX_IMAGES, "Image registration overflow.");

		images.emplace_back(image);

		return imageIndex;
	}

	uint32 GUIRenderer::registerImage(std::string_view path) {
		const uint32 imageIndex = static_cast<uint32>(images.size());

		ENGINE_ASSERT(imageIndex < MAX_IMAGES, "Image registration overflow.");

		const PNGFile file = Filesystem::load<PNGFile>(path);
		const uint64 size = file.data.size() * sizeof(uint8);

		const ref<Buffer> stagingBuffer = createStagingBuffer(size);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, size);

		std::copy(file.data.begin(), file.data.end(), mappedRange);

		stagingBuffer->unmap();

		registerImage(stagingBuffer, file.width, file.height);

		return imageIndex;
	}

	void GUIRenderer::registerImage(const ref<Buffer>& buffer, uint32 width, uint32 height) {
		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = ImageFormat::R8G8B8A8_SRGB;
		imageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
		imageDescriptor.width = width;
		imageDescriptor.height = height;
		imageDescriptor.depth = 1;
		imageDescriptor.mipLevelCount = 1;
		imageDescriptor.arrayLayerCount = 1;
		imageDescriptor.sampleCount = 1;

		const ref<Image> image = createImage(imageDescriptor);

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(image, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
		commandEncoder->copyBufferToImage(buffer, image, width, height, 0);
		commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit({commandBuffer}, nullptr, nullptr, nullptr);
		device->waitIdle();

		images.emplace_back(image);
	}

	void GUIRenderer::registerSubcomponent(const mat3x3<float32>& world, const mat3x3<float32>& imageWorld, uint32 imageIndex, const vec4<float32>& color) {
		DEBUG_ASSERT(subcomponentCount < MAX_SUBCOMPONENTS);

		subcomponentMappedRange[subcomponentCount].world = world;
		subcomponentMappedRange[subcomponentCount].imageWorld = imageWorld;
		subcomponentMappedRange[subcomponentCount].imageIndex = imageIndex;
		subcomponentMappedRange[subcomponentCount].color = color;

		subcomponentCount += 1;
	}

	void GUIRenderer::registerGlyphSubcomponent(const mat3x3<float32>& world, const mat3x3<float32>& imageWorld, uint32 imageIndex, const TextStyle& textStyle) {
		DEBUG_ASSERT(glyphSubcomponentCount < MAX_GLYPH_SUBCOMPONENTS);

		glyphSubcomponentMappedRange[glyphSubcomponentCount].world = world;
		glyphSubcomponentMappedRange[glyphSubcomponentCount].textureWorld = imageWorld;
		glyphSubcomponentMappedRange[glyphSubcomponentCount].fontIndex = imageIndex;
		glyphSubcomponentMappedRange[glyphSubcomponentCount].outlineWidth = textStyle.outlineWidth;
		glyphSubcomponentMappedRange[glyphSubcomponentCount].color = textStyle.color;
		glyphSubcomponentMappedRange[glyphSubcomponentCount].outlineColor = textStyle.outlineColor;

		glyphSubcomponentCount += 1;
	}

	void GUIRenderer::updateCamera() const {
		const ref<Swapchain>& swapchain = getSwapchain();
		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		const mat3x3<float32> cameraProjection = {
			2.0f / width, 0.0f, 0.0f,
			0.0f, -2.0f / height, 0.0f,
			-1.0f, 1.0f, 1.0f,
		};

		*cameraMappedRange = cameraProjection;
	}

	void GUIRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		if (subcomponentCount == 0 && glyphSubcomponentCount == 0) {
			return;
		}

		const ref<Swapchain>& swapchain = getSwapchain();
		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = renderPass;
		beginRenderPassDescriptor.swapchain = swapchain;
		beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};
		beginRenderPassDescriptor.imageIndex = getImageIndex();

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);

		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);

		commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));

		if (subcomponentCount > 0) {
			commandEncoder->bindRenderPipeline(subcomponentRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->bindVertexBuffer(1, subcomponentBuffer, 0, sizeof(Subcomponent));
			commandEncoder->drawIndexed(6, subcomponentCount, 0);
		}

		if (glyphSubcomponentCount > 0) {
			commandEncoder->bindRenderPipeline(glyphSubcomponentRenderPipeline);
			commandEncoder->bindAllDescriptorSets();
			commandEncoder->bindVertexBuffer(1, glyphSubcomponentBuffer, 0, sizeof(GlyphSubcomponent));
			commandEncoder->drawIndexed(6, glyphSubcomponentCount, 0);
		}

		commandEncoder->endRenderPass();
	}

	void GUIRenderer::resize(WindowResizeEvent& event) {
		updateCamera();
		updateColorAttachment();

		const ref<Swapchain>& swapchain = getSwapchain();

		swapchain->getImages(renderPass);
	}

	void GUIRenderer::resetCounters() {
		subcomponentCount = 0;
		glyphSubcomponentCount = 0;
	}
}