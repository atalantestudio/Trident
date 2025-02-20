#include <imgui/imgui.h>

#include "ProjectA/Renderer/2D/Renderer2D.hpp"

#include "SandboxLayer.hpp"

SandboxLayer::SandboxLayer() : Layer("Sandbox"), cameraController(1280.0f / 720.0f), particleSystem(100000) {}

void SandboxLayer::bind() {
	PROFILE_SCOPE(__FUNCSIG__);

	texture = ProjectA::Texture2D::create("assets/textures/checkerboard.png");

	particleProperties.startColor = lyah::vec4<ProjectA::float32>(254.0f, 212.0f, 123.0f, 255.0f) / 255.0f;
	particleProperties.endColor = lyah::vec4<ProjectA::float32>(254.0f, 109.0f, 41.0f, 255.0f) / 255.0f;
	particleProperties.startSize = 0.5f;
	particleProperties.endSize = 0.0f;
	particleProperties.sizeBias = 0.3f;
	particleProperties.lifetime = 5.0f;
	particleProperties.position = {0.0f, 0.0f};
	particleProperties.linearVelocity = {0.0f, 0.0f};
	particleProperties.linearVelocityBias = {3.0f, 1.0f};
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

	{
		PROFILE_SCOPE("Draw");

		static ProjectA::float32 angle = 0.0f;

		angle += deltaTime;

		// NOTE: Z axis is reversed (+ points towards the screen).
		// TODO: Make positive rotation be clockwise
		// TODO: Fix black squares on shapes
		ProjectA::Renderer2D::beginScene(cameraController.getCamera());
		ProjectA::Renderer2D::drawRotatedQuad({1.0f, 0.0f}, {0.8f, 0.8f}, lyah::radians(-45.0f), {0.8f, 0.2f, 0.3f, 1.0f});
		ProjectA::Renderer2D::drawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
		ProjectA::Renderer2D::drawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
		ProjectA::Renderer2D::drawQuad({0.0f, 0.0f, 0.1f}, {20.0f, 20.0f}, texture, 10.0f);
		ProjectA::Renderer2D::drawRotatedQuad({-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, angle, texture, 20.0f);
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

	if (ProjectA::Input::isMouseButtonPressed(ProjectA::MouseButton::BUTTON_LEFT)) {
		const lyah::vec2<ProjectA::float32> mousePosition = ProjectA::Input::getMousePosition();
		const ProjectA::float32 width = static_cast<ProjectA::float32>(ProjectA::Application::get().getWindow().getWidth());
		const ProjectA::float32 height = static_cast<ProjectA::float32>(ProjectA::Application::get().getWindow().getHeight());

		const lyah::vec4<ProjectA::float32>& bounds = cameraController.getBounds();
		const lyah::vec3<ProjectA::float32>& position = cameraController.getPosition();

		const ProjectA::float32 boundsWidth = bounds[1] - bounds[0];
		const ProjectA::float32 boundsHeight = bounds[3] - bounds[2];

		const ProjectA::float32 x = (mousePosition[0] / width) * boundsWidth - boundsWidth * 0.5f;
		const ProjectA::float32 y = -(mousePosition[1] / height) * boundsHeight + boundsHeight * 0.5f;

		particleProperties.position = {position[0] + x, position[1] + y};

		for (ProjectA::uint32 i = 0; i < 50; i++) {
			particleSystem.emitParticle(particleProperties);
		}
	}

	particleSystem.update(deltaTime);
	particleSystem.render(cameraController.getCamera());
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