#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/GUIRenderer.hpp"

namespace ProjectA {
	GUIRenderer::GUIRenderer() :
		cameraBuffers(MAX_FRAMES_IN_FLIGHT),
		cameraMappedRanges(MAX_FRAMES_IN_FLIGHT)
	{
		setupVertexBuffer();
		setupIndexBuffer();
		setupWorldBuffer();
		//setupColorBuffer();
		//setupTextureCoordinateBuffer();
		setupCameraBuffers();
		setupRenderPipeline();
	}

	void GUIRenderer::setupVertexBuffer() {
		const ref<Device> device = getDevice();

		// Image coordinates are reversed manually.
 		const chain<Vertex> vertices = {
			{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		};

		const uint64 size = vertices.size() * sizeof(Vertex);

		const ref<Buffer> vertexStagingBuffer = createStagingBuffer(size);

		Vertex* mappedRange = vertexStagingBuffer->map<Vertex>(0, size);

		std::copy(vertices.begin(), vertices.end(), mappedRange);

		vertexStagingBuffer->unmap();

		BufferDescriptor bufferDescriptor{};
		bufferDescriptor.size = size;
		bufferDescriptor.usage = BufferUsage::VERTEX;

		vertexBuffer = createBuffer(bufferDescriptor);

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
		bufferDescriptor.usage = BufferUsage::INDEX;

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

	void GUIRenderer::setupWorldBuffer() {
		constexpr uint64 size = MAX_INSTANCES * sizeof(mat3x3<float32>);

		BufferDescriptor worldBufferDescriptor{};
		worldBufferDescriptor.size = size;
		worldBufferDescriptor.usage = BufferUsage::VERTEX;

		worldStagingBuffer = createStagingBuffer(size, true);
		worldBuffer = createBuffer(worldBufferDescriptor);
		worldMappedRange = worldStagingBuffer->map<mat3x3<float32>>(0, size);
	}

	/*void GUIRenderer::setupColorBuffer() {
		BufferDescriptor colorBufferDescriptor{};
		colorBufferDescriptor.size = MAX_COLORS * sizeof(vec4<float32>);
		colorBufferDescriptor.usage = BufferUsage::STORAGE;

		colorStorageBuffer = createBuffer(colorBufferDescriptor);
	}*/

	/*void GUIRenderer::setupTextureCoordinateBuffer() {
		BufferDescriptor textureCoordinateBufferDescriptor{};
		textureCoordinateBufferDescriptor.size = MAX_QUADS * 4 * sizeof(vec2<float32>);
		textureCoordinateBufferDescriptor.usage = BufferUsage::VERTEX;

		textureCoordinateVertexBuffer = createBuffer(textureCoordinateBufferDescriptor);
	}*/

	void GUIRenderer::setupCameraBuffers() {
		BufferDescriptor cameraBufferDescriptor{};
		cameraBufferDescriptor.size = sizeof(mat3x3<float32>);
		cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
		cameraBufferDescriptor.persistent = true;

		for (uint32 cameraBufferIndex = 0; cameraBufferIndex < MAX_FRAMES_IN_FLIGHT; cameraBufferIndex += 1) {
			cameraBuffers[cameraBufferIndex] = createBuffer(cameraBufferDescriptor);
			cameraMappedRanges[cameraBufferIndex] = cameraBuffers[cameraBufferIndex]->map<mat3x3<float32>>(0, cameraBufferDescriptor.size);
		}
	}

	void GUIRenderer::setupRenderPipeline() {
		//const ImageViewDescriptor imageViewDescriptor;

		//defaultImage = loadDefaultImage();
		//defaultImageView = defaultImage->createView(imageViewDescriptor);

		//fontImage = loadImage("../ProjectA/src/Asset/Texture/Quiver.png");
		//fontImageView = fontImage->createView(imageViewDescriptor);

		SamplerDescriptor samplerDescriptor{};
		samplerDescriptor.magFilter = SamplerFilter::NEAREST;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		sampler = createSampler(samplerDescriptor);

		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding{};
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = cameraBuffers;

		/*DescriptorSetLayoutBinding imageDescriptorSetLayoutBinding{};
		imageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		imageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		imageDescriptorSetLayoutBinding.imageViews = {fontImageView};
		imageDescriptorSetLayoutBinding.samplers = {sampler};*/

		/*DescriptorSetLayoutBinding colorDescriptorSetLayoutBinding{};
		colorDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		colorDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		colorDescriptorSetLayoutBinding.buffers = {colorStorageBuffer};*/

		DescriptorSetLayout cameraAndColorDescriptorSetLayout;
		cameraAndColorDescriptorSetLayout.bindings = {
			cameraDescriptorSetLayoutBinding,
			//imageDescriptorSetLayoutBinding,
			//colorDescriptorSetLayoutBinding,
		};

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts.emplace_back(cameraAndColorDescriptorSetLayout);

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("../ProjectA/src/Engine/Shader/GUI/Subcomponent.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("../ProjectA/src/Engine/Shader/GUI/Subcomponent.frag.spv", fragmentShaderModuleDescriptor);

		VertexInputBinding vertexRateBinding(InputRate::VERTEX);
		vertexRateBinding.registerAttribute<vec3<float32>>(false);
		vertexRateBinding.registerAttribute<vec2<float32>>(false);

		VertexInputBinding instanceRateBinding(InputRate::INSTANCE);
		instanceRateBinding.registerAttribute<mat3x3<float32>>(false);
		//instanceRateBinding.registerAttribute<uint32>(false);
		//instanceRateBinding.registerAttribute<float32>(false);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.renderPass = Application::get().getRenderPass();
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexInputBindings = {vertexRateBinding, instanceRateBinding};
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);
	}

	/*ref<Image> GUIRenderer::loadImage(const std::string& path) {
		const PNGFile file = Filesystem::load<PNGFile>(path);
		const uint64 size = sizeof(uint8) * file.data.size();

		const ref<Buffer> stagingBuffer = createStagingBuffer(size);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, size);

		std::copy(file.data.begin(), file.data.end(), mappedRange);

		stagingBuffer->unmap();

		return loadImage(stagingBuffer, file.width, file.height);
	}*/

	/*ref<Image> GUIRenderer::loadImage(const ref<Buffer>& buffer, uint32 width, uint32 height) {
		const ref<Device> device = getDevice();

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = ImageFormat::R8G8B8A8_SRGB;
		imageDescriptor.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED;
		imageDescriptor.width = width;
		imageDescriptor.height = height;
		imageDescriptor.depth = 1;
		imageDescriptor.mipLevelCount = 1;
		imageDescriptor.arrayLayerCount = 1;
		imageDescriptor.sampleCount = 1;

		const ref<Image> image = createImage(imageDescriptor);

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

		return image;
	}*/

	/*ref<Image> GUIRenderer::loadDefaultImage() {
		const std::array<uint8, 4> data = {1, 1, 1, 1};
		const uint64 size = sizeof(uint8) * data.size();

		const ref<Buffer> stagingBuffer = createStagingBuffer(size);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, size);

		std::copy(data.begin(), data.end(), mappedRange);

		stagingBuffer->unmap();

		return loadImage(stagingBuffer, 1, 1);
	}*/

	/*void GUIRenderer::registerEntityPool(const ComponentPool<GUIComponent>& componentPool) {
		instanceCount = componentPool.componentCount;

		GUIComponent* mappedRange = textureCoordinateVertexBuffer->map<GUIComponent>(0, sizeof(vec2<float32>) * instanceCount * 4);

		std::copy(componentPool.components.begin(), componentPool.components.begin() + instanceCount, mappedRange);

		textureCoordinateVertexBuffer->unmap();

		GUIComponent* mappedRange = transformVertexBuffer->map<GUIComponent>(0, sizeof(GUIComponent) * instanceCount);

		std::copy(componentPool.components.begin(), componentPool.components.begin() + instanceCount, mappedRange);

		transformVertexBuffer->unmap();
	}*/

	void GUIRenderer::addQuad(const mat3x3<float32>& world) {
		ENGINE_ASSERT(instanceCount < MAX_INSTANCES, "Instance overflow.");

		worldMappedRange[instanceCount] = world;

		instanceCount += 1;
	}

	void GUIRenderer::updateCamera() const {
		const float32 width = static_cast<float32>(Application::get().getSwapchain()->getExtentWidth());
		const float32 height = static_cast<float32>(Application::get().getSwapchain()->getExtentHeight());

		const mat3x3<float32> cameraProjection = {
			2.0f / width, 0.0f, 0.0f,
			0.0f, -2.0f / height, 0.0f,
			-1.0f, 1.0f, 1.0f,
		};

		*cameraMappedRanges[frameIndex] = cameraProjection;
	}

	void GUIRenderer::copyWorldBuffer() const {
		const ref<Device> device = getDevice();

		const uint64 size = instanceCount * sizeof(mat3x3<float32>);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(worldStagingBuffer, worldBuffer, 0, 0, size);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void GUIRenderer::clearWorldBuffer() {
		instanceCount = 0;
	}

	/*void GUIRenderer::registerColors(const chain<vec4<float32>>& colors) const {
		vec4<float32>* mappedRange = colorStorageBuffer->map<vec4<float32>>(0, sizeof(vec4<float32>) * colors.size());

		std::copy(colors.begin(), colors.end(), mappedRange);

		colorStorageBuffer->unmap();
	}*/

	void GUIRenderer::render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		updateCamera();

		copyWorldBuffer();

		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = Application::get().getRenderPass();
		beginRenderPassDescriptor.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		beginRenderPassDescriptor.nextImageIndex = nextImageIndex;

		const float32 width = static_cast<float32>(Application::get().getSwapchain()->getExtentWidth());
		const float32 height = static_cast<float32>(Application::get().getSwapchain()->getExtentHeight());

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->bindRenderPipeline(renderPipeline);
		commandEncoder->bindDescriptorSets(0);
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, sizeof(Vertex));
		commandEncoder->bindVertexBuffer(1, worldBuffer, sizeof(mat3x3<float32>));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, Application::get().getSwapchain()->getExtentWidth(), Application::get().getSwapchain()->getExtentHeight());
		commandEncoder->drawIndexed(6, instanceCount, 0);
		commandEncoder->endRenderPass();

		frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

		clearWorldBuffer();
	}

	void GUIRenderer::onEvent(Event& event) {}
}