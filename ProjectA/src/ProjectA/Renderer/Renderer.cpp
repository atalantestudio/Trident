#include "pch.hpp"

#include "ProjectA/Renderer/Renderer.hpp"

namespace ProjectA {
	const Camera* Renderer::camera = nullptr;

	void Renderer::initialize() {
		PROFILE_SCOPE(__FUNCSIG__);

		RenderCommand::initialize();
	}

	void Renderer::destruct() {}

	void Renderer::beginScene(const Camera& camera) {
		PROFILE_SCOPE(__FUNCSIG__);

		Renderer::camera = &camera;
	}

	void Renderer::endScene() {
		PROFILE_SCOPE(__FUNCSIG__);

		Renderer::camera = nullptr;
	}

	void Renderer::submit(const ref<Shader>& shader, const ref<VertexArray>& vertexArray, const mat4x4<float>& transform) {
		PROFILE_SCOPE(__FUNCSIG__);

		shader->bind();
		shader->setUniform("u_viewProjection", Renderer::camera->getViewProjection());
		shader->setUniform("u_transform", transform);

		vertexArray->bind();

		RenderCommand::drawIndexed(vertexArray, vertexArray->getIndexBuffer()->getCount());
	}

	void Renderer::onWindowResize(uint32 width, uint32 height) {
		RenderCommand::setViewport(0, 0, width, height);
	}
}