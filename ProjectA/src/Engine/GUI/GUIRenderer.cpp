#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/GUIRenderer.hpp"

namespace ProjectA {
	GUIRenderer::GUIRenderer() :
		cameraUniformBuffers(MAX_FRAMES_IN_FLIGHT),
		cameraUniformBufferMappedRanges(MAX_FRAMES_IN_FLIGHT)
	{
		BufferDescriptor cameraUniformBufferDescriptor{};
		cameraUniformBufferDescriptor.size = sizeof(UniformBufferObject);
		cameraUniformBufferDescriptor.usage = BufferUsage::UNIFORM;

		for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
			cameraUniformBuffers[i] = createBuffer(cameraUniformBufferDescriptor);
			cameraUniformBufferMappedRanges[i] = cameraUniformBuffers[i]->map(0, cameraUniformBufferDescriptor.size);
		}

		//BufferDescriptor colorStorageBufferDescriptor{};
		//colorStorageBufferDescriptor.size = sizeof(vec4<float32>) * MAX_COLORS;
		//colorStorageBufferDescriptor.usage = BufferUsage::STORAGE;

		//colorStorageBuffer = createBuffer(colorStorageBufferDescriptor);

		const TextureViewDescriptor textureViewDescriptor;

		defaultTexture = loadDefaultTexture();
		defaultTextureView = defaultTexture->createView(textureViewDescriptor);

		fontTexture = loadTexture("../ProjectA/src/Asset/Texture/Quiver.png");
		fontTextureView = fontTexture->createView(textureViewDescriptor);

		SamplerDescriptor samplerDescriptor{};
		samplerDescriptor.magFilter = SamplerFilter::NEAREST;
		samplerDescriptor.minFilter = SamplerFilter::NEAREST;

		sampler = createSampler(samplerDescriptor);

		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding{};
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;
		cameraDescriptorSetLayoutBinding.buffers = cameraUniformBuffers;

		DescriptorSetLayoutBinding textureDescriptorSetLayoutBinding{};
		textureDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		textureDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		textureDescriptorSetLayoutBinding.textureViews = {fontTextureView};
		textureDescriptorSetLayoutBinding.samplers = {sampler};

		//DescriptorSetLayoutBinding colorDescriptorSetLayoutBinding{};
		//colorDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		//colorDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;
		//colorDescriptorSetLayoutBinding.buffers = {colorStorageBuffer};

		DescriptorSetLayout cameraAndColorDescriptorSetLayout;
		cameraAndColorDescriptorSetLayout.bindings.emplace_back(cameraDescriptorSetLayoutBinding);
		cameraAndColorDescriptorSetLayout.bindings.emplace_back(textureDescriptorSetLayoutBinding);
		//cameraAndColorDescriptorSetLayout.bindings.emplace_back(colorDescriptorSetLayoutBinding);

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts.emplace_back(cameraAndColorDescriptorSetLayout);

		pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		ShaderModuleDescriptor vertexShaderModuleDescriptor;
		vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		vertexShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> vertexShaderModule = loadShaderModule("../ProjectA/src/Engine/Shader/GUI/Subcomponent.vert.spv", vertexShaderModuleDescriptor);

		ShaderModuleDescriptor fragmentShaderModuleDescriptor;
		fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		fragmentShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> fragmentShaderModule = loadShaderModule("../ProjectA/src/Engine/Shader/GUI/Subcomponent.frag.spv", fragmentShaderModuleDescriptor);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.renderPass = Application::get().getRenderPass();
		renderPipelineDescriptor.pipelineLayout = pipelineLayout;
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(0, InputRate::VERTEX);
		renderPipelineDescriptor.vertexBufferLayout.add<vec3<float32>>(0, false);
		renderPipelineDescriptor.vertexBufferLayout.add<vec2<float32>>(0, false);
		renderPipelineDescriptor.vertexBufferLayout.defineBinding(1, InputRate::INSTANCE);
		renderPipelineDescriptor.vertexBufferLayout.add<mat3x3<float32>>(1, false);
		renderPipelineDescriptor.vertexBufferLayout.add<uint32>(1, false);
		renderPipelineDescriptor.vertexBufferLayout.add<float32>(1, false);
		renderPipelineDescriptor.vertexShaderModule = vertexShaderModule;
		renderPipelineDescriptor.fragmentShaderModule = fragmentShaderModule;
		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		renderPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		renderPipelineDescriptor.cullMode = CullMode::BACK;

		renderPipeline = createRenderPipeline(renderPipelineDescriptor);

		// Create position vertex buffer.
		{
			// Texture coordinates are reversed manually.
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

			BufferMappedRange mappedRange = positionVertexBuffer->map(0, bufferDescriptor.size);

			std::copy(vertices.begin(), vertices.end(), static_cast<Vertex*>(mappedRange));

			positionVertexBuffer->unmap();
		}

