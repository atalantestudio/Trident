#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/GUIRenderer.hpp"

namespace ProjectA {
	GUIRenderer::GUIRenderer() :
		cameraUniformBuffers(MAX_FRAMES_IN_FLIGHT),
		cameraMappedRanges(MAX_FRAMES_IN_FLIGHT)
	{
		const ref<Device> device = getDevice();

		BufferDescriptor cameraUniformBufferDescriptor{};
		cameraUniformBufferDescriptor.size = sizeof(mat3x3<float32>);
		cameraUniformBufferDescriptor.usage = BufferUsage::UNIFORM;

		for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
			cameraUniformBuffers[i] = createBuffer(cameraUniformBufferDescriptor);
			cameraMappedRanges[i] = cameraUniformBuffers[i]->map<mat3x3<float32>>(0, cameraUniformBufferDescriptor.size);
		}

		//BufferDescriptor colorStorageBufferDescriptor{};
		//colorStorageBufferDescriptor.size = sizeof(vec4<float32>) * MAX_COLORS;
		//colorStorageBufferDescriptor.usage = BufferUsage::STORAGE;

		//colorStorageBuffer = createBuffer(colorStorageBufferDescriptor);

		const ImageViewDescriptor imageViewDescriptor;

		defaultImage = loadDefaultImage();
		defaultImageView = defaultImage->createView(imageViewDescriptor);

		fontImage = loadImage("../ProjectA/src/Asset/Texture/Quiver.png");
		fontImageView = fontImage->createView(imageViewDescriptor);

		SamplerDescriptor samplerDescriptor{};
		samplerDescriptor.magFilter = SamplerFilter::NEAREST;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		sampler = createSampler(samplerDescriptor);

		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding{};
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = cameraUniformBuffers;

		DescriptorSetLayoutBinding imageDescriptorSetLayoutBinding{};
		imageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		imageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		imageDescriptorSetLayoutBinding.imageViews = {fontImageView};
		imageDescriptorSetLayoutBinding.samplers = {sampler};

		//DescriptorSetLayoutBinding colorDescriptorSetLayoutBinding{};
		//colorDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		//colorDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		//colorDescriptorSetLayoutBinding.buffers = {colorStorageBuffer};

		DescriptorSetLayout cameraAndColorDescriptorSetLayout;
		cameraAndColorDescriptorSetLayout.bindings = {
			cameraDescriptorSetLayoutBinding,
			imageDescriptorSetLayoutBinding,
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
		vertexRateBinding.registerAttribute<mat3x3<float32>>(false);
		vertexRateBinding.registerAttribute<uint32>(false);
		vertexRateBinding.registerAttribute<float32>(false);

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

		// Create position vertex buffer.
		{
			// Image coordinates are reversed manually here.
 			const chain<Vertex> vertices = {
				{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
				{{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
				{{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
				{{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			};

			BufferDescriptor bufferDescriptor{};
			bufferDescriptor.size = sizeof(Vertex) * vertices.size();
			bufferDescriptor.usage = BufferUsage::VERTEX;

			positionVertexBuffer = createBuffer(bufferDescriptor);

			Vertex* mappedRange = positionVertexBuffer->map<Vertex>(0, bufferDescriptor.size);

			std::copy(vertices.begin(), vertices.end(), mappedRange);

			positionVertexBuffer->unmap();
		}

		BufferDescriptor imageCoordinateVertexBufferDescriptor{};
		imageCoordinateVertexBufferDescriptor.size = sizeof(vec2<float32>) * MAX_QUADS * 4;
		imageCoordinateVertexBufferDescriptor.usage = BufferUsage::VERTEX;

		textureCoordinateVertexBuffer = createBuffer(imageCoordinateVertexBufferDescriptor);

		BufferDescriptor transformVertexBufferDescriptor{};
		transformVertexBufferDescriptor.size = sizeof(GUIComponent) * MAX_QUADS;
		transformVertexBufferDescriptor.usage = BufferUsage::VERTEX;

		transformVertexBuffer = createBuffer(transformVertexBufferDescriptor);

		// Create index buffer.
		{
			const chain<uint16> indices = {
				0, 1, 2,
				0, 2, 3,
			};

			BufferDescriptor bufferDescriptor{};
			bufferDescriptor.size = sizeof(uint16) * indices.size();
			bufferDescriptor.usage = BufferUsage::INDEX;

			indexBuffer = createBuffer(bufferDescriptor);

			uint16* mappedRange = indexBuffer->map<uint16>(0, bufferDescriptor.size);

			std::copy(indices.begin(), indices.end(), mappedRange);

			indexBuffer->unmap();
		}

		// Create command encoder.
		{
			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

			commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);
		}
	}

	ref<Image> GUIRenderer::loadImage(const std::string& path) {
		const PNGFile file = Filesystem::load<PNGFile>(path);
		const uint64 size = sizeof(uint8) * file.data.size();

		const ref<Buffer> stagingBuffer = createStagingBuffer(size);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, size);

		std::copy(file.data.begin(), file.data.end(), mappedRange);

		stagingBuffer->unmap();

		return loadImage(stagingBuffer, file.width, file.height);
	}

	ref<Image> GUIRenderer::loadImage(const ref<Buffer>& buffer, uint32 width, uint32 height) {
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
	}

	ref<Image> GUIRenderer::loadDefaultImage() {
		const std::array<uint8, 4> data = {1, 1, 1, 1};
		const uint64 size = sizeof(uint8) * data.size();

		const ref<Buffer> stagingBuffer = createStagingBuffer(size);

		uint8* mappedRange = stagingBuffer->map<uint8>(0, size);

		std::copy(data.begin(), data.end(), mappedRange);

		stagingBuffer->unmap();

		return loadImage(stagingBuffer, 1, 1);
	}

	void GUIRenderer::registerEntityPool(const ComponentPool<GUIComponent>& componentPool) {
		instanceCount = componentPool.componentCount;

		/*GUIComponent* mappedRange = textureCoordinateVertexBuffer->map<GUIComponent>(0, sizeof(vec2<float32>) * instanceCount * 4);

		std::copy(componentPool.components.begin(), componentPool.components.begin() + instanceCount, mappedRange);

		textureCoordinateVertexBuffer->unmap();*/

		GUIComponent* mappedRange = transformVertexBuffer->map<GUIComponent>(0, sizeof(GUIComponent) * instanceCount);

		std::copy(componentPool.components.begin(), componentPool.components.begin() + instanceCount, mappedRange);

		transformVertexBuffer->unmap();
	}

	void GUIRenderer::registerColors(const chain<vec4<float32>>& colors) const {
		/*vec4<float32>* mappedRange = colorStorageBuffer->map<vec4<float32>>(0, sizeof(vec4<float32>) * colors.size());

		std::copy(colors.begin(), colors.end(), mappedRange);

		colorStorageBuffer->unmap();*/
	}

	void GUIRenderer::render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		updateCamera();

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
		commandEncoder->bindVertexBuffer(0, positionVertexBuffer, sizeof(vec3<float32>));
		commandEncoder->bindVertexBuffer(1, transformVertexBuffer, sizeof(GUIComponent));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, Application::get().getSwapchain()->getExtentWidth(), Application::get().getSwapchain()->getExtentHeight());
		commandEncoder->drawIndexed(6, instanceCount, 0);
		commandEncoder->endRenderPass();

		frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
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

	void GUIRenderer::onEvent(Event& event) {}
}