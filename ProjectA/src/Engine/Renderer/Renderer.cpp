#include "pch.hpp"

#include "Engine/Renderer/RenderCommand.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	const Camera* Renderer::camera = nullptr;

	void Renderer::initialize() {
		PROFILE_SCOPE(__FUNCSIG__);

		RenderCommand::initialize();
	}

	void Renderer::destroy() {}

	void Renderer::submit(const ref<RenderPipeline>& renderPipeline, const mat4x4<float32>& transform) {
		PROFILE_SCOPE(__FUNCSIG__);

		renderPipeline->bind();
		renderPipeline->setUniform("u_viewProjection", camera->getViewProjection());
		renderPipeline->setUniform("u_transform", transform);

		RenderCommand::drawIndexed(renderPipeline, renderPipeline->getIndexBuffer()->getCount());
	}

	void Renderer::onWindowResize(uint32 width, uint32 height) {
		RenderCommand::setViewport(0, 0, width, height);
	}
}