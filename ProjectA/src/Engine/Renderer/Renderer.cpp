#include "pch.hpp"

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	GraphicsAPI Renderer::graphicsApi = GraphicsAPI::OPENGL;

	const Camera* Renderer::camera = nullptr;

	scope<RendererAPI> Renderer::rendererApi = RendererAPI::create();

	void Renderer::initialize() {
		PROFILE_SCOPE(FUNC_SIG);

		rendererApi->initialize();
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
		renderPipeline->setUniform("u_viewProjection", camera->getViewProjection());
		renderPipeline->setUniform("u_transform", transform);

		rendererApi->drawIndexed(renderPipeline, renderPipeline->getIndexBuffer()->getCount());
	}

	void Renderer::onWindowResize(uint32 width, uint32 height) {
		rendererApi->setViewport(0, 0, width, height);
	}
}