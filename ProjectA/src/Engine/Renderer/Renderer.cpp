#include "pch.hpp"

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	GraphicsAPI Renderer::graphicsApi = GraphicsAPI::VULKAN;

	const Camera* Renderer::camera = nullptr;

	scope<RendererAPI> Renderer::rendererApi = nullptr;

	void Renderer::initialize() {
		PROFILE_SCOPE(FUNC_SIG);

		switch (graphicsApi) {
			case GraphicsAPI::NONE:
				ENGINE_WARN("No GPU backend selected.");

				break;
			case GraphicsAPI::OPENGL:
				ENGINE_INFO("Using OpenGL backend.");

				break;
			case GraphicsAPI::VULKAN:
				ENGINE_INFO("Using Vulkan backend.");

				break;
		}

		rendererApi = RendererAPI::create();
	}

	void Renderer::destroy() {}

	GraphicsAPI Renderer::getGraphicsApi() {
		return graphicsApi;
	}

	scope<RendererAPI>& Renderer::getRendererApi() {
		return rendererApi;
	}

	void Renderer::submit(const ref<RenderPipeline>& renderPipeline, const mat4x4<float32>& transform) {
		PROFILE_SCOPE(FUNC_SIG);

		renderPipeline->bind();
		// TODO: Rename to "u_projectionView"
		renderPipeline->setUniform("u_viewProjection", camera->getProjectionView());
		renderPipeline->setUniform("u_transform", transform);

		rendererApi->drawIndexed(renderPipeline, renderPipeline->getIndexBuffer()->getCount());
	}

	void Renderer::onWindowResize(uint32 width, uint32 height) {
		rendererApi->setViewport(0, 0, width, height);
	}

	// --- Start of new Renderer parent class ---

	ref<ShaderModule> Renderer::createShaderModule(const ShaderStage stage, const std::string& code) {
		return rendererApi->createShaderModule(stage, code);
	}
}