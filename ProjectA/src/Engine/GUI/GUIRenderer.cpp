#include "pch.hpp"

#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/GUIRenderer.hpp"

namespace ProjectA {
	GUIRenderer::GUIRenderer() {}

	void GUIRenderer::build() {
		setupVertexBuffer();
		setupIndexBuffer();
		setupSubcomponentBuffer();
		setupCameraBuffer();
		setupSampler();
		setupRenderPipeline();
	}

	void GUIRenderer::setupVertexBuffer() {
		// Image coordinates are reversed manually.
 		const chain<Vertex> vertices = {
			{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
			{{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
		};

		const uint64 size = vertices.size() * sizeof(Vertex);

		const ref<Buffer> vertexStagingBuffer = createStagingBuffer(size);

		Vertex* mappedRange = vertexStagingBuffer->map<Vertex>(0, size);

		std::copy(vertices.begin(), vertices.end(), mappedRange);

		vertexStagingBuffer->unmap();

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = size;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

		vertexBuffer = createBuffer(vertexBufferDescriptor);

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, 0, 0, size);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void GUIRenderer::setupIndexBuffer() {
		const ref<Device> device = getDevice();

		const chain<uint16> indices = {
			0, 1, 2,
			0, 2, 3,
		};

		const uint64 size = indices.size() * sizeof(uint16);

		const ref<Buffer> indexStagingBuffer = createStagingBuffer(size);

		uint16* mappedRange = indexStagingBuffer->map<uint16>(0, size);

		std::copy(indices.begin(), indices.end(), mappedRange);

		indexStagingBuffer->unmap();

		BufferDescriptor bufferDescriptor{};
		bufferDescriptor.size = size;
		bufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;

		indexBuffer = createBuffer(bufferDescriptor);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(indexStagingBuffer, indexBuffer, 0, 0, size);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void GUIRenderer::setupSubcomponentBuffer() {
		constexpr uint64 size = MAX_SUBCOMPONENTS * sizeof(Subcomponent);

		subcomponentStagingBuffer = createStagingBuffer(size, true);
		subcomponentMappedRange = subcomponentStagingBuffer->map<Subcomponent>(0, size);

		BufferDescriptor subcomponentBufferDescriptor{};
		subcomponentBufferDescriptor.size = size;
		subcomponentBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

		subcomponentBuffer = createBuffer(subcomponentBufferDescriptor);
	}

	void GUIRenderer::setupCameraBuffer() {
		BufferDescriptor cameraBufferDescriptor{};
		cameraBufferDescriptor.size = sizeof(mat3x3<float32>);
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.persistent = true;

		cameraBuffer = createBuffer(cameraBufferDescriptor);
		cameraMappedRange = cameraBuffer->map<mat3x3<float32>>(0, cameraBufferDescriptor.size);
	}

	void GUIRenderer::setupSampler() {
		SamplerDescriptor samplerDescriptor{};
		samplerDescriptor.magFilter = SamplerFilter::NEAREST;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		sampler = createSampler(samplerDescriptor);
	}

	void GUIRenderer::setupRenderPipeline() {
		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding{};
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraBuffer};

		const ImageViewDescriptor imageViewDescriptor;

		imageViews.resize(images.size());

		for (uint32 imageIndex = 0; imageIndex < images.size(); imageIndex += 1) {
			const ref<Image>& image = images[imageIndex];

			imageViews[imageIndex] = image->createView(imageViewDescriptor);
		}

		chain<ref<Sampler>> samplers(images.size(), sampler);

		DescriptorSetLayoutBinding imageDescriptorSetLayoutBinding{};
		imageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		imageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		imageDescriptorSetLayoutBinding.imageViews = imageViews;
		imageDescriptorSetLayoutBinding.samplers = samplers;

		DescriptorSetLayout descriptorSetLayout;
		descriptorSetLayout.bindings = {
			cameraDescriptorSetLayoutBinding,
			imageDescriptorSetLayoutBinding,
		};

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts = {descriptorSetLayout};

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("../ProjectA/src/Asset/Shader/GUI/Subcomponent.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("../ProjectA/src/Asset/Shader/GUI/Subcomponent.frag.spv", fragmentShaderModuleDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec3<float32>>(false);
		vertexInputBinding.registerAttribute<vec2<float32>>(false);

		VertexInputBinding subcomponentInputBinding(InputRate::INSTANCE);
		subcomponentInputBinding.registerAttribute<mat3x3<float32>>(false);
		subcomponentInputBinding.registerAttribute<mat3x3<float32>>(false);
		subcomponentInputBinding.registerAttribute<uint32>(false);
		subcomponentInputBinding.registerAttribute<vec4<float32>>(false);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.renderPass = Renderer::getMainRenderPass();
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, subcomponentInputBinding};
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;
		renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);
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

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();

		images.emplace_back(image);
	}

	void GUIRenderer::registerSubcomponent(const mat3x3<float32>& world, const mat3x3<float32>& imageWorld, uint32 imageIndex, const vec4<float32>& color) {
		ENGINE_ASSERT(subcomponentCount < MAX_SUBCOMPONENTS, "Subcomponent overflow.");

		subcomponentMappedRange[subcomponentCount].world = world;
		subcomponentMappedRange[subcomponentCount].imageWorld = imageWorld;
		subcomponentMappedRange[subcomponentCount].imageIndex = imageIndex;
		subcomponentMappedRange[subcomponentCount].color = color;

		subcomponentCount += 1;
	}

	void GUIRenderer::updateCamera() const {
		const float32 width = static_cast<float32>(getSwapchain()->getExtentWidth());
		const float32 height = static_cast<float32>(getSwapchain()->getExtentHeight());

		const mat3x3<float32> cameraProjection = {
			2.0f / width, 0.0f, 0.0f,
			0.0f, -2.0f / height, 0.0f,
			-1.0f, 1.0f, 1.0f,
		};

		*cameraMappedRange = cameraProjection;
	}

	void GUIRenderer::copyMappedBuffer() const {
		if (subcomponentCount == 0) {
			return;
		}

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> copyBufferCommandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		copyBufferCommandEncoder->begin();
		copyBufferCommandEncoder->copyBufferToBuffer(subcomponentStagingBuffer, subcomponentBuffer, 0, 0, subcomponentCount * sizeof(Subcomponent));

		const ref<CommandBuffer> commandBuffer = copyBufferCommandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void GUIRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		if (subcomponentCount == 0) {
			return;
		}

		updateCamera();

		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		commandEncoder->bindRenderPipeline(renderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindIndexBuffer<uint16>(indexBuffer, 0);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
		commandEncoder->bindVertexBuffer(1, subcomponentBuffer, 0, sizeof(Subcomponent));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->drawIndexed(6, subcomponentCount, 0);
	}

	void GUIRenderer::resetSubcomponentCount() {
		subcomponentCount = 0;
	}
}