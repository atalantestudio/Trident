#pragma once

#include "ProjectA.hpp"

#include "../ParticleSystem.hpp"

class SandboxLayer : public ProjectA::Layer {
	private:
		ProjectA::OrthographicCamera3DController cameraController;
		ProjectA::vec4<ProjectA::float32> color = {0.2f, 0.3f, 0.8f, 1.0f};
		ProjectA::ref<ProjectA::Texture> checkerboardTexture;

	public:
		SandboxLayer();

		virtual ~SandboxLayer() = default;

		void bind() override;
		void unbind() override;

		void update(ProjectA::float32 deltaTime, const ProjectA::ref<ProjectA::CommandEncoder>& commandEncoder, ProjectA::uint32 nextImageIndex) override;

		virtual void renderImGui() override;

		void onEvent(ProjectA::Event& event) override;
};