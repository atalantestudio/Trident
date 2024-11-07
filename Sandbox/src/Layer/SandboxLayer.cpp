#include "imgui/imgui.h"
#include "Platform/OpenGL/OpenGLShader.hpp"
#include "ProjectA/Renderer/2D/Renderer2D.hpp"

#include "SandboxLayer.hpp"

SandboxLayer::SandboxLayer() : Layer("Sandbox"), cameraController(1280.0f / 720.0f) {}

void SandboxLayer::bind() {
	texture = ProjectA::Texture2D::create("assets/textures/plasterwall030c.jpg");
}

void SandboxLayer::unbind() {}

void SandboxLayer::update(float deltaTime) {
	ProjectA::Timer timer(__FUNCSIG__);

	{
		ProjectA::Timer timer("Update camera");

		cameraController.update(deltaTime);
	}

	{
		ProjectA::Timer timer("Pre-draw");

		ProjectA::RenderCommand::setClearColor(ProjectA::vec4<float>(0.1f, 0.1f, 0.1f, 1.0f));
		ProjectA::RenderCommand::clear();
	}

	{
		ProjectA::Timer timer("Draw");

		ProjectA::Renderer2D::beginScene(cameraController.getCamera());
		ProjectA::Renderer2D::drawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
		ProjectA::Renderer2D::drawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
		ProjectA::Renderer2D::drawTexturedQuad({0.0f, 0.0f, 0.1f}, {10.0f, 10.0f}, texture);
		ProjectA::Renderer2D::endScene();
	}
}

void SandboxLayer::renderImGui() {
	ProjectA::Timer timer(__FUNCSIG__);

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square color", (std::float_t*) &squareColor);
	ImGui::End();
}

void SandboxLayer::onEvent(ProjectA::Event& event) {
	cameraController.onEvent(event);
}