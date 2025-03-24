#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRendererAPI.hpp"

#include "Backend/Vulkan/VulkanRendererAPI.hpp"

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

	void Renderer::setGraphicsApi(const GraphicsAPI api) {
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

	ref<Device> Renderer::getDevice() const {
		return rendererApi->getDevice();
	}

	void Renderer::getWindowExtent(sint32& extentWidth, sint32& extentHeight) const {
		glfwGetWindowSize(window, &extentWidth, &extentHeight);
	}

	ref<Buffer> Renderer::createBuffer(const BufferDescriptor& descriptor) const {
		return rendererApi->createBuffer(descriptor);
	}

	ref<CommandEncoder> Renderer::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return rendererApi->createCommandEncoder(descriptor);
	}

	ref<Fence> Renderer::createFence() const {
		return rendererApi->createFence();
	}

	ref<PipelineLayout> Renderer::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return rendererApi->createPipelineLayout(descriptor);
	}

	ref<RenderPass> Renderer::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return rendererApi->createRenderPass(descriptor);
	}

	ref<RenderPipeline> Renderer::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return rendererApi->createRenderPipeline(descriptor);
	}

	ref<Sampler> Renderer::createSampler(const SamplerDescriptor& descriptor) const {
		return rendererApi->createSampler(descriptor);
	}

	ref<Semaphore> Renderer::createSemaphore() const {
		return rendererApi->createSemaphore();
	}

	ref<ShaderModule> Renderer::createShaderModule(const ShaderStage stage, const chain<uint8>& code) const {
		return rendererApi->createShaderModule(stage, code);
	}

	ref<Swapchain> Renderer::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return rendererApi->createSwapchain(descriptor);
	}

	ref<Texture> Renderer::createTexture(const TextureDescriptor& descriptor) const {
		return rendererApi->createTexture(descriptor);
	}
}