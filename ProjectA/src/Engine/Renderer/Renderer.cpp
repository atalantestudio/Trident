#include "pch.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

#include "Platform/Vulkan/VulkanRendererAPI.hpp"

namespace ProjectA {
	GraphicsAPI Renderer::graphicsApi = GraphicsAPI::VULKAN;

	const Camera* Renderer::camera = nullptr;

	scope<RendererAPI> Renderer::rendererApi = nullptr;

	void Renderer::initializeGraphicsApi(GLFWwindow* window) {
		PROFILE_SCOPE(FUNC_SIG);

		switch (graphicsApi) {
			case GraphicsAPI::NONE:
				ENGINE_WARN("No GPU backend selected.");

				break;
			case GraphicsAPI::OPENGL:
				ENGINE_INFO("Using OpenGL backend.");

				rendererApi = createScope<OpenGLRendererAPI>();

				break;
			case GraphicsAPI::VULKAN:
				ENGINE_INFO("Using Vulkan backend.");

				rendererApi = createScope<VulkanRendererAPI>(window);

				break;
		}

		ENGINE_ASSERT(rendererApi != nullptr, "GraphicsAPI::NONE is not supported.");
	}

	GraphicsAPI Renderer::getGraphicsApi() {
		return graphicsApi;
	}

	scope<RendererAPI>& Renderer::getRendererApi() {
		return rendererApi;
	}

	/*void Renderer::submit(const ref<RenderPipeline>& renderPipeline) {
		PROFILE_SCOPE(FUNC_SIG);

		renderPipeline->bind();
		renderPipeline->setUniform("u_projectionView", camera->projectionView);

		rendererApi->drawIndexed(renderPipeline, renderPipeline->getIndexBuffer()->getCount());
	}*/

	void Renderer::onWindowResize(uint32 width, uint32 height) {
		rendererApi->setViewport(0, 0, width, height);
	}

	// --- Start of new Renderer parent class ---

	ref<Buffer> Renderer::createBuffer(const BufferDescriptor& descriptor) {
		return rendererApi->createBuffer(descriptor);
	}

	ref<CommandBuffer> Renderer::createCommandBuffer() {
		return rendererApi->createCommandBuffer();
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

	ref<ShaderModule> Renderer::createShaderModule(const ShaderStage stage, const std::string& code) {
		return rendererApi->createShaderModule(stage, code);
	}

	ref<Swapchain> Renderer::createSwapchain(const SwapchainDescriptor& descriptor) {
		return rendererApi->createSwapchain(descriptor);
	}
}