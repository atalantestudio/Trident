#include <imgui/imgui.h>

#include "ProjectA/Renderer/2D/Renderer2D.hpp"

#include "SandboxLayer.hpp"

SandboxLayer::SandboxLayer() : Layer("Sandbox"), cameraController(1280.0f / 720.0f) {}

void SandboxLayer::bind() {
	PROFILE_SCOPE(__FUNCSIG__);

	checkerboardTexture = ProjectA::Texture2D::create("assets/textures/checkerboard.png");
}

void SandboxLayer::unbind() {
	PROFILE_SCOPE(__FUNCSIG__);
}

void SandboxLayer::update(ProjectA::float32 deltaTime) {
	PROFILE_SCOPE(__FUNCSIG__);

	cameraController.update(deltaTime);

	ProjectA::Renderer2D::resetStatistics();

	{
		PROFILE_SCOPE("Pre-draw");

		ProjectA::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		ProjectA::RenderCommand::clear();
	}

	ProjectA::Renderer2D::beginScene(cameraController.getCamera());

	static ProjectA::float32 angle = 0.0f;

	angle += deltaTime;

	// Z axis is reversed (+ points towards the screen).
	// TODO: Inverse rotation direction?
	// Z rotation is clockwise against axis (0, 0, -1).
	// TODO: Fix black squares on shapes
	ProjectA::Renderer2D::drawRotatedQuad({1.0f, 0.0f}, {0.8f, 0.8f}, lyah::radians(45.0f), {0.8f, 0.2f, 0.3f, 1.0f});
	ProjectA::Renderer2D::drawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
	ProjectA::Renderer2D::drawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
	ProjectA::Renderer2D::drawQuad({0.0f, 0.0f, 0.1f}, {20.0f, 20.0f}, checkerboardTexture, 10.0f);
	ProjectA::Renderer2D::drawRotatedQuad({-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, angle, checkerboardTexture, 20.0f);
	ProjectA::Renderer2D::endScene();

	ProjectA::Renderer2D::beginScene(cameraController.getCamera());

	for (ProjectA::float32 y = -5.0f; y < 5.0f; y += 0.5f) {
		for (ProjectA::float32 x = -5.0f; x < 5.0f; x += 0.5f) {
			lyah::vec2<ProjectA::float32> position = {x, y};
			lyah::vec2<ProjectA::float32> size = {0.45f, 0.45f};
			lyah::vec4<ProjectA::float32> color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f};

			ProjectA::Renderer2D::drawQuad(position, size, color);
		}
	}

	ProjectA::Renderer2D::endScene();
}

void SandboxLayer::renderImGui() {
	PROFILE_SCOPE(__FUNCSIG__);

	ProjectA::Renderer2D::Statistics statistics = ProjectA::Renderer2D::getStatistics();

	ImGui::Begin("Settings");

	ImGui::Text("Per-frame statistics:");
	ImGui::Text("Draw call count: %d", statistics.drawCallCount);
	ImGui::Text("Quad count: %d", statistics.quadCount);
	ImGui::Text("Total vertex count: %d", statistics.getTotalVertexCount());
	ImGui::Text("Total index count: %d", statistics.getTotalIndexCount());
	ImGui::ColorEdit4("Color", color.m.m128_f32);

	ImGui::End();
}

void SandboxLayer::onEvent(ProjectA::Event& event) {
	cameraController.onEvent(event);
}