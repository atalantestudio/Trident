#include "pch.hpp"
#include "Renderer.hpp"

namespace ProjectA {
	Camera* Renderer::camera = nullptr;

	void Renderer::beginScene(Camera& camera) {
		Renderer::camera = &camera;
	}

	void Renderer::endScene() {
		Renderer::camera = nullptr;
	}

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray) {
		shader->bind();
		shader->uploadUniform("u_viewProjection", Renderer::camera->getViewProjection());

		vertexArray->bind();

		RenderCommand::drawIndexed(vertexArray);
	}
}