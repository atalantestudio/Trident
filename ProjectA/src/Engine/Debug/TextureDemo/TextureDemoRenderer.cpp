#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Debug/TextureDemo/TextureDemoRenderer.hpp"
#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	TextureDemoRenderer::TextureDemoRenderer() {
		const ref<Device>& device = getDevice();

		// Load image.
		{
			const PNGFile file = Filesystem::load<PNGFile>("../ProjectA/src/Engine/Debug/TextureDemo/Asset/Zelda.png");
			const uint64 size = sizeof(uint8) * file.data.size();

			BufferDescriptor stagingBufferDescriptor{};
			stagingBufferDescriptor.size = size;
			stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

			const ref<Buffer> stagingBuffer = createBuffer(stagingBufferDescriptor);

			uint8* mappedRange = stagingBuffer->map<uint8>(0, size);

			std::copy(file.data.begin(), file.data.end(), mappedRange);

			stagingBuffer->unmap();

			ImageDescriptor imageDescriptor{};
			imageDescriptor.type = ImageType::TEXTURE_2D;
			imageDescriptor.format = ImageFormat::R8G8B8A8_SRGB;
			imageDescriptor.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED;
			imageDescriptor.width = file.width;
			imageDescriptor.height = file.height;
			imageDescriptor.depth = 1;
			imageDescriptor.mipLevelCount = 1;
			imageDescriptor.arrayLayerCount = 1;
			imageDescriptor.sampleCount = 1;

			image = createImage(imageDescriptor);

			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->setImageMemoryBarrier(image, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
			commandEncoder->copyBufferToImage(stagingBuffer, image, file.width, file.height, 0);
			commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			const ref<Device>& device = getDevice();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		// Create image view.
		{
			const ImageViewDescriptor imageViewDescriptor;

			imageView = image->createView(imageViewDescriptor);
		}

		// Create sampler.
		{
			SamplerDescriptor samplerDescriptor{};
			samplerDescriptor.magFilter = SamplerFilter::NEAREST;
			samplerDescriptor.minFilter = SamplerFilter::NEAREST;

			sampler = createSampler(samplerDescriptor);
		}

		// Create camera uniform buffer.
		{
			constexpr float32 aspectRatio = 1280.0f / 720.0f;

			const mat4x4<float32> projection = mat4x4<float32>::orthographic(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
			const uint64 size = sizeof(projection);

			BufferDescriptor bufferDescriptor{};
			bufferDescriptor.size = size;
			bufferDescriptor.usage = BufferUsage::UNIFORM;

			cameraUniformBuffer = createBuffer(bufferDescriptor);

			mat4x4<float32>* mappedRange = cameraUniformBuffer->map<mat4x4<float32>>(0, size);

			std::copy(&projection, &projection + 1, mappedRange);

			cameraUniformBuffer->unmap();
		}

		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding{};
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = {cameraUniformBuffer};

		DescriptorSetLayoutBinding imageDescriptorSetLayoutBinding{};
		imageDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		imageDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		imageDescriptorSetLayoutBinding.samplers = {sampler};
		imageDescriptorSetLayoutBinding.imageViews = {imageView};

		DescriptorSetLayout descriptorSetLayout{};
		descriptorSetLayout.bindings.emplace_back(cameraDescriptorSetLayoutBinding);
		descriptorSetLayout.bindings.emplace_back(imageDescriptorSetLayoutBinding);

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts.emplace_back(descriptorSetLayout);

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor;
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/TextureDemo/Shader/Texture.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor;
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("../ProjectA/src/Engine/Debug/TextureDemo/Shader/Texture.frag.spv", fragmentShaderModuleDescriptor);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.renderPass = Application::get().getRenderPass();
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(0, InputRate::VERTEX);
		renderPipelineDescriptor.vertexBufferLayout.add<vec2<float32>>(0, false);
		renderPipelineDescriptor.vertexBufferLayout.add<vec2<float32>>(0, false);
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);

		// Create vertex buffer.
		{
			const float32 w = static_cast<float32>(image->getWidth()) / 1000.0f;
			const float32 h = static_cast<float32>(image->getHeight()) / 1000.0f;

			// Image coordinates are reversed manually here.
			chain<Vertex> vertices(4);
			vertices[0].position = vec2<float32>(-w, -h) / 2.0f;
			vertices[0].uv = {0.0f, 1.0f};
			vertices[1].position = vec2<float32>(-w, h) / 2.0f;
			vertices[1].uv = {0.0f, 0.0f};
			vertices[2].position = vec2<float32>(w, h) / 2.0f;
			vertices[2].uv = {1.0f, 0.0f};
			vertices[3].position = vec2<float32>(w, -h) / 2.0f;
			vertices[3].uv = {1.0f, 1.0f};

			const uint64 size = sizeof(Vertex) * vertices.size();

			BufferDescriptor stagingBufferDescriptor{};
			stagingBufferDescriptor.size = size;
			stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

			const ref<Buffer> stagingBuffer = createBuffer(stagingBufferDescriptor);

			Vertex* mappedRange = stagingBuffer->map<Vertex>(0, size);

			std::copy(vertices.begin(), vertices.end(), mappedRange);

			stagingBuffer->unmap();

			BufferDescriptor vertexBufferDescriptor{};
			vertexBufferDescriptor.size = size;
			vertexBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX;

			vertexBuffer = createBuffer(vertexBufferDescriptor);

			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, 0, 0, size);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			const ref<Device>& device = getDevice();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		// Create index buffer.
		{
			chain<uint16> indices(6);
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 0;
			indices[4] = 2;
			indices[5] = 3;

			const uint64 size = sizeof(uint16) * indices.size();

			BufferDescriptor stagingBufferDescriptor{};
			stagingBufferDescriptor.size = size;
			stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

			const ref<Buffer> stagingBuffer = createBuffer(stagingBufferDescriptor);

			uint16* mappedRange = stagingBuffer->map<uint16>(0, size);

			std::copy(indices.begin(), indices.end(), mappedRange);

			stagingBuffer->unmap();

			BufferDescriptor indexBufferDescriptor{};
			indexBufferDescriptor.size = size;
			indexBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX;

			indexBuffer = createBuffer(indexBufferDescriptor);

			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, 0, 0, size);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			const ref<Device>& device = getDevice();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}
	}

	void TextureDemoRenderer::render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		BeginRenderPassDescriptor beginRenderPassDescriptor;
		beginRenderPassDescriptor.renderPass = Application::get().getRenderPass();
		beginRenderPassDescriptor.clearColor = vec4<float32>(186.0f, 113.0f, 55.0f, 255.0f) / 255.0f;
		beginRenderPassDescriptor.nextImageIndex = nextImageIndex;

		const float32 width = static_cast<float32>(Application::get().getSwapchain()->getExtentWidth());
		const float32 height = static_cast<float32>(Application::get().getSwapchain()->getExtentHeight());

		commandEncoder->beginRenderPass(beginRenderPassDescriptor);
		commandEncoder->bindRenderPipeline(renderPipeline);
		commandEncoder->bindDescriptorSets(0);
		commandEncoder->bindIndexBuffer(indexBuffer, 0, IndexType::UINT16);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, sizeof(Vertex));
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, Application::get().getSwapchain()->getExtentWidth(), Application::get().getSwapchain()->getExtentHeight());
		commandEncoder->drawIndexed(6, 1, 0);
		commandEncoder->endRenderPass();
	}
}