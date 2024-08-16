#include "pch.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"
#include "Renderer.hpp"

namespace ProjectA {
	Camera* Renderer::camera = nullptr;

	void Renderer::initialize() {
		RenderCommand::initialize();
	}

	void Renderer::beginScene(Camera& camera) {
		Renderer::camera = &camera;
	}

	void Renderer::endScene() {
		Renderer::camera = nullptr;
	}

	void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const mat4x4<float>& transform) {
		shader->bind();

		std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniform("u_viewProjection", Renderer::camera->getViewProjection());
		std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniform("u_transform", transform);

		vertexArray->bind();

		RenderCommand::drawIndexed(vertexArray);
	}
}