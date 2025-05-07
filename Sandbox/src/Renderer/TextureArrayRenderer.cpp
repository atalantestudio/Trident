#include "pch.hpp"

#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

#include "TextureArrayRenderer.hpp"

namespace ProjectA {
	constexpr char* BABOON_IMAGE_PATH = "Asset/Image/Baboon.png";
	constexpr char* CAMERAMAN_IMAGE_PATH = "Asset/Image/Cameraman.png";
	constexpr char* LENNA_IMAGE_PATH = "Asset/Image/Lenna.png";
	constexpr char* PEPPER_IMAGE_PATH = "Asset/Image/Pepper.png";

	TextureArrayRenderer::TextureArrayRenderer() {
		baboonImage = loadImage(BABOON_IMAGE_PATH);
		cameramanImage = loadImage(CAMERAMAN_IMAGE_PATH);
		lennaImage = loadImage(LENNA_IMAGE_PATH);
		pepperImage = loadImage(PEPPER_IMAGE_PATH);

		const ImageViewDescriptor imageViewDescriptor;

		baboonImageView = baboonImage->createView(imageViewDescriptor);
		cameramanImageView = cameramanImage->createView(imageViewDescriptor);
		lennaImageView = lennaImage->createView(imageViewDescriptor);
		pepperImageView = pepperImage->createView(imageViewDescriptor);

		setupSampler();
		setupRenderPipeline();
		setupVertexBuffer();
		setupInstanceVertexBuffer();
	}

	ref<Image> TextureArrayRenderer::loadImage(const std::string& imagePath) {
		const PNGFile file = Filesystem::load<PNGFile>(imagePath);
		const uint64 imageBufferSize = sizeof(uint8) * file.data.size();

		const ref<Buffer> imageStagingBuffer = createStagingBuffer(imageBufferSize);

		uint8* imageData = imageStagingBuffer->map<uint8>(0, imageBufferSize);

		std::copy(file.data.begin(), file.data.end(), imageData);

		imageStagingBuffer->unmap();

		const ref<Device> device = getDevice();

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = ImageFormat::R8G8B8A8_SRGB;
		imageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
		imageDescriptor.width = file.width;
		imageDescriptor.height = file.height;
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
		commandEncoder->copyBufferToImage(imageStagingBuffer, image, file.width, file.height, 0);
		commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();

		return image;
	}

	void TextureArrayRenderer::setupSampler() {
		SamplerDescriptor samplerDescriptor{};
		samplerDescriptor.magFilter = SamplerFilter::LINEAR;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		sampler = createSampler(samplerDescriptor);
	}

	void TextureArrayRenderer::setupRenderPipeline() {
		DescriptorSetLayoutBinding textureArrayDescriptorSetLayoutBinding;
		textureArrayDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		textureArrayDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		textureArrayDescriptorSetLayoutBinding.imageViews = {baboonImageView, cameramanImageView, lennaImageView, pepperImageView};

		// TODO: Refactor.
		textureArrayDescriptorSetLayoutBinding.samplers = {sampler, sampler, sampler, sampler};

		DescriptorSetLayout textureArrayDescriptorSetLayout;
		textureArrayDescriptorSetLayout.bindings = {textureArrayDescriptorSetLayoutBinding};

		PipelineLayoutDescriptor renderPipelineLayoutDescriptor;
		renderPipelineLayoutDescriptor.layouts = {textureArrayDescriptorSetLayout};

		renderPipelineLayout = createPipelineLayout(renderPipelineLayoutDescriptor);

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<vec2<float32>>(false);

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<vec2<float32>>(false);
		instanceInputBinding.registerAttribute<uint32>(false);

		ShaderModuleDescriptor vertexShaderModuleDescriptor{};
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("Asset/Shader/TextureArray.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("Asset/Shader/TextureArray.frag.spv", fragmentShaderModuleDescriptor);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.renderPass = getMainRenderPass();
		renderPipelineDescriptor.pipelineLayout = renderPipelineLayout;
		renderPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.polygonMode = PolygonMode::FILL;
		renderPipelineDescriptor.cullMode = CullMode::BACK;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);
	}

	void TextureArrayRenderer::setupVertexBuffer() {
		constexpr uint64 vertexBufferSize = sizeof(Vertex) * 6;

		const ref<Buffer> vertexStagingBuffer = createStagingBuffer(vertexBufferSize);

		Vertex* vertices = vertexStagingBuffer->map<Vertex>(0, vertexBufferSize);

		vertices[0].position = {-0.5f,  0.5f};
		vertices[1].position = { 0.5f,  0.5f};
		vertices[2].position = { 0.5f, -0.5f};
		vertices[3].position = {-0.5f,  0.5f};
		vertices[4].position = { 0.5f, -0.5f};
		vertices[5].position = {-0.5f, -0.5f};

		vertexStagingBuffer->unmap();

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = vertexBufferSize;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

		vertexBuffer = createBuffer(vertexBufferDescriptor);

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, 0, 0, vertexBufferSize);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TextureArrayRenderer::setupInstanceVertexBuffer() {
		constexpr uint64 instanceVertexBufferSize = sizeof(Instance) * 4;

		const ref<Buffer> instanceVertexStagingBuffer = createStagingBuffer(instanceVertexBufferSize);

		Instance* instances = instanceVertexStagingBuffer->map<Instance>(0, instanceVertexBufferSize);

		instances[0].offset = {-0.5f,  0.5f};
		instances[0].textureIndex = 0;
		instances[1].offset = { 0.5f,  0.5f};
		instances[1].textureIndex = 1;
		instances[2].offset = {-0.5f, -0.5f};
		instances[2].textureIndex = 2;
		instances[3].offset = { 0.5f, -0.5f};
		instances[3].textureIndex = 3;

		instanceVertexStagingBuffer->unmap();

		BufferDescriptor instanceVertexBufferDescriptor{};
		instanceVertexBufferDescriptor.size = instanceVertexBufferSize;
		instanceVertexBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;

		instanceVertexBuffer = createBuffer(instanceVertexBufferDescriptor);

		const ref<Device> device = getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(instanceVertexStagingBuffer, instanceVertexBuffer, 0, 0, instanceVertexBufferSize);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	void TextureArrayRenderer::render(const ref<CommandEncoder>& commandEncoder) const {
		const ref<Swapchain>& swapchain = getSwapchain();

		const float32 width = static_cast<float32>(swapchain->getExtentWidth());
		const float32 height = static_cast<float32>(swapchain->getExtentHeight());

		commandEncoder->bindRenderPipeline(renderPipeline);
		commandEncoder->bindAllDescriptorSets();
		commandEncoder->bindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
		commandEncoder->bindVertexBuffer(1, instanceVertexBuffer, 0, sizeof(Instance));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchain->getExtentWidth(), swapchain->getExtentHeight());
		commandEncoder->draw(24, 4, 0);
	}
}