		BufferDescriptor textureCoordinateVertexBufferDescriptor{};
		textureCoordinateVertexBufferDescriptor.size = sizeof(vec2<float32>) * MAX_QUADS * 4;
		textureCoordinateVertexBufferDescriptor.usage = BufferUsage::VERTEX;

		textureCoordinateVertexBuffer = createBuffer(textureCoordinateVertexBufferDescriptor);

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

			BufferMappedRange mappedRange = indexBuffer->map(0, bufferDescriptor.size);

			std::copy(indices.begin(), indices.end(), static_cast<uint16*>(mappedRange));

			indexBuffer->unmap();
		}

		// Create command encoder.
		{
			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

			commandEncoder = createCommandEncoder(commandBufferDescriptor);
		}
	}

	ref<Texture> GUIRenderer::loadTexture(const std::string& path) {
		const PNGFile file = Filesystem::load<PNGFile>(path);
		const uint64 size = sizeof(uint8) * file.data.size();

		BufferDescriptor stagingBufferDescriptor{};
		stagingBufferDescriptor.size = size;
		stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> stagingBuffer = createBuffer(stagingBufferDescriptor);

		const BufferMappedRange mappedRange = stagingBuffer->map(0, size);

		std::copy(file.data.begin(), file.data.end(), static_cast<uint8*>(mappedRange));

		stagingBuffer->unmap();

		return loadTexture(stagingBuffer, file.width, file.height);
	}

	ref<Texture> GUIRenderer::loadTexture(const ref<Buffer>& buffer, uint32 width, uint32 height) {
		TextureDescriptor textureDescriptor{};
		textureDescriptor.type = TextureType::TEXTURE_2D;
		textureDescriptor.format = TextureFormat::R8G8B8A8_SRGB;
		textureDescriptor.usage = TextureUsage::TRANSFER_DST | TextureUsage::SAMPLED;
		textureDescriptor.width = width;
		textureDescriptor.height = height;
		textureDescriptor.depth = 1;
		textureDescriptor.mipLevelCount = 1;
		textureDescriptor.arrayLayerCount = 1;
		textureDescriptor.sampleCount = 1;

		const ref<Texture> texture = createTexture(textureDescriptor);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setTextureMemoryBarrier(texture, TextureLayout::UNDEFINED, TextureLayout::TRANSFER_DST_OPTIMAL);
		commandEncoder->copyBufferToTexture(buffer, texture, width, height, 0);
		commandEncoder->setTextureMemoryBarrier(texture, TextureLayout::TRANSFER_DST_OPTIMAL, TextureLayout::SHADER_READ_ONLY_OPTIMAL);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		const ref<Device>& device = getDevice();

		device->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();

		return texture;
	}

	ref<Texture> GUIRenderer::loadDefaultTexture() {
		const std::array<uint8, 4> data = {1, 1, 1, 1};
		const uint64 size = sizeof(uint8) * data.size();

		BufferDescriptor stagingBufferDescriptor{};
		stagingBufferDescriptor.size = size;
		stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> stagingBuffer = createBuffer(stagingBufferDescriptor);

		const BufferMappedRange mappedRange = stagingBuffer->map(0, size);

		std::copy(data.begin(), data.end(), static_cast<uint8*>(mappedRange));

		stagingBuffer->unmap();

		return loadTexture(stagingBuffer, 1, 1);
	}

	void GUIRenderer::registerEntityPool(const ComponentPool<GUIComponent>& componentPool) {
		instanceCount = componentPool.componentCount;

		/*BufferMappedRange mappedRange = textureCoordinateVertexBuffer->map(0, sizeof(vec2<float32>) * instanceCount * 4);

		std::copy(componentPool.components.begin(), componentPool.components.begin() + instanceCount, static_cast<GUIComponent*>(mappedRange));

		textureCoordinateVertexBuffer->unmap();*/

		BufferMappedRange mappedRange = transformVertexBuffer->map(0, sizeof(GUIComponent) * instanceCount);

		std::copy(componentPool.components.begin(), componentPool.components.begin() + instanceCount, static_cast<GUIComponent*>(mappedRange));

		transformVertexBuffer->unmap();
	}

	void GUIRenderer::registerColors(const chain<vec4<float32>>& colors) const {
		BufferMappedRange mappedRange = colorStorageBuffer->map(0, sizeof(vec4<float32>) * colors.size());

		std::copy(colors.begin(), colors.end(), static_cast<vec4<float32>*>(mappedRange));

		colorStorageBuffer->unmap();
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

		UniformBufferObject cameraUniformBufferObject;
		cameraUniformBufferObject.projection = {
			2.0f / width, 0.0f, 0.0f,
			0.0f, -2.0f / height, 0.0f,
			-1.0f, 1.0f, 1.0f,
		};

		std::copy(&cameraUniformBufferObject, &cameraUniformBufferObject + 1, static_cast<UniformBufferObject*>(cameraUniformBufferMappedRanges[frameIndex]));
	}

	void GUIRenderer::onEvent(Event& event) {}
}