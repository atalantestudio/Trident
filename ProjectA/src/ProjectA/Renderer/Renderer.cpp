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

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const mat4x4<float>& transform) {
		shader->bind();
		shader->uploadUniform("u_viewProjection", Renderer::camera->getViewProjection());
		shader->uploadUniform("u_transform", transform);

		vertexArray->bind();

		RenderCommand::drawIndexed(vertexArray);
	}
}