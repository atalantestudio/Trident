#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRendererAPI.hpp"
#include "Backend/Vulkan/VulkanRendererAPI.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	GraphicsAPI Renderer::graphicsApi = GraphicsAPI::NONE;

	GLFWwindow* Renderer::window = nullptr;

	scope<RendererAPI> Renderer::rendererApi = nullptr;

	void Renderer::registerWindow(GLFWwindow* window) {
		ENGINE_ASSERT(Renderer::window == nullptr, "The window has already been registered.");

		Renderer::window = window;
	}

	GraphicsAPI Renderer::getGraphicsApi() {
		return graphicsApi;
	}

	const scope<RendererAPI>& Renderer::getBackend() {
		return rendererApi;
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

				rendererApi = createScope<OpenGLRendererAPI>(window);

				break;
			case GraphicsAPI::VULKAN:
				ENGINE_INFO("Using Vulkan backend.");

				rendererApi = createScope<VulkanRendererAPI>(window);

				break;
		}

		ENGINE_ASSERT(rendererApi != nullptr, "Failed to create a corresponding renderer API.");
	}

	void Renderer::getWindowExtent(sint32& extentWidth, sint32& extentHeight) {
		glfwGetWindowSize(window, &extentWidth, &extentHeight);
	}

	ref<Device> Renderer::getDevice() {
		return rendererApi->getDevice();
	}

	ref<Buffer> Renderer::createBuffer(const BufferDescriptor& descriptor) {
		return rendererApi->createBuffer(descriptor);
	}

	ref<CommandEncoder> Renderer::createCommandEncoder(const CommandBufferDescriptor& descriptor) {
		return rendererApi->createCommandEncoder(descriptor);
	}

	ref<Fence> Renderer::createFence() {
		return rendererApi->createFence();
	}

	ref<PipelineLayout> Renderer::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) {
		return rendererApi->createPipelineLayout(descriptor);
	}

	ref<RenderPass> Renderer::createRenderPass(const RenderPassDescriptor& descriptor) {
		return rendererApi->createRenderPass(descriptor);
	}

	ref<RenderPipeline> Renderer::createRenderPipeline(const RenderPipelineDescriptor& descriptor) {
		return rendererApi->createRenderPipeline(descriptor);
	}

	ref<Sampler> Renderer::createSampler(const SamplerDescriptor& descriptor) {
		return rendererApi->createSampler(descriptor);
	}

	ref<Semaphore> Renderer::createSemaphore() {
		return rendererApi->createSemaphore();
	}

	ref<Texture> Renderer::createTexture(const TextureDescriptor& descriptor) {
		return rendererApi->createTexture(descriptor);
	}

	ref<ShaderModule> Renderer::loadShaderModule(const std::string& path, const ShaderModuleDescriptor& descriptor) {
		const chain<uint8> code = Filesystem::loadByteBuffer(path);

		return rendererApi->createShaderModule(descriptor, code);
	}
}