#pragma once

#include "ProjectA.hpp"

class SandboxLayer : public ProjectA::Layer {
	private:
		ProjectA::Ref<ProjectA::VertexArray> squareVertexArray;
		ProjectA::Ref<ProjectA::Shader> colorShader;

		ProjectA::OrthographicCameraController cameraController;

		ProjectA::vec4<float> squareColor = {0.2f, 0.3f, 0.8f, 1.0f};

	public:
		SandboxLayer();

		virtual ~SandboxLayer() = default;

		void bind() override;
		void unbind() override;

		void update(float deltaTime) override;

		virtual void renderImGui() override;

		void onEvent(ProjectA::Event& event) override;
};