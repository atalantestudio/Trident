#include "pch.hpp"

#include "Backend/OpenGL/OpenGLGraphicsAdapter.hpp"
#include "Backend/Vulkan/VulkanGraphicsAdapter.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	GraphicsAPI Renderer::graphicsApi = GraphicsAPI::NONE;

	GLFWwindow* Renderer::window = nullptr;

	scope<GraphicsAdapter> Renderer::adapter = nullptr;

	void Renderer::registerWindow(GLFWwindow* window) {
		ENGINE_ASSERT(Renderer::window == nullptr, "The window has already been registered.");

		Renderer::window = window;
	}

	GraphicsAPI Renderer::getGraphicsApi() {
		return graphicsApi;
	}

	const scope<GraphicsAdapter>& Renderer::getBackend() {
		return adapter;
	}

	void Renderer::setGraphicsApi(GraphicsAPI api) {
		graphicsApi = api;
	}

	void Renderer::recreate() {
		ENGINE_ASSERT(window != nullptr, "The window hasn't been registered.");

		switch (graphicsApi) {
			case GraphicsAPI::NONE:
				ENGINE_WARN("No GPU backend selected.");

				break;
			case GraphicsAPI::OPENGL:
				ENGINE_INFO("Using OpenGL backend.");

				adapter = createScope<OpenGLGraphicsAdapter>(window);

				break;
			case GraphicsAPI::VULKAN:
				ENGINE_INFO("Using Vulkan backend.");

				adapter = createScope<VulkanGraphicsAdapter>(window);

				break;
		}

		ENGINE_ASSERT(adapter != nullptr, "Failed to initialize the graphics adapter.");
	}

	void Renderer::getWindowExtent(sint32& extentWidth, sint32& extentHeight) {
		glfwGetWindowSize(window, &extentWidth, &extentHeight);
	}

	ref<Device> Renderer::getDevice() {
		return adapter->getDevice();
	}

	ref<Buffer> Renderer::createBuffer(const BufferDescriptor& descriptor) {
		return adapter->createBuffer(descriptor);
	}

	ref<Buffer> Renderer::createStagingBuffer(uint64 size) {
		BufferDescriptor stagingBufferDescriptor{};
		stagingBufferDescriptor.size = size;
		stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		return adapter->createBuffer(stagingBufferDescriptor);
	}

	ref<ComputePipeline> Renderer::createComputePipeline(const ComputePipelineDescriptor& descriptor) {
		return adapter->createComputePipeline(descriptor);
	}

	ref<Fence> Renderer::createFence() {
		return adapter->createFence();
	}

	ref<Image> Renderer::createImage(const ImageDescriptor& descriptor) {
		return adapter->createImage(descriptor);
	}

	ref<PipelineLayout> Renderer::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) {
		return adapter->createPipelineLayout(descriptor);
	}

	ref<RenderPass> Renderer::createRenderPass(const RenderPassDescriptor& descriptor) {
		return adapter->createRenderPass(descriptor);
	}

	ref<RenderPipeline> Renderer::createRenderPipeline(const RenderPipelineDescriptor& descriptor) {
		return adapter->createRenderPipeline(descriptor);
	}

	ref<Sampler> Renderer::createSampler(const SamplerDescriptor& descriptor) {
		return adapter->createSampler(descriptor);
	}

	ref<Semaphore> Renderer::createSemaphore() {
		return adapter->createSemaphore();
	}

	ref<ShaderModule> Renderer::loadShaderModule(const std::string& path, const ShaderModuleDescriptor& descriptor) {
		const chain<uint8> code = Filesystem::loadByteBuffer(path);

		return adapter->createShaderModule(code, descriptor);
	}
}