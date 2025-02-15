#include <imgui/imgui.h>

#include "ProjectA/Renderer/2D/Renderer2D.hpp"

#include "SandboxLayer.hpp"

SandboxLayer::SandboxLayer() : Layer("Sandbox"), cameraController(1280.0f / 720.0f) {}

void SandboxLayer::bind() {
	PROFILE_SCOPE(__FUNCSIG__);

	texture = ProjectA::Texture2D::create("assets/textures/plasterwall030c.jpg");
}

void SandboxLayer::unbind() {
	PROFILE_SCOPE(__FUNCSIG__);
}

void SandboxLayer::update(ProjectA::float32 deltaTime) {
	PROFILE_SCOPE(__FUNCSIG__);

	cameraController.update(deltaTime);

	{
		PROFILE_SCOPE("Pre-draw");

		ProjectA::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		ProjectA::RenderCommand::clear();
	}

	{
		PROFILE_SCOPE("Draw");

		// NOTE: Z axis is reversed (+ points towards the screen).
		ProjectA::Renderer2D::beginScene(cameraController.getCamera());
		//ProjectA::Renderer2D::drawRotatedQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, lyah::radians(-45.0f), {0.8f, 0.2f, 0.3f, 1.0f});
		ProjectA::Renderer2D::drawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, color);
		ProjectA::Renderer2D::drawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
		ProjectA::Renderer2D::drawQuad({-5.0f, -5.0f, 0.1f}, {10.0f, 10.0f}, texture, 10.0f);
		ProjectA::Renderer2D::drawQuad({-0.5f, -0.5f, 0.01f}, {1.0f, 1.0f}, texture, 20.0f);
		ProjectA::Renderer2D::endScene();
	}
}

void SandboxLayer::renderImGui() {
	PROFILE_SCOPE(__FUNCSIG__);

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Color", color.m.m128_f32);
	ImGui::End();
}

void SandboxLayer::onEvent(ProjectA::Event& event) {
	cameraController.onEvent(event);
}