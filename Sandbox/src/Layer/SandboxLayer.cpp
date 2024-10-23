#include "imgui/imgui.h"
#include "Platform/OpenGL/OpenGLShader.hpp"
#include "ProjectA/Renderer/2D/Renderer2D.hpp"

#include "../Timer.hpp"
#include "SandboxLayer.hpp"

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) {profileResults.push_back(profileResult);})

SandboxLayer::SandboxLayer() : Layer("Sandbox"), cameraController(1280.0f / 720.0f) {}

void SandboxLayer::bind() {
	texture = ProjectA::Texture2D::create("assets/textures/plasterwall030c.jpg");
}

void SandboxLayer::unbind() {}

void SandboxLayer::update(float deltaTime) {
	PROFILE_SCOPE("SandboxLayer::update");

	{
		PROFILE_SCOPE("CameraController::update");

		cameraController.update(deltaTime);
	}

	ProjectA::RenderCommand::setClearColor(ProjectA::vec4<float>(0.1f, 0.1f, 0.1f, 1.0f));
	ProjectA::RenderCommand::clear();

	ProjectA::Renderer2D::beginScene(cameraController.getCamera());
	ProjectA::Renderer2D::drawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
	ProjectA::Renderer2D::drawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
	ProjectA::Renderer2D::drawTexturedQuad({0.0f, 0.0f, 0.1f}, {10.0f, 10.0f}, texture);
	ProjectA::Renderer2D::endScene();
}

void SandboxLayer::renderImGui() {
	ImGui::Begin("Settings");

	ImGui::ColorEdit4("Square color", (std::float_t*) &squareColor);

	for (ProfileResult& profileResult : profileResults) {
		char label[50];

		strcpy(label, "%.3fms - ");
		strcat(label, profileResult.name);

		ImGui::Text(label, profileResult.duration);
	}

	ImGui::End();

	profileResults.clear();
}

void SandboxLayer::onEvent(ProjectA::Event& event) {
	cameraController.onEvent(event);
}