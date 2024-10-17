#include "imgui/imgui.h"
#include "Platform/OpenGL/OpenGLShader.hpp"

#include "SandboxLayer.hpp"

SandboxLayer::SandboxLayer() : Layer("Sandbox"), cameraController(1280.0f / 720.0f) {}

void SandboxLayer::bind() {
	squareVertexArray = ProjectA::VertexArray::create();

	std::float_t squareVertices[4 * 3] = {
		-0.5f, -0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f
	};

	ProjectA::Ref<ProjectA::VertexBuffer> squareVertexBuffer = ProjectA::VertexBuffer::create(squareVertices, sizeof(squareVertices));

	squareVertexBuffer->setLayout({
		{
			ProjectA::ShaderType::Float3, "a_position", false
		}
	});

	squareVertexArray->addVertexBuffer(squareVertexBuffer);

	std::uint32_t squareIndices[6] = {
		0, 1, 2,
		0, 2, 3
	};

	ProjectA::Ref<ProjectA::IndexBuffer> squareIndexBuffer = ProjectA::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(std::uint32_t));

	squareVertexArray->setIndexBuffer(squareIndexBuffer);

	colorShader = ProjectA::Shader::create("assets/shaders/Color.glsl");
}

void SandboxLayer::unbind() {}

void SandboxLayer::update(float deltaTime) {
	cameraController.update(deltaTime);

	ProjectA::RenderCommand::setClearColor(ProjectA::vec4<float>(0.1f, 0.1f, 0.1f, 1.0f));
	ProjectA::RenderCommand::clear();

	ProjectA::Renderer::beginScene(cameraController.getCamera());

	colorShader->bind();
	std::dynamic_pointer_cast<ProjectA::OpenGLShader>(colorShader)->uploadUniform("u_color", squareColor);

	static const ProjectA::mat4x4<float> squareScale(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	ProjectA::Renderer::submit(colorShader, squareVertexArray, squareScale);

	ProjectA::Renderer::endScene();
}

void SandboxLayer::renderImGui() {
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square color", (std::float_t*) &squareColor);
	ImGui::End();
}

void SandboxLayer::onEvent(ProjectA::Event& event) {
	cameraController.onEvent(event);
}